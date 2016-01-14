//SecondPass.cpp

#include "SecondPass.h"
#include "CalibrationList.h"
#include "CalibrationData.h"
#include "RDKConfig.h"
#include "TreeFilter.h"

#include "TTree.h"
#include "TChain.h"
#include "TFile.h"
#include "TSystem.h"
#include "TString.h"
#include "TSystemDirectory.h"
#include "TPRegexp.h"
#include "TList.h"
#include "TBits.h"
#include "TMath.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using namespace TMath;

Long64_t absDiff(Long64_t a,Long64_t b){
	if(a>b){
		return a-b;
	}else{
		return b-a;
	}
}

SecondPass::SecondPass(const Char_t *rl,Char_t t,Int_t s,UInt_t c){
	runlist=rl;
	type=t;
	series=s;
	config=c;
	epCalPeak=-1;
	TFile file(Form("%s/cal.root",RDKANAL));
	bapdList=(CalibrationList*)file.Get("CalibrationList");
	file.Close();
	bgoList=new CalibrationList(Form("%s/temp_calibration.csv",RDKANAL));
	inTree=new TChain("rdk");
	inTree->SetBranchAddress("entry",(Double_t*)&entry);
	inTree->SetBranchAddress("recon",(Double_t*)&recon);
	TString path;
	path.Form("%s/second_pass/ss%i",RDKANAL,series);
	gSystem->mkdir(path,kTRUE);
	output=TFile::Open(Form("%s/%c%i.root",path.Data(),type,series),"recreate","",0);
	outTree=new TTree(Form("rdk_second_pass-%c%i",type,series),"");
	outTree->Branch("epEvent",(Double_t*)&epevent,epEvent::BRANCH);
	ifstream input(Form("%s/epcal.csv",RDKANAL));
	TString buffer;
	buffer.ReadLine(input);
	while(!input.eof()){
		buffer.ReadLine(input);
		TStringToken token(buffer,",");
		token.NextToken();
		Int_t n=token.Atoi();
		token.NextToken();
		Double_t p=token.Atof();
		if(series==n){
			epCalPeak=p;
		}
	}
}

SecondPass::~SecondPass(){
	close();
	delete inTree;
	delete output;
	delete bapdList;
	delete bgoList;
}

void SecondPass::close(){
	if(output && output->IsOpen()){
		output->WriteTObject(outTree,"rdk_second_pass","overwrite");
		output->Close();
	}
}

void SecondPass::open(Bool_t FILTER){
	fstream infile(runlist);
	TString runs;
	runs.ReadFile(infile);
	infile.close();
	FileStat_t stats;
	TString temp,path;
	temp.Form("[%c]\\d+r\\d+.root",type);
	path.Form("%s/ss%i",RDKANAL,series);
	TPRegexp regex(temp),digit("\\d+");
	Int_t k=0;
	if(!gSystem->GetPathInfo(path,stats)){
		TSystemDirectory dir(".",path);
		TList *list=dir.GetListOfFiles();
		if(DEBUG){
			cout<<list->GetEntries()<<" files found"<<endl;
		}
		for(Int_t i=0;i<list->GetSize();i++){
			TSystemFile *file=(TSystemFile*)list->At(i);
			TString name=file->GetName();
			if(!file->IsDirectory() && name==name(regex)){
				if(DEBUG){
					cout<<"Valid file name: "<<name<<endl;
				}
				TString fileName=Form("%s/ss%i/%s",RDKANAL,series,name.Data());
				TFile temp(fileName);
				TTree *ndatTree=(TTree*)temp.Get("ndat");
				NData ndat;
				ndatTree->SetBranchAddress("ndata",(Double_t*)&ndat);
				ndatTree->GetEntry(0);
				Int_t series=TString(name(digit)).Atoi();
				Int_t run=TString(name(digit,name.Index("r"))).Atoi();
				CalibrationData *data=bgoList->getCal(series,run);
				TString filter;
				filter.Form("%c,%i,%i",type,series,run);
				Bool_t valid=kFALSE;
				if(FILTER){
					for(Int_t j=0;j<14;j++){
						valid|=data->getPeak(j)>=0;
					}
					valid&=TreeFilter::std->valid(ndat);
				}else{
					valid=kTRUE;
				}
				if(runs.Contains(filter) && valid){
					if(DEBUG){
						cout<<"Valid file: "<<name<<endl;
					}
					inTree->Add(fileName);
				}else if(DEBUG){
					cout<<"Invalid file: "<<name<<endl;
				}
			}else if(DEBUG){
				cout<<"Invalid file name: "<<name<<endl;
			}
		}
	}else if(DEBUG){
		cout<<"Path not found: "<<path<<endl;
	}
	input=inTree->GetEntries();
	cout<<"Input events: "<<input<<endl;
}

void SecondPass::fill(Bool_t FILTER){
	try{
		Int_t r;
		Long64_t n,i=0;
		glitch=0;
		open(FILTER);
		CalibrationData *cal1,*cal2;
		while(i<inTree->GetEntries()){
			entries.clear();
			recons.clear();
			inTree->GetEntry(i);
			r=entry.nrun;
			cal1=bgoList->getCal(entry);
			cal2=bapdList->getCal(entry);
			while(r==entry.nrun){
				entries.push_back(entry);
				Double_t ep_scale=-1;
				if(epCalPeak>0){
					ep_scale=756.58/epCalPeak;
				}else if(DEF_CAL){
					ep_scale=756.58/24000;
				}
				recon.E_e*=ep_scale;
				recon.t_e*=0.04;
				recon.E_p*=ep_scale;
				recon.t_p*=0.04;
				recon.E_q*=ep_scale;
				recon.t_q*=0.04;
				recon.ep_chi2*=ep_scale*ep_scale;
				for(Int_t k=0;k<14;k++){
					UInt_t mode=RDKConfig::getMode(config,k);
					Double_t g_scale=-1;
					if(mode==BAPD_MODE){
						if(cal2){
							g_scale=5.9/cal2->getPeak(k);
						}else if(DEF_CAL){
							g_scale=0.0015625; // 1/640
						}
					}else if(mode==BGO_MODE){
						if(cal1){
							g_scale=511./cal1->getPeak(k);
						}else if(DEF_CAL){
							g_scale=0.03125; // 1/32
						}
					}
#ifdef __GNUG__
					if(std::isnan(recon.E_gamma[k])){
#elif defined(_MSC_VER)
					if(_isnan(recon.E_gamma[k])){
#else
					if(recon.E_gamma[k]!=recon.E_gamma[k]){
#endif
						recon.E_gamma[k]=-2;
						recon.t_gamma[k]=-2;
						recon.chi2_gamma[k]=-2;
					}else{
						recon.E_gamma[k]*=g_scale;
						recon.t_gamma[k]*=0.04;
						recon.chi2_gamma[k]*=g_scale*g_scale;
					}
				}
				recons.push_back(recon);
				if(inTree->GetEntry(++i)==0){
					r=-1;
				}
			}
			g[0].clear();
			g[1].clear();
			singleBitECC();
			start=1;
			t0=0;
			t1=0;
			while(!entries.empty()){
				fillBuffer();
				glitchECC();
				sync();
				flushBuffer();
			}
			flushGlitches();
		}
		n=outTree->GetEntries();
		close();
		cout<<"Output events: "<<n<<endl;
		cout<<"Unpaired: "<<100.0*n/input-100<<"%"<<endl;
		cout<<"Glitches: "<<glitch<<endl;
	}catch(...){
		cerr<<"Unknown exception in SecondPass::fill"<<endl;
	}
}

void SecondPass::singleBitECC(){
	try{
		Long64_t threshold=1ll<<29;
		Long64_t t0,t1,t2;
		for(Int_t n=0;n<2;n++){
			t0=150*entries[0].timestamp[n];
			t0&=mask;
			entries[0].timestamp[n]=t0/150.;
			t1=150*entries[1].timestamp[n];
			t1&=mask;
			entries[1].timestamp[n]=t1/150.;
			for(Int_t i=2;i<entries.size();i++){
				t2=150*entries[i].timestamp[n];
				Long64_t t2_=t2&mask;
				if(t2_-t0>0 && t2_-t0<threshold){
					t2=t2_;
					entries[i].timestamp[n]=t2/150.;
				}
				t0=t1;
				t1=t2;
			}
			t0=150*entries[0].timestamp[n];
			for(Int_t i=1;i<entries.size();i++){
				t1=150*entries[i].timestamp[n];
				Long64_t t1_=t1&mask;
				if(t1_-t0>0 && t1_-t0<threshold){
					t1=t1_;
					entries[i].timestamp[n]=t1/150.;
				}
				t0=t1;
			}
		}
	}catch(...){
		cerr<<"Unknown exception in SecondPass::singleBitECC"<<endl;
	}
}

void SecondPass::glitchECC(){
	try{
		for(Int_t n=0;n<2;n++){
			temp.clear();
			while(b[n][0].timestamp>b[n][1].timestamp){
				g[n].push_back(b[n][0]);
				b[n].pop_front();
			}
			while(b[n].size()>2){
				if(b[n][0].timestamp>=b[n][1].timestamp){
					g[n].push_back(b[n][1]);
					b[n].erase(b[n].begin()+1);
				}else{
					temp.push_back(b[n][0]);
					b[n].pop_front();
				}
			}
			temp.push_back(b[n][0]);
			if(b[n][0].timestamp>b[n][1].timestamp){
				g[n].push_back(b[n][1]);
			}else{
				temp.push_back(b[n][1]);
			}
			b[n].clear();
			b[n].insert(b[n].begin(),temp.begin(),temp.end());
		}
	}catch(...){
		cerr<<"Unknown exception in SecondPass::glitchECC"<<endl;
		cerr<<"g[0].size()="<<g[0].size()<<endl;
		cerr<<"g[1].size()="<<g[1].size()<<endl;
	}
}

void SecondPass::sync(){
	try{
		Long64_t t0_,t1_;
		Double_t threshold[2]={1e-4,1e-3};
		for(Int_t i=0;i<b[0].size() && i<b[1].size();i++){
			t0_=b[0][i].timestamp;
			t1_=b[1][i].timestamp;
			if(TMath::Abs(Double_t(t0_-t0)/Double_t(t1_-t1)-1)<threshold[start]){
				t0=t0_;
				t1=t1_;
			}else if(t0_-t0<t1_-t1){
				b[1].insert(b[1].begin()+i,BoardEvent());
			}else{
				b[0].insert(b[0].begin()+i,BoardEvent());
			}
			start=0;
		}
	}catch(...){
		cerr<<"Unknown exception in SecondPass::sync"<<endl;
	}
}

void SecondPass::fillBuffer(){
	b[0].clear();
	b[1].clear();
	for(Int_t j=0;j<100 && !entries.empty() && !recons.empty();j++){
		Entry entry=entries.front();
		Recon recon=recons.front();
		BoardEvent b0(type,entry,recon,0);
		BoardEvent b1(type,entry,recon,1);
		entries.pop_front();
		recons.pop_front();
		b[0].push_back(b0);
		b[1].push_back(b1);
	}
}

void SecondPass::flushBuffer(){
	while(b[0].size()<b[1].size()){
		b[0].push_back(BoardEvent());
	}
	while(b[1].size()<b[0].size()){
		b[1].push_back(BoardEvent());
	}
	for(Int_t j=0;j<b[0].size();j++){
		epevent=epEvent(b[0][j],b[1][j]);
		if(MULTI){
			multiplicity(epevent);
		}
		outTree->SetBranchAddress("epEvent",(Double_t*)&epevent);
		outTree->Fill();
	}
}

void SecondPass::flushGlitches(){
	for(Int_t n=0;n<2;n++){
		glitch+=g[n].size();
		for(Int_t i=0;i<g[n].size();i++){
			if(n==0){
				epevent=epEvent(g[n][i],BoardEvent());
			}else{
				epevent=epEvent(BoardEvent(),g[n][i]);
			}
			outTree->SetBranchAddress("epEvent",(Double_t*)&epevent);
			outTree->Fill();
		}
	}
}

void SecondPass::multiplicity(epEvent &event){
	compare.setEvent(&event);
	deque<Int_t> v;
	Int_t start=event.timestamp[0]>0?0:7;
	Int_t stop=event.timestamp[1]>0?14:7;
	//fill list of BGO events
	for(Int_t i=start;i<stop;i++){
		UInt_t mode=RDKConfig::getMode(config,i);
		if(mode==BGO_MODE && event.t_gamma[i]>0 && event.E_gamma[i]>10){
			v.push_back(i);
		}
		event.M[i]=0;
		event.grp[i]=-1;
	}
	if(v.size()>0){
		//sort list by t_gamma
		sort(v.begin(),v.end(),compare);
		//group
		event.grp[v[0]]=0;
		deque<Int_t> m;
		Int_t tempM=1;
		for(Int_t i=1;i<v.size();i++){
			if(Abs(event.t_gamma[v[i-1]]-event.t_gamma[v[i]])<sigma_gamma){
				event.grp[v[i]]=event.grp[v[i-1]];
				tempM++;
			}else{
				event.grp[v[i]]=event.grp[v[i-1]]+1;
				m.push_back(tempM);
				tempM=1;
			}
		}
		m.push_back(tempM);
		for(Int_t i=0;i<v.size();i++){
			Int_t grp=event.grp[v[i]];
			event.M[v[i]]=m[grp];
		}
	}
}

const Long64_t SecondPass::mask=0x7FFFFFFEFFFFFFFF;
const Int_t SecondPass::sigma_gamma=15;
Bool_t SecondPass::MULTI=kFALSE;
Bool_t SecondPass::DEF_CAL=kFALSE;
Bool_t SecondPass::DEBUG=kFALSE;