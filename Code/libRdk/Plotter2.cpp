//Plotter2.cpp

#include "Plotter2.h"
#include "RDKConfig.h"
#include "RDKTools.h"

#include "TAxis.h"
#include "TChain.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TList.h"
#include "TLine.h"
#include "TPRegexp.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TF1.h"

#include "TMath.h"

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

const Double_t E_e_chi2_max=800.0;
const Int_t E_e_chi2_bins=800;

TCanvas *c1=NULL;

Plotter2::Plotter2():bgoOnPeak(13),bgoOffPeak(13),bgoFinal(13),bgoDeltaT(13),
bapd3OnPeak(4),bapd3OffPeak(4),bapd3Final(4),bapd3DeltaT(4),
bapd4OnPeak(5),bapd4OffPeak(5),bapd4Final(5),bapd4DeltaT(5),
sbdChi2OfE(E_e_chi2_bins){
	file=NULL;
	TH1F *temp;
	TString name,title;
	Int_t bins=max[0]/binSize[0];
	for(Int_t i=0;i<12;i++){
		Int_t board=i/6,num=i%6;
		name.Form("bgo_onpeak_%i%i",board,num);
		title.Form("BGO On Peak %i%i;keV;epg/ep",board,num);
		temp=new TH1F(name,title,bins,0,max[0]);
		bgoOnPeak[i]=temp;
		temp->Sumw2();
		name.Form("bgo_offpeak_%i%i",board,num);
		title.Form("BGO Off Peak %i%i;keV;epg/ep",board,num);
		temp=new TH1F(name,title,bins,0,max[0]);
		bgoOffPeak[i]=temp;
		temp->Sumw2();
		name.Form("bgo_final_%i%i",board,num);
		title.Form("BGO Final %i%i;keV;epg/ep",board,num);
		temp=new TH1F(name,title,bins,0,max[0]);
		bgoFinal[i]=temp;
		temp->Sumw2();
		name.Form("bgo_delta_t_%i%i",board,num);
		title.Form("BGO Delta_t %i%i;#mus;epg/ep",board,num);
		temp=new TH1F(name,title,(windows[0][3]-windows[0][0])/0.04,
			windows[0][0]-0.02,windows[0][3]-0.02);
		bgoDeltaT[i]=temp;
		temp->Sumw2();
		ep_bgo[i]=Quantity(1e-10,0);
	}
	temp=new TH1F("bgo_onpeak_all","BGO On Peak All;keV;epg/ep",bins,0,max[0]);
	bgoOnPeak[12]=temp;
	temp->Sumw2();
	temp=new TH1F("bgo_offpeak_all","BGO Off Peak All;keV;epg/ep",bins,0,max[0]);
	bgoOffPeak[12]=temp;
	temp->Sumw2();
	temp=new TH1F("bgo_final_all","BGO Final All;keV;epg/ep",bins,0,max[0]);
	bgoFinal[12]=temp;
	temp->Sumw2();
	temp=new TH1F("bgo_delta_t_all","BGO Delta_t All;#mus;epg/ep",
		(windows[0][3]-windows[0][0])/0.04,windows[0][0]-0.02,windows[0][3]-0.02);
	bgoDeltaT[12]=temp;
	temp->Sumw2();
	bins=max[1]/binSize[1];
	for(Int_t i=0;i<3;i++){
		name.Form("bapd3_onpeak_%i",i);
		title.Form("bAPD On Peak %i;keV;epg/ep",i);
		temp=new TH1F(name,title,bins,0,max[1]);
		bapd3OnPeak[i]=temp;
		temp->Sumw2();
		name.Form("bapd3_offpeak_%i",i);
		title.Form("bAPD Off Peak %i;keV;epg/ep",i);
		temp=new TH1F(name,title,bins,0,max[1]);
		bapd3OffPeak[i]=temp;
		temp->Sumw2();
		name.Form("bapd3_final_%i",i);
		title.Form("bAPD Final %i;keV;epg/ep",i);
		temp=new TH1F(name,title,bins,0,max[1]);
		bapd3Final[i]=temp;
		temp->Sumw2();
		name.Form("bapd3_delta_t_%i",i);
		title.Form("bAPD Delta_t %i;#mus;epg/ep",i);
		temp=new TH1F(name,title,(windows[1][3]-windows[1][0])/0.04,windows[1][0]-0.02,
			windows[1][3]-0.02);
		bapd3DeltaT[i]=temp;
		temp->Sumw2();
		ep_bapd3[i]=Quantity(1e-10,0);
	}
	temp=new TH1F("bapd3_onpeak_all","bAPD On Peak All;keV;epg/ep",bins,0,max[1]);
	bapd3OnPeak[3]=temp;
	temp->Sumw2();
	temp=new TH1F("bapd3_offpeak_all","bAPD Off Peak All;keV;epg/ep",bins,0,max[1]);
	bapd3OffPeak[3]=temp;
	temp->Sumw2();
	temp=new TH1F("bapd3_final_all","bAPD Final All;keV;epg/ep",bins,0,max[1]);
	bapd3Final[3]=temp;
	temp->Sumw2();
	temp=new TH1F("bapd3_delta_t_all","bAPD Delta_t All;#mus;epg/ep",
		(windows[1][3]-windows[1][0])/0.04,windows[1][0]-0.02,windows[1][3]-0.02);
	bapd3DeltaT[3]=temp;
	temp->Sumw2();
	for(Int_t i=0;i<4;i++){
		name.Form("bapd4_onpeak_%i",i);
		title.Form("bAPD On Peak %i;keV;epg/ep",i);
		temp=new TH1F(name,title,bins,0,max[1]);
		bapd4OnPeak[i]=temp;
		temp->Sumw2();
		name.Form("bapd4_offpeak_%i",i);
		title.Form("bAPD Off Peak %i;keV;epg/ep",i);
		temp=new TH1F(name,title,bins,0,max[1]);
		bapd4OffPeak[i]=temp;
		name.Form("bapd4_final_%i",i);
		title.Form("bAPD Final %i;keV;epg/ep",i);
		temp=new TH1F(name,title,bins,0,max[1]);
		bapd4Final[i]=temp;
		temp->Sumw2();
		name.Form("bapd4_delta_t_%i",i);
		title.Form("bAPD Delta_t %i;#mus;epg/ep",i);
		temp=new TH1F(name,title,(windows[1][3]-windows[1][0])/0.04,windows[1][0]-0.02,
			windows[1][3]-0.02);
		bapd4DeltaT[i]=temp;
		temp->Sumw2();
		ep_bapd4[i]=Quantity(1e-10,0);
	}
	temp=new TH1F("bapd4_onpeak_all","bAPD On Peak All;keV;epg/ep",bins,0,max[1]);
	bapd4OnPeak[4]=temp;
	temp->Sumw2();
	temp=new TH1F("bapd4_offpeak_all","bAPD Off Peak All;keV;epg/ep",bins,0,max[1]);
	bapd4OffPeak[4]=temp;
	temp->Sumw2();
	temp=new TH1F("bapd4_final_all","bAPD Final All;keV;epg/ep",bins,0,max[1]);
	bapd4Final[4]=temp;
	temp->Sumw2();
	temp=new TH1F("badp4_delta_t_all","bAPD Delta_t All;#mus;epg/ep",
		(windows[1][3]-windows[1][0])/0.04,windows[1][0]-0.02,windows[1][3]-0.02);
	bapd4DeltaT[4]=temp;
	temp->Sumw2();
	sbd_E_e=new TH1F("sbd_E_e","Electron energy;keV;probability",emax/ebin,0,emax);
	sbd_E_e->Sumw2();
	sbd_E_p=new TH1F("sbd_E_p","Proton energy;keV;probability",pmax/pbin,0,pmax);
	sbd_E_p->Sumw2();
	sbd_E_q=new TH1F("sbd_E_q","Quasi-Proton energy;keV;probability",pmax/pbin,0,pmax);
	sbd_E_q->Sumw2();
	sbd_tof=new TH1F("sbd_tof","Electron-proton TOF;#mus;probability",
		tofmax/0.04,-0.02,tofmax-0.02);
	sbd_tof->Sumw2();
	sbd_q_tof=new TH1F("sbd_q_tof","Electron-quasi-proton TOF;#mus;probability",
		tofmax/0.04,-0.02,tofmax-0.02);
	sbd_q_tof->Sumw2();
	for(Int_t i=0;i<sbdChi2OfE.GetSize();i++){
		Float_t binSize=E_e_chi2_max/sbdChi2OfE.GetSize(),min=i*binSize,max=(i+1)*binSize;
		name.Form("sbd_chi2_%i",i);
		title.Form("#chi^{2}_{ep} from %.2f keV to %.2f keV;#chi^{2}_{ep} (keV^{2});probability"
			,min,max);
		temp=new TH1F(name,title,500,0,6e4);
		temp->Sumw2();
		sbdChi2OfE[i]=temp;
	}
	sbdChi2vE=new TH2F("sbd_chi2_v_E","#chi^{2}_{ep} vs E_{e};E_{e} (keV);#chi^{2}_{ep} (keV^{2})",
		500,0,1000,500,0,6e4);
}

Plotter2::~Plotter2(){
}

Int_t bapdIndex(UInt_t mode,Int_t i){
	Int_t value=-1;
	if(mode==BAPD_MODE){
		if(i==6){
			value=0;
		}else if(i==13){
			value=1;
		}else if(i==2){
			value=2;
		}
	}else if(mode==BAPD_PERP){
		if(i==6){
			value=0;
		}else if(i==13){
			value=1;
		}else if(i==5){
			value=2;
		}else if(i==12){
			value=3;
		}
	}
	return value;
}

void Plotter2::fill(TTree *tree){
	TString path;
	path.Form("%s/second_pass",RDKANAL);
	Int_t multi_missmatch=0;
	Double_t avg_missmatch=0;
	Double_t bgo_scale=Double_t(windows[0][1]-windows[0][2])/
		Double_t(windows[0][3]-windows[0][0]+windows[0][1]-windows[0][2]);
	Double_t bapd_scale=Double_t(windows[1][1]-windows[1][2])/
		Double_t(windows[1][3]-windows[1][0]+windows[1][1]-windows[1][2]);
	Double_t binSize=E_e_chi2_max/E_e_chi2_bins;
	//TChain *tree=new TChain("rdk_second_pass");
	//FileStat_t stats;
	//TPRegexp regex("ss\\d+"),number("\\d+");
	//if(!gSystem->GetPathInfo(path,stats)){
	//	TSystemDirectory dir(".",path);
	//	TList *list=dir.GetListOfFiles();
	//	for(Int_t i=0;i<list->GetEntries();i++){
	//		TSystemFile *file=(TSystemFile*)list->At(i);
	//		TString fileName=file->GetName();
	//		if(file->IsDirectory() && fileName==fileName(regex)){
	//			Int_t n=TString(fileName(number)).Atoi();
	//			path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,n,n);
	//			if(n<400 && !gSystem->GetPathInfo(path,stats)){
	//				tree->AddFile(path);
	//			}
	//		}
	//	}
	//}
	epEvent epevent;
	tree->SetBranchAddress("epEvent",(Double_t*)&epevent);
	path.Form("%s/detector_config.csv",RDKANAL);
	RDKConfig config(path);
	//begin filling histograms
	Quantity ep;
	vector<UInt_t> modes(14);
	vector<Double_t> totalE(14),avgT(14),m(14);
	vector<Double_t> tempE(14),tempT(14),tempM(14);
	for(Int_t i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		UInt_t c=config.getConfig(epevent.nseries);
		Double_t tof=epevent.t_p-epevent.t_e;
		if(epFilter(epevent)){
			for(Int_t j=0;j<14;j++){
				modes[j]=RDKConfig::getMode(c,j);
				totalE[j]=0;
				avgT[j]=0;
			}
			sbd_E_e->Fill(epevent.E_e);
			sbd_E_p->Fill(epevent.E_p);
			sbd_tof->Fill(tof);
			Double_t q_tof=epevent.t_q-epevent.t_e;
			if(q_tof>tofmin && q_tof<tofmax){
				sbd_E_q->Fill(epevent.E_q);
				sbd_q_tof->Fill(epevent.t_q-epevent.t_e);
			}
			ep+=Quantity(1);
			//Chi2(E_e) and Chi2 vs E_e
			Int_t E_bin=TMath::Floor(epevent.E_e/binSize);
			getSbdChi2OfE(E_bin)->Fill(epevent.chi2_ep);
			sbdChi2vE->Fill(epevent.E_e,epevent.chi2_ep);
			if(MULTIPLICITY){
				for(Int_t j=0;j<14;j++){
					m[j]=0;
					totalE[j]=0;
					avgT[j]=0;
				}
				for(Int_t j=0;j<14;j++){
					Int_t grp=epevent.grp[j];
					if(modes[j]==BGO_MODE && grp>-1 && grp<14){
						m[grp]++;
						totalE[grp]+=epevent.E_gamma[j];
						avgT[grp]+=epevent.t_gamma[j];
					}
				}
				for(Int_t j=0;j<14;j++){
					Int_t grp=epevent.grp[j];
					if(modes[j]==BGO_MODE && epevent.grp[j]>-1 && epevent.grp[j]<14){
						epevent.E_gamma[j]=totalE[grp];
						epevent.t_gamma[j]=avgT[grp]/m[grp];
						if(m[grp]!=epevent.M[j]){
							multi_missmatch++;
							avg_missmatch+=epevent.M[j]-m[grp];
						}
					}
				}
			}
			for(Int_t j=0;j<14;j++){
				if(modes[j]==BGO_MODE){
					Int_t index=j-j/7;
					ep_bgo[index]+=Quantity(1,1);
					if(epevent.E_gamma[j]>min[0] && epevent.E_gamma[j]<max[0]){
						Double_t delta_t=epevent.t_gamma[j]-epevent.t_e;
						Double_t mScale=epevent.M[j]>0 && MULTIPLICITY?1.0/epevent.M[j]:1;
						getBgoDeltaT(index)->Fill(delta_t,mScale);
						if(delta_t>windows[0][1] && delta_t<=windows[0][2]){	//on peak
							getBgoOnPeak(index)->Fill(epevent.E_gamma[j],mScale);
							epg_ep_bgo[index]+=Quantity(1,1);
						}else if(delta_t>windows[0][0] && delta_t<=windows[0][3]){ //off peak
							getBgoOffPeak(index)->Fill(epevent.E_gamma[j],mScale);
							epg_ep_bgo[index]-=Quantity(bgo_scale,bgo_scale);
						}
					}
				}else if(modes[j]==BAPD_MODE){
					Int_t index=bapdIndex(modes[j],j);
					ep_bapd3[index]+=Quantity(1,1);
					if(epevent.E_gamma[j]>min[1] && epevent.E_gamma[j]<max[1]){
						Double_t delta_t=epevent.t_gamma[j]-epevent.t_e;
						getBapd3DeltaT(index)->Fill(delta_t);
						if(delta_t>windows[1][1] && delta_t<=windows[1][2]){
							getBapd3OnPeak(index)->Fill(epevent.E_gamma[j]);
							epg_ep_bapd3[index]+=Quantity(1,1);
						}else if(delta_t>windows[1][0] && delta_t<=windows[1][3]){
							getBapd3OffPeak(index)->Fill(epevent.E_gamma[j]);
							epg_ep_bapd3[index]-=Quantity(bapd_scale,bapd_scale);
						}
					}
				}else if(modes[j]==BAPD_PERP){
					//Int_t index=bapdIndex(modes[j],j);
					//ep_bapd4[index]+=Quantity(1,1);
					//if(epevent.E_gamma[j]>min[1] && epevent.E_gamma[j]<max[1]){
					//	Double_t delta_t=epevent.t_gamma[j]-epevent.t_e;
					//	getBapd4DeltaT(index)->Fill(delta_t);
					//	if(delta_t>windows[1][1] && delta_t<=windows[1][2]){
					//		getBapd4OnPeak(index)->Fill(epevent.E_gamma[j]);
					//		epg_ep_bapd4[index]+=Quantity(1,1);
					//	}else if(delta_t>windows[1][0] && delta_t<=windows[1][3]){
					//		getBapd4OffPeak(index)->Fill(epevent.E_gamma[j]);
					//		epg_ep_bapd4[index]-=Quantity(bapd_scale,bapd_scale);
					//	}
					//}
				}
			}
		}
	}
	if(multi_missmatch>0){
		avg_missmatch/=multi_missmatch;
		cout<<"Multiplicity missmatches: "<<multi_missmatch<<endl;
		cout<<"Average missmatch: "<<avg_missmatch<<endl;
	}
	Quantity ep_total=0;
	for(Int_t i=0;i<12;i++){
		ep_total+=ep_bgo[i];
		if(ep_bgo[i].getValue()>0){
			Double_t scale=EPSCALE?1/ep_bgo[i].getValue():1;
			getBgoOnPeak(i)->Scale(scale);
			getBgoOffPeak(i)->Scale(scale);
			getBgoDeltaT(i)->Scale(scale);
			getBgoFinal(i)->Add(getBgoOnPeak(i),getBgoOffPeak(i),1,bgo_scale);
		}
	}
	for(Int_t i=0;i<12;i++){
		getBgoOnPeak(12)->Add(getBgoOnPeak(i),(ep_bgo[i]/ep_total).getValue());
		getBgoOffPeak(12)->Add(getBgoOffPeak(i),(ep_bgo[i]/ep_total).getValue());
		getBgoDeltaT(12)->Add(getBgoDeltaT(i),(ep_bgo[i]/ep_total).getValue());
		epg_ep_bgo[12]+=epg_ep_bgo[i];
		epg_ep_bgo[i]/=ep_bgo[i];
	}
	epg_ep_bgo[12]/=ep_total;
	getBgoFinal(12)->Add(getBgoOnPeak(12),getBgoOffPeak(12),1,bgo_scale);
	getBgoFinal(12)->Scale(12);
	ep_total=0;
	for(Int_t i=0;i<3;i++){
		ep_total+=ep_bapd3[i];
		Double_t scale=EPSCALE?1/ep_bapd3[i].getValue():1;
		getBapd3OnPeak(i)->Scale(scale);
		getBapd3OffPeak(i)->Scale(scale);
		getBapd3DeltaT(i)->Scale(scale);
		getBapd3Final(i)->Add(getBapd3OnPeak(i),getBapd3OffPeak(i),1,bapd_scale);
	}
	for(Int_t i=0;i<3;i++){
		Double_t scale=(ep_bapd3[i]/ep_total).getValue();	//Weighted sum
		//Double_t scale=1;	//Non weighted sum
		getBapd3OnPeak(3)->Add(getBapd3OnPeak(i),scale);
		getBapd3OffPeak(3)->Add(getBapd3OffPeak(i),scale);
		getBapd3DeltaT(3)->Add(getBapd3DeltaT(i),scale);
		epg_ep_bapd3[3]+=epg_ep_bapd3[i];
		epg_ep_bapd3[i]/=ep_bapd3[i];
	}
	epg_ep_bapd3[3]/=ep_total;
	getBapd3Final(3)->Add(getBapd3OnPeak(3),getBapd3OffPeak(3),1,bapd_scale);
	getBapd3Final(3)->Scale(3);
	ep_total=0;
	for(Int_t i=0;i<4;i++){
		ep_total+=ep_bapd4[i];
		Double_t scale=EPSCALE?1/ep_bapd4[i].getValue():1;
		getBapd4OnPeak(i)->Scale(scale);
		getBapd4OffPeak(i)->Scale(scale);
		getBapd4Final(i)->Add(getBapd4OnPeak(i),getBapd4OffPeak(i),1,bapd_scale);
	}
	for(Int_t i=0;i<4;i++){
		getBapd4OnPeak(4)->Add(getBapd4OnPeak(i),(ep_bapd4[i]/ep_total).getValue());
		getBapd4OffPeak(4)->Add(getBapd4OffPeak(i),(ep_bapd4[i]/ep_total).getValue());
		getBapd4DeltaT(4)->Add(getBapd4DeltaT(i),(ep_bapd4[i]/ep_total).getValue());
		epg_ep_bapd4[4]+=epg_ep_bapd4[i];
		epg_ep_bapd4[i]/=ep_bapd4[i];
	}
	epg_ep_bapd4[4]/=ep_total;
	getBapd4Final(4)->Add(getBapd4OnPeak(4),getBapd4OffPeak(4),1,bapd_scale);
	getBapd4Final(4)->Scale(4);
	Double_t scale=EPSCALE?1/ep.getValue():1;
	sbd_E_e->Scale(scale);
	sbd_E_p->Scale(scale);
	sbd_tof->Scale(scale);
	sbd_E_q->Scale(scale);
	sbd_q_tof->Scale(scale);
}

void Plotter2::save(const Char_t *path){
	TFile *temp;
	if(file && file->GetName()==TString(path)){
		temp=file;
	}else{
		temp=TFile::Open(path,"recreate");
	}
	for(Int_t i=0;i<4;i++){
		temp->WriteTObject(bapd3OnPeak[i]);
		temp->WriteTObject(bapd3OffPeak[i]);
		temp->WriteTObject(bapd3Final[i]);
		temp->WriteTObject(bapd3DeltaT[i]);
	}
	for(Int_t i=0;i<5;i++){
		temp->WriteTObject(bapd4OnPeak[i]);
		temp->WriteTObject(bapd4OffPeak[i]);
		temp->WriteTObject(bapd4Final[i]);
		temp->WriteTObject(bapd4DeltaT[i]);
	}
	for(Int_t i=0;i<13;i++){
		temp->WriteTObject(bgoOnPeak[i]);
		temp->WriteTObject(bgoOffPeak[i]);
		temp->WriteTObject(bgoFinal[i]);
		temp->WriteTObject(bgoDeltaT[i]);
	}
	temp->WriteTObject(sbd_E_e);
	temp->WriteTObject(sbd_E_p);
	temp->WriteTObject(sbd_tof);
	temp->WriteTObject(sbd_E_q);
	temp->WriteTObject(sbd_q_tof);
	for(Int_t i=0;i<sbdChi2OfE.GetSize();i++){
		temp->WriteTObject(sbdChi2OfE[i]);
	}
	temp->WriteTObject(sbdChi2vE);
	if(temp!=file){
		temp->Close();
	}
}

void Plotter2::open(const Char_t *path){
	if(file!=NULL){
		file->Close();
	}
	file=TFile::Open(path,"update");
	for(Int_t i=0;i<12;i++){
		Int_t board=i/6,num=i%6;
		TString name;
		name.Form("bgo_onpeak_%i%i",board,num);
		bgoOnPeak[i]=file->Get(name);
		name.Form("bgo_offpeak_%i%i",board,num);
		bgoOffPeak[i]=file->Get(name);
		name.Form("bgo_final_%i%i",board,num);
		bgoFinal[i]=file->Get(name);
		name.Form("bgo_delta_t_%i%i",board,num);
		bgoDeltaT[i]=file->Get(name);
	}
	bgoOnPeak[12]=file->Get("bgo_onpeak_all");
	bgoOffPeak[12]=file->Get("bgo_offpeak_all");
	bgoFinal[12]=file->Get("bgo_final_all");
	bgoDeltaT[12]=file->Get("bgo_delta_t_all");
	for(Int_t i=0;i<3;i++){
		TString name;
		name.Form("bapd3_onpeak_%i",i);
		bapd3OnPeak[i]=file->Get(name);
		name.Form("bapd3_offpeak_%i",i);
		bapd3OffPeak[i]=file->Get(name);
		name.Form("bapd3_final_%i",i);
		bapd3Final[i]=file->Get(name);
		name.Form("bapd3_delta_t_%i",i);
		bapd3DeltaT[i]=file->Get(name);
	}
	bapd3OnPeak[3]=file->Get("bapd3_onpeak_all");
	bapd3OffPeak[3]=file->Get("bapd3_offpeak_all");
	bapd3Final[3]=file->Get("bapd3_final_all");
	bapd3DeltaT[3]=file->Get("bapd3_delta_t_all");
	for(Int_t i=0;i<4;i++){
		TString name;
		name.Form("bapd4_onpeak_%i",i);
		bapd4OnPeak[i]=file->Get(name);
		name.Form("bapd4_offpeak_%i",i);
		bapd4OffPeak[i]=file->Get(name);
		name.Form("bapd4_final_%i",i);
		bapd4Final[i]=file->Get(name);
		name.Form("bapd4_delta_t_%i",i);
		bapd4DeltaT[i]=file->Get(name);
	}
	bapd4OnPeak[4]=file->Get("bapd4_onpeak_all");
	bapd4OffPeak[4]=file->Get("bapd4_offpeak_all");
	bapd4Final[4]=file->Get("bapd4_final_all");
	bapd4DeltaT[4]=file->Get("bapd4_delta_t_all");
	sbd_E_e=(TH1F*)file->Get("sbd_E_e");
	sbd_E_p=(TH1F*)file->Get("sbd_E_p");
	sbd_tof=(TH1F*)file->Get("sbd_tof");
	sbd_E_q=(TH1F*)file->Get("sbd_E_q");
	sbd_q_tof=(TH1F*)file->Get("sbd_q_tof");
	for(Int_t i=0;i<sbdChi2OfE.GetSize();i++){
		TString name;
		name.Form("sbd_chi2_%i",i);
		sbdChi2OfE[i]=file->Get(name);
	}
	sbdChi2vE=(TH2F*)file->Get("sbd_chi2_v_E");
}

void Plotter2::draw(const Char_t *path,Int_t set){
	if(!c1){
		c1=new TCanvas("rdkplot","rdkplot",1920,1080);
	}
	c1->Clear();
	TString name;
	TLine lower,upper;
	lower.SetLineColor(kRed);
	upper.SetLineColor(kRed);
	name.Form("%s/onpeak",path);
	gSystem->mkdir(name);
	name.Form("%s/offpeak",path);
	gSystem->mkdir(name);
	name.Form("%s/final",path);
	gSystem->mkdir(name);
	name.Form("%s/delta_t",path);
	gSystem->mkdir(name);
	name.Form("%s/sbd",path);
	gSystem->mkdir(name);
	name.Form("%s/chi2",path);
	gSystem->mkdir(name);
	for(Int_t i=0;i<13;i++){
		getBgoOnPeak(i)->Draw();
		name.Form("%s/onpeak/%s.png",path,getBgoOnPeak(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
		getBgoOffPeak(i)->Draw();
		name.Form("%s/offpeak/%s.png",path,getBgoOffPeak(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
		getBgoFinal(i)->Draw();
		name.Form("%s/final/%s.png",path,getBgoFinal(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
		getBgoDeltaT(i)->Draw();
		lower.DrawLine(windows[0][1],0,windows[0][1],getBgoDeltaT(i)->GetMaximum()*1.1);
		upper.DrawLine(windows[0][2],0,windows[0][2],getBgoDeltaT(i)->GetMaximum()*1.1);
		name.Form("%s/delta_t/%s.png",path,getBgoDeltaT(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
	}
	for(Int_t i=0;i<4;i++){
		getBapd3OnPeak(i)->Draw();
		name.Form("%s/onpeak/%s.png",path,getBapd3OnPeak(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
		getBapd3OffPeak(i)->Draw();
		name.Form("%s/offpeak/%s.png",path,getBapd3OffPeak(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
		getBapd3Final(i)->Draw();
		name.Form("%s/final/%s.png",path,getBapd3Final(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
		getBapd3DeltaT(i)->Draw();
		lower.DrawLine(windows[1][1],0,windows[1][1],getBapd3DeltaT(i)->GetMaximum()*1.1);
		upper.DrawLine(windows[1][2],0,windows[1][2],getBapd3DeltaT(i)->GetMaximum()*1.1);
		name.Form("%s/delta_t/%s.png",path,getBapd3DeltaT(i)->GetName());
		c1->SaveAs(name);
		c1->Clear();
	}
	sbd_E_e->Draw();
	name.Form("%s/sbd/%s.png",path,sbd_E_e->GetName());
	c1->SaveAs(name);
	c1->Clear();
	sbd_E_p->Draw();
	name.Form("%s/sbd/%s.png",path,sbd_E_p->GetName());
	c1->SaveAs(name);
	c1->Clear();
	sbd_tof->Draw();
	name.Form("%s/sbd/%s.png",path,sbd_tof->GetName());
	c1->SaveAs(name);
	c1->Clear();
	sbd_E_q->Draw();
	name.Form("%s/sbd/%s.png",path,sbd_E_q->GetName());
	c1->SaveAs(name);
	c1->Clear();
	sbd_q_tof->Draw();
	name.Form("%s/sbd/%s.png",path,sbd_q_tof->GetName());
	c1->SaveAs(name);
	c1->SetLogy(kTRUE);
	Quantity sum,integral;
	TF1 *gaus=new TF1("gaus","gaus",0,6e4);
	for(Int_t i=0;i<E_e_chi2_bins;i++){
		TH1F *temp=getSbdChi2OfE(i);
		if(temp->GetEntries()>0){
			name.Form("%s/chi2/%s.png",path,temp->GetName());
			//temp->Scale(1.0/temp->GetEntries());
			Double_t energy=E_e_chi2_max*(i+1)/E_e_chi2_bins;
			Double_t bound=(set>=0?getSetBound(set,energy):6e4);
			//cout<<i<<" "<<energy<<" keV "<<bound<<" kev^2"<<endl;
			temp->Fit(gaus,"q","",0,bound);
			sum+=temp->Integral(0,temp->FindBin(bound))*120;
			integral+=gaus->Integral(0,bound);
			temp->Draw();
			c1->SaveAs(name);
			//cout<<i<<" Sum: "<<sum.getValue()<<" Integral: "<<integral.getValue()<<endl;
		}
	}
	Quantity diff=100*(sum-integral)/integral;
	c1->SetLogy(kFALSE);
	c1->SetLogz(kTRUE);
	name.Form("%s/chi2/%s.png",path,sbdChi2vE->GetName());
	sbdChi2vE->Draw("contz");
	c1->SaveAs(name);
	c1->SetLogz(kFALSE);
	c1->Close();
	cout<<"Diff from gaussian fits: "<<diff.getValue()<<"+/-"<<diff.getUncertainty()<<"%"<<endl;
}

void Plotter2::print(const Char_t *path,const Char_t *label){
	TString fileName;
	fileName.Form("%s/bgo_energy_%s.csv",path,label);
	ofstream out(fileName);
	out<<"Bin Center (keV),det00,err00,det01,err01,det02,err02,det03,err03,";
	out<<"det04,err04,det05,err05,det10,err10,det11,err11,det12,err12,";
	out<<"det13,err13,det14,err14,det15,err15,all,err"<<endl;
	Quantity sum[13];
	for(Int_t i=1;i<=getBgoFinal(0)->GetNbinsX();i++){
		out<<getBgoFinal(0)->GetBinCenter(i);
		for(Int_t j=0;j<13;j++){
			Quantity q(getBgoFinal(j)->GetBinContent(i),getBgoFinal(j)->GetBinError(i));
			sum[j]+=q;
			out<<","<<q.getValue()<<","<<q.getUncertainty();
		}
		out<<endl;
	}
	out<<"sum";
	for(Int_t i=0;i<13;i++){
		out<<","<<sum[i].getValue()<<","<<sum[i].getUncertainty();
	}
	out<<endl;
	Bool_t print_ep=kFALSE;
	for(Int_t i=0;i<12;i++){
		print_ep=print_ep || ep_bgo[i].getValue()>1e-10;
	}
	if(print_ep){
		out<<"ep";
		for(Int_t i=0;i<12;i++){
			out<<","<<ep_bgo[i].getValue()<<","<<ep_bgo[i].getUncertainty();
		}
		out<<endl;
	}
	out.close();
	fileName.Form("%s/bapd_energy_%s.csv",path,label);
	out.open(fileName);
	for(Int_t i=0;i<13;i++){
		sum[i]=Quantity(0,0);
	}
	out<<"Bin Center (keV),bapd0,err0,bapd1,err1,bapd2,err2,all,err"<<endl;
	for(Int_t i=1;i<=getBapd3Final(0)->GetNbinsX();i++){
		out<<getBapd3Final(0)->GetBinCenter(i);
		for(Int_t j=0;j<4;j++){
			Quantity q(getBapd3Final(j)->GetBinContent(i),getBapd3Final(j)->GetBinError(i));
			sum[j]+=q;
			out<<","<<q.getValue()<<","<<q.getUncertainty();
		}
		out<<endl;
	}
	out<<"sum";
	for(Int_t i=0;i<4;i++){
		out<<","<<sum[i].getValue()<<","<<sum[i].getUncertainty();
	}
	out<<endl;
	print_ep=kFALSE;
	for(Int_t i=0;i<3;i++){
		print_ep=print_ep || ep_bapd3[i].getValue()!=0;
	}
	if(print_ep){
		out<<"ep";
		for(Int_t i=0;i<3;i++){
			out<<","<<ep_bapd3[i].getValue()<<","<<ep_bapd3[i].getUncertainty();
		}
		out<<endl;
	}
	out.close();
	fileName.Form("%s/ep_tof_%s.csv",path,label);
	out.open(fileName);
	out<<"Bin Center (us),n/ep,err"<<endl;
	for(Int_t i=1;i<=getSbdTof()->GetNbinsX();i++){
		out<<getSbdTof()->GetBinCenter(i)<<","<<getSbdTof()->GetBinContent(i);
		out<<","<<getSbdTof()->GetBinError(i)<<endl;
	}
	out.close();
	fileName.Form("%s/e_energy_%s.csv",path,label);
	out.open(fileName);
	out<<"Bin Center (keV),n/ep,err"<<endl;
	for(Int_t i=1;i<getSbdEe()->GetNbinsX();i++){
		out<<getSbdEe()->GetBinCenter(i)<<","<<getSbdEe()->GetBinContent(i);
		out<<","<<getSbdEe()->GetBinError(i)<<endl;
	}
	out.close();
	fileName.Form("%s/p_energy_%s.csv",path,label);
	out.open(fileName);
	out<<"Bin Center (keV),n/ep,err"<<endl;
	for(Int_t i=1;i<getSbdEp()->GetNbinsX();i++){
		out<<getSbdEp()->GetBinCenter(i)<<","<<getSbdEp()->GetBinContent(i);
		out<<","<<getSbdEp()->GetBinError(i)<<endl;
	}
	out.close();
}

Bool_t Plotter2::epFilter(epEvent &epevent){
	Double_t tof=epevent.t_p-epevent.t_e;
	Bool_t result=epevent.timestamp[0]>0 && epevent.timestamp[1]>0
			&& tof>tofmin && tof<tofmax
			&& epevent.E_e>emin && epevent.E_e<emax
			&& epevent.E_p>pmin && epevent.E_p<pmax;
	if(result && COOPER){
		Int_t set=getSet(epevent);
		if(set>=0 && !REV_COOPER){
			result &= epevent.chi2_ep<getSetBound(epevent);
		}else if(set>=0 && REV_COOPER){
			result &= epevent.chi2_ep>=getSetBound(epevent);
		}else{
			result=kFALSE;
		}
	}
	return result;
}

Int_t Plotter2::getSet(epEvent &epevent){
	Int_t set=-1;
	if(binary_search(set1.begin(),set1.end(),epevent.nseries)){
		set=0;
	}else if(binary_search(set2a.begin(),set2a.end(),epevent.nseries)){
		set=1;
	}else if(binary_search(set2b.begin(),set2b.end(),epevent.nseries)){
		set=2;
	}else if(binary_search(set3.begin(),set3.end(),epevent.nseries)){
		set=3;
	}else if(binary_search(set4.begin(),set4.end(),epevent.nseries)){
		set=4;
	}
	return set;
}

Double_t Plotter2::getSetBound(Int_t set,Double_t E_e){
	Double_t result=-1;
	if(set>=0){
		result=p0[set]+p2[set]*E_e*E_e;
	}
	return result;
}

Double_t Plotter2::getSetBound(epEvent &epevent){
	return getSetBound(getSet(epevent),epevent.E_e);
}

Float_t Plotter2::binSize[2]={1,0.25};
Float_t Plotter2::max[2]={800,20};
Float_t Plotter2::min[2]={10,0.25};
Float_t Plotter2::windows[2][4]={{-15,-3,3,21},{-13.84,-1.84,-0.24,11.76}};
Float_t Plotter2::emax=800;
Float_t Plotter2::emin=100;
Float_t Plotter2::pmax=31;
Float_t Plotter2::pmin=10;
Float_t Plotter2::ebin=5;
Float_t Plotter2::pbin=0.25;
Float_t Plotter2::tofmin=2;
Float_t Plotter2::tofmax=25;
Bool_t Plotter2::EPSCALE=kTRUE;
Bool_t Plotter2::MULTIPLICITY=kFALSE;
Bool_t Plotter2::COOPER=kTRUE;
Bool_t Plotter2::REV_COOPER=kFALSE;
Double_t Plotter2::p0[5]={28715,20873,28551,12195,24979};
Double_t Plotter2::p2[5]={0.0699423,0.0570625,0.0705464,0.0660285,0.0798169};
namespace PLOTTER{
	Int_t s1[11]={227,228,253,257,270,289,290,291,292,304,305};
	Int_t s2a[10]={76,77,78,79,87,89,91,93,94,95};
	Int_t s2b[2]={215,223};
	Int_t s3[20]={104,105,106,119,120,126,127,151,153,155,156,160,161,162,165,166,175,176,185,187};
	Int_t s4[4]={311,313,314,317};
}

using namespace PLOTTER;

vector<Int_t> Plotter2::set1(s1,s1+sizeof(s1)/sizeof(int));
vector<Int_t> Plotter2::set2a(s2a,s2a+sizeof(s2a)/sizeof(int));
vector<Int_t> Plotter2::set2b(s2b,s2b+sizeof(s2b)/sizeof(int));
vector<Int_t> Plotter2::set3(s3,s3+sizeof(s3)/sizeof(int));
vector<Int_t> Plotter2::set4(s4,s4+sizeof(s4)/sizeof(int));