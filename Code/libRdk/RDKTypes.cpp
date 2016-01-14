//RDKTypes.cpp

#include "RDKTypes.h"

#include "TSystem.h"
#include "TMath.h"

#include <cstring>

const UInt_t DEFAULT_CONFIG=0x65556565;
const Char_t *RDKANAL=gSystem->Getenv("RDKANAL");
const Char_t *RDKRAW=gSystem->Getenv("RDKRAW");
const Char_t *RDKHOME=gSystem->Getenv("RDKHOME");
Char_t *WORKDIR=new Char_t[256];
RDKConfig *rdk2config=new RDKConfig(Form("%s/detector_config.csv",RDKANAL));

RDKException::RDKException(const Char_t *text){
	strcpy(description,text);
}

NData::NData(){
	Long64_t *temp=&this->nn00;
	for(Int_t i=0;i<13;i++){
		*(temp++)=0;
	}
	series=0;
	run=0;
	start=0;
}

NData& NData::operator=(const NData &a){
	Long64_t *t1=&this->nn00;
	const Long64_t *t2=&a.nn00;
	for(Int_t i=0;i<15;i++){
		*(t1++)=*(t2++);
	}
	start=a.start;
	return *this;
}

NData& NData::operator=(const Int_t a[18]){
	Long64_t *temp=&this->nn00;
	for(Int_t i=0;i<18;i++){
		if(i<8 || (i>8 && i<13)){
			*(temp++)=a[i];
		}else if(i==8){
			temp++;
		}
	}
	nn=nn00+nn10+nn20+nn30;
	return *this;
}

NData& NData::operator+=(const NData &a){
	Long64_t *t1=&this->nn00;
	const Long64_t *t2=&a.nn00;
	for(Int_t i=0;i<13;i++){
		*(t1++)+=*(t2++);
	}
	return *this;
}

Task::Task(const Char_t *p,Int_t s,Int_t r,Char_t t,UInt_t c){
	path=p;
	series=s;
	run=r;
	type=t;
	config=c;
}

Bool_t operator<(Task a,Task b){
	if(a.series==b.series){
		if(a.run==b.run){
			return a.type<b.type;
		}else{
			return a.run<b.run;
		}
	}else{
		return a.series<b.series;
	}
}

std::ostream& operator<<(std::ostream& out,const Task& task){
	return out<<task.path<<"/"<<task.type<<task.series<<"r"<<task.run;
}

const Char_t *Recon::BRANCH="t_e/D:E_e:t_p:E_p:t_q:E_q:t_gamma[14]:E_gamma[14]:chi2_gamma[14]:"
	"R2_gamma[14]:fitparam[14][6]:nfit[14]:noise[14]:t_rise[14]:t_half:t_dip:sigma_e:"
	"sigma_p:ep_offset:ep_chi2:ep_edm:ep_errdef:sat[14]/I:tail[14]:peak_pos[14]:per_E[14]:disc[14]/F"
	"flat[14]:adj_E[14]";
const Char_t *Entry::BRANCH="timestamp[2]/D:nevent/I:nrun:nseries";
const Char_t *epEvent::BRANCH="t_e/D:E_e:t_half:t_dip:t_p:E_p:chi2_ep:sigma_e:sigma_p:"
	"t_q:E_q:t_gamma[14]:E_gamma[14]:chi2_gamma[14]:t_rise[14]:g_param[14][6]:M[14]/I:grp[14]:timestamp[2]/L:"
	"nseries/I:nrun:nevent:nevent0:nevent1:type/b";

epEvent::epEvent(){
	timestamp[0]=0;
	timestamp[1]=0;
	type='\0';
	nseries=0;
	nevent=0;
	nevent0=0;
	nevent1=0;
	t_e=-1;
	E_e=-1;
	t_half=-1;
	t_dip=-1;
	t_p=-1;
	E_p=-1;
	t_q=-1;
	E_q=-1;
	chi2_ep=-1;
	sigma_e=-1;
	sigma_p=-1;
	for(Int_t i=0;i<14;i++){
		t_gamma[i]=-1;
		E_gamma[i]=-1;
		chi2_gamma[i]=-1;
		t_rise[i]=-1;
		M[i]=0;
		grp[i]=-1;
		for(Int_t j=0;j<6;j++){
			g_param[i][j]=-1;
		}
	}
}

epEvent::epEvent(const epEvent& e){
	Char_t *clone=(Char_t*)this,*original=(Char_t*)&e;
	for(Int_t i=0;i<sizeof(epEvent);i++){
		clone[i]=original[i];
	}
}

epEvent::epEvent(Char_t c,const Entry &entry,const Recon &recon){
	timestamp[0]=150*entry.timestamp[0];
	timestamp[1]=150*entry.timestamp[1];
	type=c;
	nseries=entry.nseries;
	nevent=entry.nevent;
	nevent0=nevent;
	nevent1=nevent;
	t_e=recon.t_e;
	E_e=recon.E_e;
	t_half=recon.t_half;
	t_dip=recon.t_dip;
	t_p=recon.t_p;
	E_p=recon.E_p;
	t_q=recon.t_q;
	E_q=recon.E_q;
	chi2_ep=recon.ep_chi2;
	sigma_e=recon.sigma_e;
	sigma_p=recon.sigma_p;
	for(Int_t i=0;i<14;i++){
		t_gamma[i]=recon.t_gamma[i];
		E_gamma[i]=recon.E_gamma[i];
		chi2_gamma[i]=recon.chi2_gamma[i];
		t_rise[i]=recon.t_rise[i];
		M[i]=0;
		grp[i]=-1;
		for(Int_t j=0;j<6;j++){
			g_param[i][j]=-1;
		}
	}
}

epEvent::epEvent(const BoardEvent &board0,const BoardEvent &board1,Int_t n){
	timestamp[0]=board0.timestamp;
	timestamp[1]=board1.timestamp;
	if(timestamp[0]>1){
		type=board0.type;
		nseries=board0.nseries;
		nrun=board0.nrun;
	}else if(timestamp[1]>0){
		type=board1.type;
		nseries=board1.nseries;
		nrun=board1.nrun;
	}
	nevent=n;
	nevent0=board0.nevent;
	nevent1=board1.nevent;
	t_e=board0.t_e;
	E_e=board0.E_e;
	t_half=board0.t_half;
	t_dip=board0.t_dip;
	t_p=board0.t_p;
	E_p=board0.E_p;
	t_q=board0.t_q;
	E_q=board0.E_q;
	chi2_ep=board0.chi2_ep;
	sigma_e=board0.sigma_e;
	sigma_p=board1.sigma_p;
	for(Int_t i=0;i<7;i++){
		t_gamma[i]=board0.t_gamma[i];
		t_gamma[i+7]=board1.t_gamma[i];
		E_gamma[i]=board0.E_gamma[i];
		E_gamma[i+7]=board1.E_gamma[i];
		t_rise[i]=board0.t_rise[i];
		t_rise[i+7]=board1.t_rise[i];
		chi2_gamma[i]=board0.chi2_gamma[i];
		chi2_gamma[i+7]=board1.chi2_gamma[i];
		M[i]=0;
		M[i+7]=0;
		grp[i]=-1;
		grp[i+7]=-1;
		for(Int_t j=0;j<5;j++){
			g_param[i][j]=board0.g_param[i][j];
			g_param[i+7][j]=board1.g_param[i][j];
		}
	}
}

epEvent& epEvent::operator=(const epEvent &event){
	timestamp[0]=event.timestamp[0];
	timestamp[1]=event.timestamp[1];
	type=event.type;
	nseries=event.nseries;
	nrun=event.nrun;
	nevent=event.nevent;
	nevent0=event.nevent0;
	nevent1=event.nevent1;
	t_e=event.t_e;
	E_e=event.E_e;
	t_half=event.t_half;
	t_dip=event.t_dip;
	t_p=event.t_p;
	E_p=event.E_p;
	t_q=event.t_q;
	E_q=event.E_q;
	chi2_ep=event.chi2_ep;
	sigma_e=event.sigma_e;
	sigma_p=event.sigma_p;
	for(Int_t i=0;i<14;i++){
		t_gamma[i]=event.t_gamma[i];
		E_gamma[i]=event.E_gamma[i];
		t_rise[i]=event.t_rise[i];
		chi2_gamma[i]=event.chi2_gamma[i];
		M[i]=event.M[i];
		grp[i]=event.grp[i];
		for(Int_t j=0;j<6;j++){
			g_param[i][j]=event.g_param[i][j];
		}
	}
	return *this;
}

Bool_t epEvent::operator<(const epEvent& b) const{
	Bool_t result;
	if(nseries==b.nseries && nrun==b.nrun){
		result=timestamp[1]<b.timestamp[1];
	}else if(nrun!=b.nrun){
		result=nrun<b.nrun;
	}else{
		result=nseries<b.nseries;
	}
	return result;
}

Bool_t epEvent::operator==(const epEvent& b) const{
	return nseries==b.nseries && nrun==b.nrun && TMath::Abs(timestamp[1]-b.timestamp[1])<600;;
}

Bool_t epEvent::operator!=(const epEvent& b) const{
	return !operator==(b);
}

std::ostream& operator<<(std::ostream& out,const epEvent& epevent){
	//preamble
	out<<epevent.type<<epevent.nseries<<"r"<<epevent.nrun<<","<<epevent.nevent<<",";
	out<<epevent.nevent0<<","<<epevent.timestamp[0]<<",";
	out<<epevent.nevent1<<","<<epevent.timestamp[1]<<",";
	//ep
	out<<epevent.t_e<<","<<epevent.E_e<<","<<epevent.sigma_e<<",";
	out<<epevent.t_p<<","<<epevent.E_p<<","<<epevent.sigma_p<<",";
	out<<epevent.t_q<<","<<epevent.E_q<<","<<epevent.chi2_ep<<",";
	out<<epevent.t_half<<","<<epevent.t_dip;
	//photon
	for(Int_t i=0;i<14;i++){
		out<<","<<epevent.t_gamma[i]<<","<<epevent.E_gamma[i]<<","<<epevent.M[i]<<",";
		out<<epevent.grp[i]<<","<<epevent.chi2_gamma[i]<<","<<epevent.t_rise[i];
		//photon fit parameters
		for(Int_t j=0;j<4;j++){
			out<<","<<epevent.g_param[i][j];
		}
	}
	return out;
}

BoardEvent::BoardEvent(){
	timestamp=-1;
	type=0;
	nseries=-1;
	nrun=-1;
	nevent=-1;
	t_e=-1;
	E_e=-1;
	t_half=-1;
	t_dip=-1;
	t_p=-1;
	E_p=-1;
	t_q=-1;
	E_q=-1;
	chi2_ep=-1;
	sigma_e=-1;
	sigma_p=-1;
	for(Int_t i=0;i<7;i++){
		t_gamma[i]=-1;
		E_gamma[i]=-1;
		noise[i]=-1;
		t_rise[i]=-1;
		chi2_gamma[i]=-1;
	}
}

BoardEvent::BoardEvent(Char_t c,const Entry &entry,const Recon &recon,Int_t n){
	timestamp=entry.timestamp[n]*150;
	type=c;
	nseries=entry.nseries;
	nrun=entry.nrun;
	nevent=entry.nevent;
	if(n==0){
		t_e=recon.t_e;
		E_e=recon.E_e;
		t_half=recon.t_half;
		t_dip=recon.t_dip;
		t_p=recon.t_p;
		E_p=recon.E_p;
		t_q=recon.t_q;
		E_q=recon.E_q;
		chi2_ep=recon.ep_chi2;
		sigma_e=recon.sigma_e;
		sigma_p=recon.sigma_p;
	}else{
		t_e=-1;
		E_e=-1;
		t_half=-1;
		t_dip=-1;
		t_p=-1;
		E_p=-1;
		t_q=-1;
		E_q=-1;
		chi2_ep=-1;
		sigma_e=-1;
		sigma_p=-1;
	}
	for(Int_t i=0;i<7;i++){
		Int_t j=7*n+i;
		t_gamma[i]=recon.t_gamma[j];
		E_gamma[i]=recon.E_gamma[j];
		chi2_gamma[i]=recon.chi2_gamma[j];
		noise[i]=recon.noise[j];
		t_rise[i]=recon.t_rise[j];
		for(Int_t k=0;k<6;k++){
			g_param[i][k]=recon.fitparam[j][k];
		}
	}
}

Bool_t operator<(const BoardEvent &a,const BoardEvent &b){
	return a.timestamp<b.timestamp;
}