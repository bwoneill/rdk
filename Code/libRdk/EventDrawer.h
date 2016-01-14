//EventDrawer.h

#ifndef EVENTDRAWER_H
#define EVENTDRAWER_H

#include "RDKTypes.h"

class RawFileReader;

class TGraph;
class TF1;
class TChain;

class EventDrawer{
private:
	static TGraph *graph;
	static TF1 *epFunc,*gFunc;
	RawFileReader *reader;
	TChain *tree;
	Int_t event,ch;
public:
	EventDrawer();
	~EventDrawer();
	void openSeries(Char_t,Int_t);
	void drawEvent(Int_t=-1,Int_t=-1);
	void draw(const epEvent&,const RawData&,int);
	TF1* getEpFunc(){return epFunc;}
	TF1* getGFunc(){return gFunc;}
	TGraph* getGraph(){return graph;}
};

#endif