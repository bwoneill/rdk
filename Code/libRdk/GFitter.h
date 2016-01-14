//GFitter.h

#ifndef GFITTER_H
#define GFTTTER_H

#include "Rtypes.h"

class TF1;
class TGraph;
class TFitResultPtr;

class GFitter{
private:
	static TF1 *func;
	static TGraph *graph;
	static GFitter *fitter;
	GFitter();
public:
	~GFitter();
	static GFitter* getFitter();
	TFitResultPtr fit(const Short_t*,const Double_t*);	//6 parameters
	TF1* getFunc(){return func;}
	TGraph* getGraph(){return graph;}
	void FixParamter(Int_t,Double_t);
	void ReleaseParameter(Int_t);
	friend class EventDrawer;
};

#endif