//Plotter2.h

#ifndef PLOTTER2_H
#define PLOTTER2_H

#include "RDKTypes.h"
#include "Quantity.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TObjArray.h"

#include <vector>

class TFile;
class TTree;

class Plotter2{
private:
	TFile *file;
	TObjArray bgoOnPeak,bapd3OnPeak,bapd4OnPeak;
	TObjArray bgoOffPeak,bapd3OffPeak,bapd4OffPeak;
	TObjArray bgoFinal,bapd3Final,bapd4Final;
	TObjArray bgoDeltaT,bapd3DeltaT,bapd4DeltaT;
	TH1F *sbd_E_e,*sbd_E_p,*sbd_tof,*sbd_E_q,*sbd_q_tof;
	TObjArray sbdChi2OfE;
	TH2F *sbdChi2vE;
	Quantity ep_bgo[12],ep_bapd3[3],ep_bapd4[4];
	Quantity epg_ep_bgo[13],epg_ep_bapd3[4],epg_ep_bapd4[5];
	Bool_t epFilter(epEvent&);
public:
	Plotter2();
	~Plotter2();
	void fill(TTree*);
	void save(const Char_t*);
	void open(const Char_t*);
	void draw(const Char_t*,Int_t=-1);
	void print(const Char_t*,const Char_t*);
	TH1F* getBgoOnPeak(Int_t i){return (TH1F*)bgoOnPeak[i];}
	TH1F* getBgoOffPeak(Int_t i){return (TH1F*)bgoOffPeak[i];}
	TH1F* getBgoFinal(Int_t i){return (TH1F*)bgoFinal[i];}
	TH1F* getBgoDeltaT(Int_t i){return (TH1F*)bgoDeltaT[i];}
	TH1F* getBapd3OnPeak(Int_t i){return (TH1F*)bapd3OnPeak[i];}
	TH1F* getBapd3OffPeak(Int_t i){return (TH1F*)bapd3OffPeak[i];}
	TH1F* getBapd3Final(Int_t i){return (TH1F*)bapd3Final[i];}
	TH1F* getBapd3DeltaT(Int_t i){return (TH1F*)bapd3DeltaT[i];}
	TH1F* getBapd4OnPeak(Int_t i){return (TH1F*)bapd4OnPeak[i];}
	TH1F* getBapd4OffPeak(Int_t i){return (TH1F*)bapd4OffPeak[i];}
	TH1F* getBapd4Final(Int_t i){return (TH1F*)bapd4Final[i];}
	TH1F* getBapd4DeltaT(Int_t i){return (TH1F*)bapd4DeltaT[i];}
	TH1F* getSbdEe(){return sbd_E_e;}
	TH1F* getSbdEp(){return sbd_E_p;}
	TH1F* getSbdTof(){return sbd_tof;}
	TH1F* getSbdEq(){return sbd_E_q;}
	TH1F* getSbdQTof(){return sbd_q_tof;}
	TH2F* getSbdChi2vE(){return sbdChi2vE;}
	TH1F* getSbdChi2OfE(Int_t i){return (TH1F*)sbdChi2OfE[i];}
	Quantity getEpBgo(Int_t i){return ep_bgo[i];}
	Quantity getEpBapd3(Int_t i){return ep_bapd3[i];}
	Quantity getEpBapd4(Int_t i){return ep_bapd4[i];}
	Quantity getEpgEpBgo(Int_t i){return epg_ep_bgo[i];}
	Quantity getEpgEpBapd3(Int_t i){return epg_ep_bapd3[i];}
	Quantity getEpgEpBapd4(Int_t i){return epg_ep_bapd4[i];}
	Int_t getSet(epEvent&);
	Double_t getSetBound(Int_t,Double_t);
	Double_t getSetBound(epEvent&);
	static Float_t binSize[2],max[2],min[2],windows[2][4],emax,emin,pmax,pmin;
	static Float_t ebin,pbin,tofmin,tofmax;
	static Double_t p0[5],p2[5];
	static std::vector<Int_t> set1,set2a,set2b,set3,set4;
	static Bool_t EPSCALE,MULTIPLICITY,COOPER,REV_COOPER;
};

#endif