#include "EpFilter.h"

#include <algorithm>

using std::vector;


EpFilter::EpFilter(){
}


EpFilter::~EpFilter(){
}

Bool_t EpFilter::isValid(epEvent &epevent){
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

Int_t EpFilter::getSet(epEvent &epevent){
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

Double_t EpFilter::getSetBound(Int_t set,Double_t E_e){
	Double_t result=-1;
	if(set>=0){
		result=p0[set]+p2[set]*E_e*E_e;
	}
	return result;
}

Double_t EpFilter::getSetBound(epEvent &epevent){
	return getSetBound(getSet(epevent),epevent.E_e);
}

UInt_t EpFilter::getStatus(epEvent &epevent){
	UInt_t result=0;
	Double_t tof=epevent.t_p-epevent.t_e;
	if(epevent.timestamp[0]<=0 || epevent.timestamp[1]<=0){
		result+=INVALID_TIMESTAMP;
	}
	if(tof<=tofmin || tof>=tofmax){
		result+=INVALID_TOF;
	}
	if(epevent.E_e<=emin || epevent.E_e>=emax){
		result+=INVALID_E_ENERGY;
	}
	if(epevent.E_p<=pmin || epevent.E_p>=pmax){
		result+=INVALID_P_ENERGY;
	}
	Int_t set=getSet(epevent);
	if(epevent.chi2_ep>=getSetBound(epevent)){
		result+=INVALID_CHI2;
	}
	return result;
}

Float_t EpFilter::binSize[2]={1,0.25};
Float_t EpFilter::max[2]={800,20};
Float_t EpFilter::min[2]={10,0.25};
Float_t EpFilter::windows[2][4]={{-15,-3,3,21},{-13.84,-1.84,-0.24,11.76}};
Float_t EpFilter::emax=800;
Float_t EpFilter::emin=100;
Float_t EpFilter::pmax=31;
Float_t EpFilter::pmin=10;
Float_t EpFilter::ebin=5;
Float_t EpFilter::pbin=0.25;
Float_t EpFilter::tofmin=2;
Float_t EpFilter::tofmax=25;
Bool_t EpFilter::EPSCALE=kTRUE;
Bool_t EpFilter::MULTIPLICITY=kFALSE;
Bool_t EpFilter::COOPER=kTRUE;
Bool_t EpFilter::REV_COOPER=kFALSE;

UInt_t EpFilter::INVALID_TIMESTAMP=1u;
UInt_t EpFilter::INVALID_TOF=2u;
UInt_t EpFilter::INVALID_E_ENERGY=4u;
UInt_t EpFilter::INVALID_P_ENERGY=8u;
UInt_t EpFilter::INVALID_CHI2=16;

Double_t EpFilter::p0[5]={28715,20873,28551,12195,24979};
Double_t EpFilter::p2[5]={0.0699423,0.0570625,0.0705464,0.0660285,0.0798169};

Int_t s1[11]={227,228,253,257,270,289,290,291,292,304,305};
Int_t s2a[10]={76,77,78,79,87,89,91,93,94,95};
Int_t s2b[2]={215,223};
Int_t s3[20]={104,105,106,119,120,126,127,151,153,155,156,160,161,162,165,166,175,176,185,187};
Int_t s4[4]={311,313,314,317};
vector<Int_t> EpFilter::set1(s1,s1+sizeof(s1)/sizeof(int));
vector<Int_t> EpFilter::set2a(s2a,s2a+sizeof(s2a)/sizeof(int));
vector<Int_t> EpFilter::set2b(s2b,s2b+sizeof(s2b)/sizeof(int));
vector<Int_t> EpFilter::set3(s3,s3+sizeof(s3)/sizeof(int));
vector<Int_t> EpFilter::set4(s4,s4+sizeof(s4)/sizeof(int));