#ifndef EPFILTER_H
#define EPFILTER_H

#include "RDKTypes.h"

#include <vector>

class EpFilter{
private:
public:
	EpFilter();
	~EpFilter();
	Bool_t isValid(epEvent&);
	UInt_t getStatus(epEvent&);
	Int_t getSet(epEvent&);
	Double_t getSetBound(Int_t,Double_t);
	Double_t getSetBound(epEvent&);
	static Float_t binSize[2],max[2],min[2],windows[2][4],emax,emin,pmax,pmin;
	static Float_t ebin,pbin,tofmin,tofmax;
	static Double_t p0[5],p2[5];
	static std::vector<Int_t> set1,set2a,set2b,set3,set4;
	static Bool_t EPSCALE,MULTIPLICITY,COOPER,REV_COOPER;
	static UInt_t INVALID_TIMESTAMP,INVALID_TOF,INVALID_E_ENERGY,INVALID_P_ENERGY;
	static UInt_t INVALID_CHI2;
};

#endif