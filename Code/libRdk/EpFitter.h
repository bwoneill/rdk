//EpFitter.h

#ifndef EPFITTER_H
#define EPFITTER_H

#include "Rtypes.h"

class TF1;
class TGraph;
class TFitResultPtr;

class EpFitter{
private:
	static TF1 *func;
	static TGraph *graph;
	static EpFitter *fitter;
	EpFitter();
public:
	~EpFitter();
	static EpFitter* getFitter();
	TFitResultPtr fit(const Short_t*,const Double_t*);	//7 parameters
	TF1* getFunc(){return func;}
	TGraph* getGraph(){return graph;}
	friend class EventDrawer;
};

#endif