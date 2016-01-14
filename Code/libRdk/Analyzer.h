//Analyzer.h

#ifndef ANALYZER_H
#define ANALYZER_H

#include "RDKTypes.h"
#include "Smoother.h"

#include "TVectorD.h"

class Analyzer{
private:
	TVectorD y,y1,y2,b,m,sigma,chi2;
	Double_t chi2max;
	Smoother *eSmoother,*tSmoother;
	Bool_t status;
	UInt_t config;
	UInt_t mode[14];
public:
	Analyzer(Smoother*,Smoother*,UInt_t=DEFAULT_CONFIG);
	~Analyzer();
	Bool_t preanalyze(const RawData&,Recon&,Int_t,Bool_t=kFALSE); //To be used within parallel for loop
	void analyze(const RawData&,Recon&,Int_t,Bool_t=kFALSE); //Uses parallellized smoother, should
	                                                         //not be used in parallel for loop
	void nullevent(Recon&,Int_t,Bool_t=kFALSE);
	inline void setConfig(UInt_t c){config=c;}
	inline UInt_t getConfig(){return config;}
	static Bool_t GFIT;
};

#endif