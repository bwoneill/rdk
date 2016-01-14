//Plotter.h

#ifndef PLOTTER_H
#define PLOTTER_H

#include "RDKTypes.h"
#include "TThread.h"

class CalibrationList;
class CalibrationData;
class RawTreeReader;
class TH1;
class TCanvas;
class TFile;

class Plotter{
private:
	Plotter();
	~Plotter();
	TH1 **onPeak,**offPeak,**bgc,**delta_t;
	TH1 **prePeak,**postPeak;
	TFile *file;
	TCanvas *canvas;
	Long64_t ep;
	Bool_t cal;
	RawTreeReader *reader;
	Recon rtemp;
	Entry etemp;
	CalibrationData *bgo_cal,*bapd_cal;
	static Plotter *fPlotter;
public:
	void fill(RawTreeReader*,CalibrationList*,CalibrationList*);
	void reset();
	void draw(const Char_t*);
	void save(const Char_t*);
	void open(const Char_t*);
	TH1 *getOnPeak(Int_t)const;
	TH1 *getOffPeak(Int_t)const;
	TH1 *getPrePeak(Int_t)const;
	TH1 *getPostPeak(Int_t)const;
	TH1 *getBgc(Int_t)const;
	TH1 *getDelta_t(Int_t)const;
	Long64_t getEp()const;
	static Bool_t epEvent(const Recon&);
	static Bool_t gEvent(const Recon&,Int_t);
	static Bool_t isPrePeak(const Recon&,Int_t);
	static Bool_t isPostPeak(const Recon&,Int_t);
	static Bool_t isOnPeak(const Recon&,Int_t);
	static Bool_t isOffPeak(const Recon&,Int_t);
	static Plotter *getPlotter();
	static Float_t binSize[2],max[2],min[2],windows[2][4];
};

#endif