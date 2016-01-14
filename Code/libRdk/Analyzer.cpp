//Analyzer.cpp

#include "Analyzer.h"
#include "ThreadedAnalyzer.h"
#include "EpgFitFunction.h"
#include "RDKConfig.h"
#include "GFitter.h"

#include "TFitResultPtr.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TF1.h"
#include "TLine.h"
#include "TMarker.h"
//#include "TFitterMinuit.h"
#include "TMath.h"
using namespace TMath;

#include <iostream>
using namespace std;

#define BGO_OFFSET 64
#define BAPD_OFFSET 32
#define MIN_BASELINE 128
#define CHI2MIN 4900

Analyzer::Analyzer(Smoother *es,Smoother *ts,UInt_t c)
:y(2048),y1(2048),y2(2048),m(2048),b(2048),sigma(2048),chi2(2048),status(kFALSE),
config(c){
	eSmoother=es;
	tSmoother=ts;
}

Analyzer::~Analyzer(){
}

Bool_t Analyzer::preanalyze(const RawData & rdata,Recon &recon,Int_t n,Bool_t print){
	mode[n]=RDKConfig::getMode(config,n);
	if(mode[n]==BGO_MODE || mode[n]==BAPD_MODE){
		const Short_t *sig=rdata.ch[n];
		Double_t max=sig[0],min=max,sum0=0,sum1=0,sum2=0;
		chi2max=0;
		Int_t xmax=0,xmin=0;
		for(Int_t i=0;i<2048;i++){
			y[i]=sig[i];
			if(y[i]>max){
				max=y[i];
				xmax=i;
			}else if(y[i]<min){
				min=y[i];
				xmin=i;
			}
			sum0+=y[i];
			sum1+=i*y[i];
			sum2+=y[i]*y[i];
			if(i>1){
				Double_t sigma2=sum2/(i+1)-sum0*sum0/(i+1)/(i+1);
				m[i]=(12*sum1-6*i*sum0)/(i*(i+1.0)*(i+2.0));
				b[i]=((4*i+2)*sum0-6*sum1)/((i+1.0)*(i+2.0));
				sigma[i]=Sqrt(sigma2);
				chi2[i]=sum2-(12*sum1*sum1-12*i*sum0*sum1+i*(4.0*i+2)*sum0*sum0)/(i*(i+1.0)*(i+2.0));
				//R2[i]=1-chi2[i]/(sigma2*(i+1));
				//sigmab[i]=Sqrt(chi2[i]*(4*i+2)/((i-1.0)*(i+1.0)*(i+2.0)));
				//sigmam[i]=Sqrt(chi2[i]*12/((i-1.0)*i*(i+1.0)*(i+2.0)));
				//sigmaAvg[i]=Sqrt(sigmab[i]*sigmab[i]+sigmam[i]*sigmam[i]*i*i/4);
				Double_t temp=chi2[i]/(i-1.0);
				if(temp>chi2max){
					chi2max=temp;
				}
			}else{
				m[i]=0;
				b[i]=0;
				sigma[i]=0;
				chi2[i]=0;
				//R2[i]=0;
				//sigmab[i]=0;
				//sigmam[i]=0;
				//sigmaAvg[i]=0;
			}
		}
		if(xmax<=0){
			xmax=1;
		}else if(xmax>=2047){
			xmax=2046;
		}
		if(xmin<=0){
			xmin=1;
		}else if(xmin>=2047){
			xmin=2046;
		}
		recon.pre_E[n]=max-min;
		recon.disc[n]=Max(y[xmax]-0.5*(y[xmax+1]+y[xmax-1]),0.5*(y[xmin-1]+y[xmin+1])-y[xmin]);
		recon.flat[n]=chi2max;
		recon.sat[n]=max>=MAXHEIGHT;
		recon.tail[n]=xmax<=xmin;
		recon.peak_pos[n]=xmax;
		const Int_t MIN_PEAK_POS=MIN_BASELINE+(mode[n]==BAPD_MODE)?BAPD_OFFSET:BGO_OFFSET;
		if(ThreadedAnalyzer::DEBUG && print){
			cerr<<"Jump: "<<recon.disc[n]<<endl;
			cerr<<"max: "<<max<<endl;
			cerr<<"xmax: "<<xmax<<endl;
			cerr<<"min: "<<min<<endl;
			cerr<<"xmin: "<<xmin<<endl;
			cerr<<"chi2max: "<<recon.flat[n]<<endl;
		}

		status=!recon.sat[n] && recon.disc[n]<300 && !recon.tail[n] && 
			recon.peak_pos[n]>MIN_PEAK_POS && recon.peak_pos[n]<1792 
			&& recon.pre_E[n]>300 && recon.flat[n]>CHI2MIN;
	}else{
		status=kFALSE;
	}
	return status;
}
	
void Analyzer::analyze(const RawData &rdata,Recon &recon,Int_t n,Bool_t print){
	if(status){
		tSmoother->smooth(y,y1);
		eSmoother->smooth(y,y2);
		Double_t max=y2[0],min=y2[0];
		Int_t xmax=0,xmin=0;
		for(Int_t i=0;i<2048;i++){
			if(y2[i]>max){
				max=y2[i];
				xmax=i;
			}else if(y2[i]<min){
				min=y2[i];
				xmin=i;
			}
		}
		if(xmax>=2047){
			xmax=2046;
		}
		if(xmin>=2047){
			xmin=2046;
		}
		Double_t t_gamma=0,E_gamma=0;
		const Int_t offset=(mode[n]==BAPD_MODE)?BAPD_OFFSET:BGO_OFFSET;
		for(Int_t j=xmax-offset;j>MIN_BASELINE && t_gamma==0;j--){
			Int_t i=j+offset;
			Double_t E=max-b[j]-m[j]*xmax,th=0.1*E+b[j]+m[j]*i;
			if(y1[i]<th && chi2[j]<7e3*(j-1)){
				t_gamma=i+1;
				E_gamma=E;
			}
		}
		if(t_gamma>2047){
			t_gamma-=offset;
		}
		recon.adj_E[n]=E_gamma;
		if(GFIT && E_gamma>200 && E_gamma<30000){
			GFitter *fitter=GFitter::getFitter();
			Double_t par[6];
			par[0]=b[t_gamma];
			par[2]=1e-3;
			par[3]=mode[n]==BGO_MODE?5e-3:1;
			par[1]=E_gamma*(par[2]+par[3])/par[3]*Power((par[2]+par[3])/par[2],par[2]/par[3]);
			par[4]=t_gamma;
			par[5]=m[t_gamma];
			TFitResultPtr result=fitter->fit(rdata.ch[n],par);
			Double_t t=result->Parameter(4),B=result->Parameter(1),
				C=result->Parameter(2),D=result->Parameter(3);
			recon.t_gamma[n]=t;
			recon.E_gamma[n]=B*D*Power((C+D)/C,-C/D)/(C+D);
			recon.chi2_gamma[n]=result->Chi2();
			recon.R2_gamma[n]=-1;
			for(Int_t i=0;i<6;i++){
				recon.fitparam[n][i]=result->Parameter(i);
			}
			recon.nfit[n]=result->NCalls();
			recon.noise[n]=t_gamma>1?Sqrt(chi2[t_gamma-1]):-1;
			recon.t_rise[n]=Log((C+D)/C)/D;
			if(ThreadedAnalyzer::DEBUG && print){
				TF1 *f1=fitter->getFunc();
				for(Int_t i=0;i<2048;i++){
					ThreadedAnalyzer::original->SetPoint(i,0.04*i,y[i]);
					ThreadedAnalyzer::smooth1->SetPoint(i,0.04*i,f1->Eval(i));
				}
				Int_t t_max=t+recon.t_rise[n];
				ThreadedAnalyzer::original->Draw("al");
				ThreadedAnalyzer::original->GetXaxis()->SetRangeUser(0,2048*0.04);
				ThreadedAnalyzer::smooth1->Draw("same");
				ThreadedAnalyzer::marker1->DrawMarker(t*0.04,f1->Eval(t));
				ThreadedAnalyzer::marker2->DrawMarker(t_max*0.04,f1->Eval(t_max));
				TAxis *temp=ThreadedAnalyzer::original->GetYaxis();
				Double_t a1=temp->GetXmin(),a2=temp->GetXmax();
				ThreadedAnalyzer::t_line->DrawLine(t*0.04,a1,t*0.04,a2);
				ThreadedAnalyzer::E_line->DrawLine(0,f1->Eval(t_max),2048*0.04,f1->Eval(t_max));
				cerr<<"chi2[end]="<<chi2[2047]/2046<<endl;
			}
		}else{
			recon.t_gamma[n]=t_gamma;
			recon.E_gamma[n]=E_gamma;
			recon.chi2_gamma[n]=chi2max;
			recon.R2_gamma[n]=-1;
			for(Int_t i=0;i<5;i++){
				recon.fitparam[n][i]=0;
			}
			recon.nfit[n]=-1;
			recon.noise[n]=t_gamma>1?TMath::Sqrt(chi2[t_gamma-1]):-1;
			recon.t_rise[n]=xmax-t_gamma;
			if(ThreadedAnalyzer::DEBUG && print){
				for(Int_t i=0;i<2048;i++){
					ThreadedAnalyzer::original->SetPoint(i,0.04*i,y[i]);
					ThreadedAnalyzer::smooth1->SetPoint(i,0.04*i,y1[i]);
					ThreadedAnalyzer::smooth2->SetPoint(i,0.04*i,y2[i]);
				}
				Int_t t_offset=t_gamma>offset?t_gamma-offset:0;
				ThreadedAnalyzer::function->SetParameter(0,b[t_offset]);
				ThreadedAnalyzer::function->SetParameter(1,m[t_offset]/0.04);
				ThreadedAnalyzer::original->Draw("al");
				ThreadedAnalyzer::original->GetXaxis()->SetRangeUser(0,2048*0.04);
				ThreadedAnalyzer::smooth1->Draw("same");
				ThreadedAnalyzer::smooth2->Draw("same");
				ThreadedAnalyzer::function->Draw("same"); 
				ThreadedAnalyzer::marker1->DrawMarker(t_gamma*0.04,y1[t_gamma]);
				ThreadedAnalyzer::marker2->DrawMarker(xmax*0.04,y2[xmax]);
				ThreadedAnalyzer::marker3->DrawMarker(t_offset*0.04,
					ThreadedAnalyzer::function->Eval(t_offset*0.04));
				TAxis *temp=ThreadedAnalyzer::original->GetYaxis();
				Double_t a1=temp->GetXmin(),a2=temp->GetXmax();
				ThreadedAnalyzer::t_line->DrawLine(t_gamma*0.04,a1,t_gamma*0.04,a2);
				ThreadedAnalyzer::E_line->DrawLine(0,y2[xmax],2048*0.04,y2[xmax]);
				cerr<<"chi2[t_offset]="<<chi2[t_offset]/(t_offset-1)<<endl;
				cerr<<"chi2[end]="<<chi2[2047]/2046<<endl;
			}
		}
	}else{
		if(ThreadedAnalyzer::DEBUG && print){
			for(Int_t i=0;i<2048;i++){
				ThreadedAnalyzer::original->SetPoint(i,0.04*i,y[i]);
			}
			ThreadedAnalyzer::original->Draw("al");
			ThreadedAnalyzer::original->GetXaxis()->SetRangeUser(0,2048*0.04);
			ThreadedAnalyzer::function->Draw("same"); 
			TAxis *temp=ThreadedAnalyzer::original->GetYaxis();
			Double_t a1=temp->GetXmin(),a2=temp->GetXmax();
		}
		recon.E_gamma[n]=0;
		recon.t_gamma[n]=0;
		recon.chi2_gamma[n]=chi2max;
		recon.R2_gamma[n]=-1;
		for(Int_t i=0;i<5;i++){
			recon.fitparam[n][i]=0;
		}
		recon.nfit[n]=-1;
		recon.noise[n]=0;
		recon.t_rise[n]=0;
	}
}

void Analyzer::nullevent(Recon &recon,Int_t n,Bool_t print){
	if(ThreadedAnalyzer::DEBUG && print){
		for(Int_t i=0;i<2048;i++){
			ThreadedAnalyzer::original->SetPoint(i,0.04*i,y[i]);
		}
		ThreadedAnalyzer::original->Draw("al");
		ThreadedAnalyzer::original->GetXaxis()->SetRangeUser(0,2048*0.04);
		ThreadedAnalyzer::function->Draw("same"); 
		TAxis *temp=ThreadedAnalyzer::original->GetYaxis();
		Double_t a1=temp->GetXmin(),a2=temp->GetXmax();
	}
	recon.E_gamma[n]=0;
	recon.t_gamma[n]=0;
	recon.chi2_gamma[n]=chi2max;
	recon.R2_gamma[n]=0;
	for(Int_t i=0;i<5;i++){
		recon.fitparam[n][i]=0;
	}
	recon.nfit[n]=-1;
	recon.noise[n]=0;
	recon.t_rise[n]=0;
}

Bool_t Analyzer::GFIT=kTRUE;