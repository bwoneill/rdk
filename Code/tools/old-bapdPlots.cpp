//bapdPlots.cpp

#include <iomanip>
#include <fstream>
using namespace std;

using namespace TMath;

TH1F **on_peak=NULL,**off_peak=NULL,**bg_corrected=NULL;
TH1F ***on_peak_rise=NULL,***off_peak_rise=NULL,***bg_corrected_rise=NULL;
int bapdCh[3]={2,6,13};
double PEAK=-27,ONPEAK=10,OFFPEAK=200,MAX=3e4,MIN=00,SCALE=1.58440369177913990e-3;
double LOW=0.3,MED=0.8,HIGH=5;
char *titles[4]={"3","1","2","total"}, *levels[3]={"low","med","high"};
double bins[69]={0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1,1.5,2,2.5,3,3.5,
4,4.5,5,5.5,6,6.5,7,7.5,8,8.5,9,9.5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,150,200,250,300,340,350,
400,450,500,550,600,650,700,750};
double br[68]={5.847455E-004,4.149669E-004,3.215895E-004,2.624087E-004,2.221101E-004,1.923835E-004,1.697520E-004,
1.517956E-004,1.373767E-004,1.252711E-004,1.151870E-004,1.065647E-004,9.925420E-005,9.279970E-005,8.714597E-005,
8.208609E-005,7.761568E-005,7.368394E-005,5.809210E-004,4.114864E-004,3.185986E-004,2.594194E-004,2.185997E-004,
1.889949E-004,1.664412E-004,1.484886E-004,1.340202E-004,1.218662E-004,1.118622E-004,1.033077E-004,9.599858E-005,
8.954393E-005,8.389637E-005,7.881434E-005,7.454409E-005,7.047267E-005,5.494211E-004,3.796391E-004,2.874420E-004,
2.290062E-004,1.885909E-004,1.589660E-004,1.369651E-004,1.195111E-004,1.053333E-004,9.357072E-005,8.388879E-005,
7.563469E-005,6.873181E-005,6.264634E-005,5.734032E-005,5.266555E-005,4.849950E-005,4.476618E-005,3.094965E-004,
1.675940E-004,9.805126E-005,5.957051E-005,3.059785E-005,5.952440E-006,2.226460E-005,1.321974E-005,7.488177E-006,
3.922879E-006,1.828172E-006,7.031858E-007,1.920408E-007,2.543317E-008};

void bapdPlots(bool MCBINS=false){
	if(on_peak!=NULL){
		delete[] on_peak;
	}
	if(off_peak!=NULL){
		delete[] off_peak;
	}
	if(bg_corrected!=NULL){
		delete[] bg_corrected;
	}
	if(on_peak_rise!=NULL){
		delete[] on_peak_rise;
	}
	if(off_peak_rise!=NULL){
		delete[] off_peak_rise;
	}
	if(bg_corrected_rise!=NULL){
		delete[] bg_corrected_rise;
	}
	on_peak=new TH1F[4];
	off_peak=new TH1F[4];
	bg_corrected=new TH1F[4];
	on_peak_rise=new TH1F[4];
	off_peak_rise=new TH1F[4];
	bg_corrected_rise=new TH1F[4];
	for(int i=0;i<4;i++){
		TString name=Form("E_gamma bAPD %s",titles[i]),riseName=Form("t_rise bAPD %s",titles[i]);
		on_peak_rise[i]=new TH1F[3];
		off_peak_rise[i]=new TH1F[3];
		bg_corrected_rise[i]=new TH1F[3];
		for(int j=0;j<3;j++){
			on_peak_rise[i][j]=new TH1F(Form("onpeak-%s-%s",titles[i],levels[j]),riseName,50,0,2);
			on_peak_rise[i][j]->Sumw2();
			on_peak_rise[i][j]->SetMarkerStyle(kFullCircle);
			off_peak_rise[i][j]=new TH1F(Form("offpeak-%s-%s",titles[i],levels[j]),riseName,50,0,2);
			off_peak_rise[i][j]->Sumw2();
			off_peak_rise[i][j]->SetMarkerStyle(kFullCircle);
			bg_corrected_rise[i][j]=new TH1F(Form("bgc-%s-%s",titles[i],levels[j]),riseName,50,0,2);
			bg_corrected_rise[i][j]->Sumw2();
			bg_corrected_rise[i][j]->SetMarkerStyle(kFullCircle);
		}
		if(!MCBINS){
			on_peak[i]=new TH1F(Form("onpeak-%s",titles[i]),name,2*(int)(MAX*SCALE),0,MAX*SCALE);
			off_peak[i]=new TH1F(Form("offpeak-%s",titles[i]),name,2*(int)(MAX*SCALE),0,MAX*SCALE);
			bg_corrected[i]=new TH1F(Form("bgc-%s",titles[i]),name,2*(int)(MAX*SCALE),0,MAX*SCALE);
		}else{
			on_peak[i]=new TH1F(Form("onpeak-%s",titles[i]),name,68,bins);
			off_peak[i]=new TH1F(Form("offpeak-%s",titles[i]),name,68,bins);
			bg_corrected[i]=new TH1F(Form("bgc-%s",titles[i]),name,68,bins);
		}
		on_peak[i]->Sumw2();
		on_peak[i]->SetMarkerStyle(kFullCircle);
		off_peak[i]->Sumw2();
		off_peak[i]->SetMarkerStyle(kFullCircle);
		bg_corrected[i]->Sumw2();
		bg_corrected[i]->SetMarkerStyle(kFullCircle);
	}
	for(int i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		for(int j=0;j<3;j++){
			int ch=bapdCh[j];
			double E_gamma=recon.E_gamma[ch]*SCALE;
			double max,min;
			if(MCBINS){
				min=bins[0];
				max=bins[68];
			}else{
				min=MIN*SCALE;
				max=MAX*SCALE;
			}
			if(recon.t_gamma[ch]>0 && recon.t_gamma[ch]<NPOINTS && E_gamma<max && E_gamma>min && recon.t_rise[ch]<100 && recon.E_gamma[ch]>200){
				double delta_t=recon.t_gamma[ch]-recon.t_e,w=2;
				double t_rise=recon.t_rise[ch]*0.04;
				int E_level=(E_gamma>=LOW && E_gamma<MED?0:(E_gamma>=MED && E_gamma<HIGH?1:(E_gamma>=HIGH?2:-1)));
				if(MCBINS){
					for(int k=0;k<68;k++){
						if(E_gamma>=bins[k] && E_gamma<=bins[k+1]){
							w=1/(bins[k+1]-bins[k]);
							break;
						}
					}
				}
				if(fabs(delta_t-PEAK)<=ONPEAK){
					on_peak[j]->Fill(E_gamma,w);
					on_peak[3]->Fill(E_gamma,w);
					if(E_level>=0){
						on_peak_rise[j][E_level]->Fill(t_rise[ch]);
						on_peak_rise[3][E_level]->Fill(t_rise[ch]);
					}
				}else if(fabs(delta_t-PEAK)<=OFFPEAK){
					off_peak[j]->Fill(E_gamma,w);
					off_peak[3]->Fill(E_gamma,w);
					if(E_level>=0){
						off_peak_rise[j][E_level]->Fill(t_rise[ch]);
						off_peak_rise[3][E_level]->Fill(t_rise[ch]);
					}
				}
			}
		}
	}
	double w=(ONPEAK+0.5)/(ONPEAK-OFFPEAK);
	for(int i=0;i<4;i++){
		bg_corrected[i]->Add(on_peak[i],off_peak[i],1,w);
		for(int j=0;j<3;j++){
			bg_corrected_rise[i][j]->Add(on_peak_rise[i][j],off_peak_rise[i][j],1,w);
		}
	}
	saveBAPD(MCBINS);
}

void saveBAPD(bool log=false){
	TCanvas *c1=new TCanvas();
	char *dir=log?"log":"linear";
	gStyle->SetErrorX(0);
	if(log){
		gPad->SetLogx();
	}
	c1->SetLogy();
	for(int i=0;i<4;i++){
		on_peak[i]->Draw("E1");
		c1->SaveAs(Form("%s/Temp/bapd/%s/on_peak/E_gamma_%s.png",RDKHOME,dir,titles[i]));
		off_peak[i]->Draw("E1");
		c1->SaveAs(Form("%s/Temp/bapd/%s/off_peak/E_gamma_%s.png",RDKHOME,dir,titles[i]));
		bg_corrected[i]->Draw("E1");
		c1->SaveAs(Form("%s/Temp/bapd/%s/bg_corrected/E_gamma_%s.png",RDKHOME,dir,titles[i]));
	}
	c1->SetLogx(0);
	c1->SetLogy(0);
	gStyle->SetOptFit(0111);
	for(int i=0;i<4;i++){
		for(int j=0;j<3;j++){
			on_peak_rise[i][j]->Fit("gaus","WW");
			on_peak_rise[i][j]->Draw("E1");
			c1->SaveAs(Form("%s/Temp/bapd/rise/on_peak/t_rise_%s_%s.png",RDKHOME,titles[i],levels[j]));
			off_peak_rise[i][j]->Fit("gaus","WW");
			off_peak_rise[i][j]->Draw("E1");
			c1->SaveAs(Form("%s/Temp/bapd/rise/off_peak/t_rise_%s_%s.png",RDKHOME,titles[i],levels[j]));
			bg_corrected_rise[i][j]->Fit("gaus","WW");
			bg_corrected_rise[i][j]->Draw("E1");
			c1->SaveAs(Form("%s/Temp/bapd/rise/bg_corrected/t_rise_%s_%s.png",RDKHOME,titles[i],levels[j]));
		}
	}
	c1->Close();
	ofstream out(Form("%s/Temp/bapd/%s/bg_corrected/data_%s.txt",RDKHOME,dir,dir));
	out<<setw(15)<<"Bin (keV)"<<setw(15)<<"RDK count"<<setw(15)<<"Uncertainty"<<endl;
	for(int i=1;i<bg_corrected[3]->GetNbinsX();i++){
		out<<setw(15)<<bg_corrected[3]->GetBinCenter(i);
		out<<setw(15)<<bg_corrected[3]->GetBinContent(i)*bg_corrected[3]->GetBinWidth(i);
		out<<setw(15)<<bg_corrected[3]->GetBinError(i)*bg_corrected[3]->GetBinWidth(i)<<endl;
	}
}