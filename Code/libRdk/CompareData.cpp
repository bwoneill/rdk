//CompareData.cpp

#include "CompareData.h"

#include "TMath.h"

#include <iostream>
#include <algorithm>
#include <fstream>

using namespace std;

const Char_t *epComp::BRANCH=
	"nseries/I:nrun:nevent:d_delta_t/D:d_E_e:d_E_p:d_t_gamma[14]:d_E_gamma[14]:"
	"p_delta_t:p_E_e:p_E_p:p_t_gamma[14]:p_E_gamma[14]";

epComp::epComp(){
	nseries=-1;
	nrun=-1;
	nevent=-1;
	d_delta_t=-99999;
	d_E_e=-99999;
	d_E_p=-99999;
	p_delta_t=-99999;
	p_E_e=-99999;
	p_E_p=-99999;
	for(Int_t i=0;i<14;i++){
		d_t_gamma[i]=-99999;
		p_t_gamma[i]=-99999;
		d_E_gamma[i]=-99999;
		p_E_gamma[i]=-99999;
	}
}

epComp::epComp(const epComp& s){
	Char_t *dest=(Char_t*)this,*src=(Char_t*)&s;
	for(Int_t i=0;i<sizeof(epComp);i++){
		dest[i]=src[i];
	}
}

CompareData::CompareData(TString f){
	file=new TFile(f,"recreate");
	result=new TTree("rdk_comparison","rdk_comparison");
	result->Branch("epComp",(Double_t*)&comp,epComp::BRANCH);
}


CompareData::~CompareData(){
	file->WriteTObject(result,"rdk_comparison");
	file->Close();
}

void CompareData::compare(TTree *a,TTree *b,TString o1,TString o2){
	a->SetBranchAddress("epEvent",(Double_t*)&event);
	b->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t i=0;i<a->GetEntries();i++){
		a->GetEntry(i);
		if(event.timestamp[0]>0 && event.timestamp[1]>0){
			data1.push_back(epEvent(event));
		}
	}
	for(Int_t i=0;i<b->GetEntries();i++){
		b->GetEntry(i);
		if(event.timestamp[0]>0 && event.timestamp[1]>0){
			data2.push_back(epEvent(event));
		}
	}
	cout<<data1.size()<<endl;
	cout<<data2.size()<<endl;
	sort(data1.begin(),data1.end());
	sort(data2.begin(),data2.end());
	Bool_t log=kFALSE;
	ofstream out1,out2;
	if(o1!="" && o2!=""){
		log=kTRUE;
		out1.open(o1);
		//preamble
		out1<<"Run,event #,# on board 0,timestamp 0,# on board 1,timestamp 1,";
		//ep
		out1<<"t_e,E_e,sigma_e,t_p,E_p,sigma_p,t_q,E_q,chi2_ep,t_half,t_dip";
		//photon
		for(Int_t i=0;i<14;i++){
			Int_t brd=i/7,ch=i%7;
			out1<<",t_gamma "<<brd<<ch<<",E_gamma "<<brd<<ch<<",M "<<brd<<ch<<",Group "<<brd<<ch;
			out1<<",chi2_gamma "<<brd<<ch<<",t_rise "<<brd<<ch;
			out1<<",A "<<brd<<ch<<",B "<<brd<<ch<<",C "<<brd<<ch<<",D "<<brd<<ch;
		}
		//ep flags
		out1<<",ep flags"<<endl;
		out2.open(o2);
		//preamble
		out2<<"Run,event #,# on board 0,timestamp 0,# on board 1,timestamp 1,";
		//ep
		out2<<"t_e,E_e,sigma_e,t_p,E_p,sigma_p,t_q,E_q,chi2_ep,t_half,t_dip";
		//photon
		for(Int_t i=0;i<14;i++){
			Int_t brd=i/7,ch=i%7;
			out2<<",t_gamma "<<brd<<ch<<",E_gamma "<<brd<<ch<<",M "<<brd<<ch<<",Group "<<brd<<ch;
			out2<<",chi2_gamma "<<brd<<ch<<",t_rise "<<brd<<ch;
			out2<<",A "<<brd<<ch<<",B "<<brd<<ch<<",C "<<brd<<ch<<",D "<<brd<<ch;
		}
		//ep flags
		out2<<",ep flags"<<endl;
	}
	while(!data1.empty()){
		if(data1.size()%1000==0){
			cout<<data1.size()<<","<<data2.size()<<","<<result->GetEntries()<<endl;
		}
		Bool_t cont=kTRUE;
		epEvent c=data1.front();
		for(Int_t i=0;i<data2.size() && cont;i++){
			epEvent d=data2[i];
			if(c==d){
				Double_t dt_c=c.t_p-c.t_e,dt_d=d.t_p-d.t_e;
				comp.nseries=c.nseries;
				comp.nrun=c.nrun;
				comp.nevent=c.nevent;
				if(dt_c>0 && dt_d>0){
					comp.d_delta_t=dt_c-dt_d;
					comp.p_delta_t=comp.d_delta_t/dt_c;
				}else{
					comp.d_delta_t=-99999;
					comp.p_delta_t=-99999;
				}
				if(c.E_e>0 && d.E_e>0){
					comp.d_E_e=c.E_e-d.E_e;
					comp.p_E_e=comp.d_E_e/c.E_e;
				}else{
					comp.d_E_e=-99999;
					comp.p_E_e=-99999;
				}
				if(c.E_p>0 && d.E_p>0){
					comp.d_E_p=c.E_p-d.E_p;
					comp.p_E_p=comp.d_E_p/c.E_p;
				}else{
					comp.d_E_p=-99999;
					comp.p_E_p=-99999;
				}
				for(Int_t j=0;j<14;j++){
					if(c.t_gamma[j]>0 && d.t_gamma[j]>0 && c.E_gamma[j]>0 && d.E_gamma[j]>0){
						comp.d_t_gamma[j]=c.t_gamma[j]-d.t_gamma[j];
						comp.p_t_gamma[j]=comp.d_t_gamma[j]/c.t_gamma[j];
						comp.d_E_gamma[j]=c.E_gamma[j]-d.E_gamma[j];
						comp.p_E_gamma[j]=comp.d_E_gamma[j]/c.E_gamma[j];
					}
				}
				cont=kFALSE;
				result->Fill();
				if(log){
					out1<<c<<endl;
					out2<<d<<endl;
				}
				data2.erase(data2.begin()+i);
			}else if(c<d){
				cont=kFALSE;
			}
		}
		data1.pop_front();
	}
	cout<<result->GetEntries()<<endl;
	file->WriteTObject(result,"rdk_comparison");
	if(log){
		out1.close();
		out2.close();
	}
}