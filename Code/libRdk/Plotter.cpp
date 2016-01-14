//RDKPlotter.cpp

#include "Plotter.h"
#include "CalibrationList.h"
#include "CalibrationData.h"
#include "RawTreeReader.h"
#include "Quantity.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TString.h"
#include "TBits.h"
#include "TThread.h"
#include "TFile.h"
#include "TAxis.h"
#include "TLine.h"
#include <iostream>
#include <vector>
using namespace std;

Plotter::Plotter(){
	file=NULL;
	reader=NULL;
	onPeak=new TH1*[16];
	offPeak=new TH1*[16];
	prePeak=new TH1*[16];
	postPeak=new TH1*[16];
	bgc=new TH1*[16];
	delta_t=new TH1*[16];
	canvas=NULL;
	ep=0;
	cal=kFALSE;
	for(Int_t i=0;i<14;i++){
		Float_t size=binSize[CalibrationData::mode.TestBitNumber(i)];
		Float_t m=max[CalibrationData::mode.TestBitNumber(i)];
		Int_t board=i/7,det=i%7;
		Int_t nbins=m/size;
		m=nbins*size;
		TString name,title;
		name.Form("onPeak_%i%i",board,det);
		title.Form("On peak detector %i%i;keV",board,det);
		onPeak[i]=new TH1F(name,title,nbins,0,m);
		onPeak[i]->Sumw2();
		name.Form("offPeak_%i%i",board,det);
		title.Form("Off peak detector %i%i;keV",board,det);
		offPeak[i]=new TH1F(name,title,nbins,0,m);
		offPeak[i]->Sumw2();
		name.Form("prePeak_%i%i",board,det);
		title.Form("Pre-peak detector %i%i;keV",board,det);
		prePeak[i]=new TH1F(name,title,nbins,0,m);
		prePeak[i]->Sumw2();
		prePeak[i]->SetLineColor(kRed);
		name.Form("postPeak_%i%i",board,det);
		title.Form("Post-peak detector %i%i;keV",board,det);
		postPeak[i]=new TH1F(name,title,nbins,0,m);
		postPeak[i]->Sumw2();
		postPeak[i]->SetLineColor(kBlue);
		name.Form("bgc_%i%i",board,det);
		title.Form("Background corrected detector %i%i;keV",board,det);
		bgc[i]=new TH1F(name,title,nbins,0,m);
		bgc[i]->Sumw2();
		name.Form("delta_t_%i%i",board,det);
		title.Form("Delta_t detector %i%i;#Delta_{t} (#mus)",board,det);
		delta_t[i]=new TH1F(name,title,NPOINTS*3/4,-NPOINTS*0.01-0.02,NPOINTS*0.02-0.02);
		delta_t[i]->Sumw2();
	}
	onPeak[14]=new TH1F("onPeak_bgo","On peak BGO detectors;keV",max[0]/binSize[0],0,max[0]);
	onPeak[14]->Sumw2();
	offPeak[14]=new TH1F("offPeak_bgo","Off peak BGO detectors;keV",max[0]/binSize[0],0,max[0]);
	offPeak[14]->Sumw2();
	prePeak[14]=new TH1F("prePeak_bgo","Pre-peak BGO detectors;keV",max[0]/binSize[0],0,max[0]);
	prePeak[14]->Sumw2();
	prePeak[14]->SetLineColor(kRed);
	postPeak[14]=new TH1F("postPeak_bgo","Post-peak BGO detectors;keV",max[0]/binSize[0],0,max[0]);
	postPeak[14]->Sumw2();
	postPeak[14]->SetLineColor(kBlue);
	bgc[14]=new TH1F("bgc_bgo","Background corrected BGO detectors;keV",max[0]/binSize[0],0,max[0]);
	bgc[14]->Sumw2();
	delta_t[14]=new TH1F("delta_t_bgo","Delta_t BGO detectors;#Delta_{t} (#mus)",
		NPOINTS*3/4,-NPOINTS/4*0.04-0.02,NPOINTS/2*0.04-0.02);
	delta_t[14]->Sumw2();
	onPeak[15]=new TH1F("onPeak_bapd","On peak bAPD detectors;keV",max[1]/binSize[1],0,max[1]);
	onPeak[15]->Sumw2();
	offPeak[15]=new TH1F("offPeak_bapd","Off peak bAPD detectors;keV",max[1]/binSize[1],0,max[1]);
	offPeak[15]->Sumw2();
	prePeak[15]=new TH1F("prePeak_bapd","Pre-peak bAPD detectors;keV",max[1]/binSize[1],0,max[1]);
	prePeak[15]->Sumw2();
	prePeak[15]->SetLineColor(kRed);
	postPeak[15]=new TH1F("postPeak_bapd","Post-peak bAPD detectors;keV",max[1]/binSize[1],0,max[1]);
	postPeak[15]->Sumw2();
	postPeak[15]->SetLineColor(kBlue);
	bgc[15]=new TH1F("bgc_bapd","Background corrected bAPD detectors;keV",max[1]/binSize[1],0,max[1]);
	bgc[15]->Sumw2();
	delta_t[15]=new TH1F("delta_t_bapd","Delta_t bAPD detectors;#Delta_{t} (#mus)",
		NPOINTS*3/4,-NPOINTS*0.01-0.02,NPOINTS*0.02-0.02);
	delta_t[15]->Sumw2();
}

Plotter::~Plotter(){
	delete[] onPeak;
	delete[] offPeak;
	delete[] prePeak;
	delete[] postPeak;
	delete[] bgc;
	delete[] delta_t;
	if(canvas){
		delete canvas;
	}
	if(file){
		file->Close();
	}
}

void Plotter::fill(RawTreeReader *r,CalibrationList *bgo_list,CalibrationList *bapd_list){
	reader=r;
	Long64_t index=0;
	Double_t preScale[2]={(windows[0][2]-windows[0][1])/(windows[0][1]-windows[0][0]),
		(windows[1][2]-windows[1][1])/(windows[1][1]-windows[1][0])};
	Double_t postScale[2]={(windows[0][2]-windows[0][1])/(windows[0][3]-windows[0][2]),
		(windows[1][2]-windows[1][1])/(windows[1][3]-windows[1][2])};
	while(index<reader->tree->GetEntries()){
		reader->tree->GetEntry(index++);
		rtemp=reader->recon;
		etemp=reader->entry;
		bgo_cal=bgo_list->getCal(etemp.nseries,etemp.nrun);
		bapd_cal=bapd_list->getCal(etemp.nseries,etemp.nrun);
		if(epEvent(rtemp)){
			ep++;
			for(Int_t i=0;i<14;i++){
				if(CalibrationData::mode.TestBitNumber(i)){
					rtemp.E_gamma[i]*=5.9;
					rtemp.E_gamma[i]/=bapd_cal->getPeak(i);
				}else{
					rtemp.E_gamma[i]*=511.;
					rtemp.E_gamma[i]/=bgo_cal->getPeak(i);
				}
				Int_t mode=CalibrationData::mode.TestBitNumber(i),total=14+mode;
				if(gEvent(rtemp,i)){
					Float_t t=0.04*(rtemp.t_gamma[i]-rtemp.t_e);
					delta_t[i]->Fill(t);
					delta_t[total]->Fill(t);
					if(isOnPeak(rtemp,i)){
						onPeak[i]->Fill(rtemp.E_gamma[i],1/onPeak[i]->GetBinWidth(i));
						onPeak[total]->Fill(rtemp.E_gamma[i],1/onPeak[i]->GetBinWidth(i));
					}else if(isPrePeak(rtemp,i)){
						prePeak[i]->Fill(rtemp.E_gamma[i],preScale[mode]/prePeak[i]->GetBinWidth(i));
						offPeak[i]->Fill(rtemp.E_gamma[i],1/offPeak[i]->GetBinWidth(i));
						prePeak[total]->Fill(rtemp.E_gamma[i],preScale[mode]/prePeak[i]->GetBinWidth(i));
						offPeak[total]->Fill(rtemp.E_gamma[i],1/offPeak[i]->GetBinWidth(i));
					}else if(isPostPeak(rtemp,i)){
						postPeak[i]->Fill(rtemp.E_gamma[i],postScale[mode]/postPeak[i]->GetBinWidth(i));
						offPeak[i]->Fill(rtemp.E_gamma[i],1/offPeak[i]->GetBinWidth(i));
						postPeak[total]->Fill(rtemp.E_gamma[i],postScale[mode]/postPeak[i]->GetBinWidth(i));
						offPeak[total]->Fill(rtemp.E_gamma[i],1/offPeak[i]->GetBinWidth(i));
					}
				}
			}
		}
	}
	Double_t bgo_scale=Double_t(windows[0][1]-windows[0][2])/
		Double_t(windows[0][3]-windows[0][0]+windows[0][1]-windows[0][2]);
	Double_t bapd_scale=Double_t(windows[1][1]-windows[1][2])/
		Double_t(windows[1][3]-windows[1][0]+windows[1][1]-windows[1][2]);
	for(Int_t i=0;i<14;i++){
		if(CalibrationData::mode.TestBitNumber(i)){
			bgc[i]->Add(onPeak[i],offPeak[i],1,bapd_scale);
		}else{
			bgc[i]->Add(onPeak[i],offPeak[i],1,bgo_scale);
		}
	}
	bgc[14]->Add(onPeak[14],offPeak[14],1,bgo_scale);
	bgc[15]->Add(onPeak[15],offPeak[15],1,bapd_scale);
}

void Plotter::reset(){
	ep=0;
	for(Int_t i=0;i<16;i++){
		onPeak[i]->Reset("M");
		offPeak[i]->Reset("M");
		bgc[i]->Reset("M");
		delta_t[i]->Reset("M");
	}
	if(canvas){
		canvas->Clear();
	}
}

void Plotter::draw(const Char_t* path){
	if(canvas==NULL){
		canvas=new TCanvas("rdk_plots","rdk_plots",1920,1080);
	}else{
		canvas->Clear();
	}
	TLine line[4];
	line[0].SetLineColor(kRed);
	line[1].SetLineColor(kRed);
	line[2].SetLineColor(kBlue);
	line[3].SetLineColor(kBlue);
	for(Int_t i=0;i<16;i++){
		TString p;
		onPeak[i]->Draw();
		p.Form("%s/onpeak/%s.png",path,onPeak[i]->GetName());
		//cout<<p<<endl;
		canvas->SaveAs(p);
		canvas->Clear();
		offPeak[i]->Draw();
		p.Form("%s/offpeak/%s.png",path,offPeak[i]->GetName());
		//cout<<p<<endl;
		canvas->SaveAs(p);
		canvas->Clear();
		Double_t pre_max=prePeak[i]->GetMaximum();
		Double_t post_max=postPeak[i]->GetMaximum();
		Double_t max=1.1*(pre_max>post_max?pre_max:post_max);
		prePeak[i]->GetYaxis()->SetRangeUser(0,max);
		postPeak[i]->GetYaxis()->SetRangeUser(0,max);
		prePeak[i]->Draw();
		postPeak[i]->Draw("same");
		if(i<14){
			p.Form("%s/offpeak/compare_%i%i.png",path,i/7,i%7);
		}else if(i==14){
			p.Form("%s/offpeak/compare_bgo.png",path);
		}else if(i==15){
			p.Form("%s/offpeak/compare_bapd.png",path);
		}
		canvas->SaveAs(p);
		canvas->Clear();
		max=1.1*bgc[i]->GetMaximum();
		bgc[i]->GetYaxis()->SetRangeUser(0,max);
		bgc[i]->Draw();
		p.Form("%s/bgc/%s.png",path,bgc[i]->GetName());
		//cout<<p<<endl;
		canvas->SaveAs(p);
		canvas->Clear();
		max=1.1*delta_t[i]->GetMaximum();
		delta_t[i]->Draw();
		delta_t[i]->GetYaxis()->SetRangeUser(0,max);
		Int_t mode=CalibrationData::mode.TestBitNumber(i) || i==15;
		for(Int_t j=0;j<4;j++){
			line[j].DrawLine(0.04*windows[mode][j],0,0.04*windows[mode][j],max);
		}
		p.Form("%s/delta_t/%s.png",path,delta_t[i]->GetName());
		//cout<<p<<endl;
		canvas->SaveAs(p);
		canvas->Clear();
	}
	canvas->Close();
	vector<Int_t> bgo,bapd;
	for(Int_t i=0;i<14;i++){
		if(CalibrationData::mode.TestBitNumber(i)){
			bapd.push_back(i);
		}else{
			bgo.push_back(i);
		}
	}
	bgo.push_back(14);
	bapd.push_back(15);
	ofstream out_bgo(Form("%s/bgc/bgc_bgo_data.csv",path));
	ofstream out_bapd(Form("%s/bgc/bgc_bapd_data.csv",path));
	Quantity results[16];
	out_bgo<<"Bin center (keV)";
	out_bapd<<"Bin center (keV)";
	for(Int_t i=0;i<14;i++){
		if(CalibrationData::mode.TestBitNumber(i)){
			out_bapd<<",det"<<i/7<<i%7<<",err"<<i/7<<i%7;
		}else{
			out_bgo<<",det"<<i/7<<i%7<<",err"<<i/7<<i%7;
		}
	}
	out_bapd<<",total,err"<<endl;
	out_bgo<<",total,err"<<endl;
	for(Int_t i=1;i<=bgc[bgo[0]]->GetNbinsX();i++){
		out_bgo<<bgc[bgo[0]]->GetBinCenter(i);
		for(Int_t j=0;j<bgo.size();j++){
			Int_t ch=bgo[j];
			Quantity q(bgc[ch]->GetBinContent(i),bgc[ch]->GetBinError(i));
			q*=bgc[ch]->GetBinWidth(i);
			out_bgo<<","<<q.getValue()<<","<<q.getUncertainty();
			results[ch]+=q;
		}
		out_bgo<<endl;
	}
	for(Int_t i=1;i<=bgc[bapd[0]]->GetNbinsX();i++){
		out_bapd<<bgc[bapd[0]]->GetBinCenter(i);
		for(Int_t j=0;j<bapd.size();j++){
			Int_t ch=bapd[j];
			Quantity q(bgc[ch]->GetBinContent(i),bgc[ch]->GetBinError(i));
			q*=bgc[ch]->GetBinWidth(i);
			out_bapd<<","<<q.getValue()<<","<<q.getUncertainty();
			results[ch]+=q;
		}
		out_bapd<<endl;
	}
	out_bapd<<"total";
	out_bgo<<"total";
	for(Int_t i=0;i<bapd.size();i++){
		out_bapd<<","<<results[bapd[i]].getValue()<<","<<results[bapd[i]].getUncertainty();
	}
	for(Int_t i=0;i<bgo.size();i++){
		out_bgo<<","<<results[bgo[i]].getValue()<<","<<results[bgo[i]].getUncertainty();
	}
	out_bapd<<endl;
	out_bgo<<endl;
	out_bapd<<"ep,"<<ep<<flush;
	out_bgo<<"ep,"<<ep<<flush;
}

void Plotter::save(const Char_t *path){
	if(file && file->IsOpen()){
		file->Close();
	}
	file=TFile::Open(Form("%s/histograms.root",path),"recreate");
	for(Int_t i=0;i<16;i++){
		file->WriteTObject(bgc[i]);
		file->WriteTObject(onPeak[i]);
		file->WriteTObject(offPeak[i]);
		file->WriteTObject(prePeak[i]);
		file->WriteTObject(postPeak[i]);
		file->WriteTObject(delta_t[i]);
	}
	file->Flush();
}

void Plotter::open(const Char_t *path){
	if(file && file->IsOpen()){
		file->Close();
	}
	file=TFile::Open(Form("%s/histograms.root",path));
	for(Int_t i=0;i<16;i++){
		bgc[i]=(TH1*)file->Get(bgc[i]->GetName());
		onPeak[i]=(TH1*)file->Get(onPeak[i]->GetName());
		offPeak[i]=(TH1*)file->Get(offPeak[i]->GetName());
		prePeak[i]=(TH1*)file->Get(prePeak[i]->GetName());
		postPeak[i]=(TH1*)file->Get(postPeak[i]->GetName());
		delta_t[i]=(TH1*)file->Get(delta_t[i]->GetName());
	}
}

TH1 *Plotter::getOnPeak(Int_t i) const{
	return onPeak[i];
}

TH1 *Plotter::getOffPeak(Int_t i) const{
	return offPeak[i];
}

TH1 *Plotter::getBgc(Int_t i)const{
	return bgc[i];
}

TH1 *Plotter::getDelta_t(Int_t i)const{
	return delta_t[i];
}

TH1 *Plotter::getPrePeak(Int_t i)const{
	return prePeak[i];
}

TH1 *Plotter::getPostPeak(Int_t i)const{
	return postPeak[i];
}

Long64_t Plotter::getEp()const{
	return ep;
}

Bool_t Plotter::epEvent(const Recon &recon){
	return recon.t_p-recon.t_e<650;
}

Bool_t Plotter::gEvent(const Recon &recon,Int_t i){
	Int_t j=CalibrationData::mode.TestBitNumber(i);
	return recon.t_gamma[i]>0 && recon.t_gamma[i]<NPOINTS &&
		recon.E_gamma[i]>=min[j] && recon.E_gamma[i]<max[j];
}

Bool_t Plotter::isOnPeak(const Recon &recon,Int_t i){
	Int_t j=CalibrationData::mode.TestBitNumber(i);
	Int_t delta=recon.t_gamma[i]-recon.t_e;
	return delta>windows[j][1] && delta<=windows[j][2];
}

Bool_t Plotter::isPrePeak(const Recon &recon,Int_t i){
	Int_t j=CalibrationData::mode.TestBitNumber(i);
	Int_t delta=recon.t_gamma[i]-recon.t_e;
	return delta>windows[j][0] && delta<=windows[j][1];
}

Bool_t Plotter::isPostPeak(const Recon &recon,Int_t i){
	Int_t j=CalibrationData::mode.TestBitNumber(i);
	Int_t delta=recon.t_gamma[i]-recon.t_e;
	return delta>windows[j][2] && delta<=windows[j][3];
}

Bool_t Plotter::isOffPeak(const Recon &recon,Int_t i){
	return isPrePeak(recon,i) || isPostPeak(recon,i);
}

Plotter *Plotter::getPlotter(){
	if(fPlotter==NULL){
		fPlotter=new Plotter();
	}
	return fPlotter;
}

Plotter *Plotter::fPlotter=NULL;
Float_t Plotter::binSize[2]={5,0.25};
Float_t Plotter::max[2]={900,20};
Float_t Plotter::min[2]={10,0.25};
Float_t Plotter::windows[2][4]={{-250,-50,50,550},{-346,-46,-6,294}};