//SecondPass.h

#ifndef SECONDPASS_H
#define SECONDPASS_H

#include "RDKTypes.h"

#include <vector>
#include <deque>

class TFile;
class TTree;
class TChain;
class CalibrationList;

class SecondPass{
private:
	class MultiplicityCompare{
	private:
		epEvent *event;
	public:
		void setEvent(epEvent *e){event=e;}
		Bool_t operator()(Int_t i,Int_t j){return event->t_gamma[i]<event->t_gamma[j];}
	} compare;
	Double_t epCalPeak;
	Long64_t input,glitch,t0,t1;
	Char_t type;
	const Char_t *runlist;
	Int_t series,start;
	UInt_t config;
	TFile *output;
	TTree *outTree;
	TChain *inTree;
	Entry entry;
	Recon recon;
	epEvent epevent;
	std::deque<epEvent> events;
	std::deque<Entry> entries;
	std::deque<Recon> recons;
	std::deque<BoardEvent> b[2],g[2],temp;
	CalibrationList *bgoList,*bapdList;
	void singleBitECC();
	void glitchECC();
	void sync();
	void flushBuffer();
	void fillBuffer();
	void flushGlitches();
	void multiplicity(epEvent&);
	void close();
	void open(Bool_t=kTRUE);
public:
	SecondPass(const Char_t *,Char_t,Int_t,UInt_t);
	~SecondPass();
	void fill(Bool_t=kTRUE);
	const static Long64_t mask;
	const static Int_t sigma_gamma;
	static Bool_t MULTI,DEF_CAL,DEBUG;
};

#endif