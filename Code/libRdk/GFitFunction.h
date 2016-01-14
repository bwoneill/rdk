//GFitFunction.h

#ifndef GFITFUNCTION_H
#define GFITFUNCTION_H

#include "Rtypes.h"

#include "Minuit2/FCNGradientBase.h"

#include <vector>

class GFitFunction: public ROOT::Minuit2::FCNBase{
private:
	Double_t *y;
	const Int_t N;
public:
	GFitFunction(Int_t);
	~GFitFunction();
	Double_t operator()(const std::vector<Double_t>&) const;
	Double_t Up() const{return 1;}
	inline void setPoint(Int_t i,Double_t v){y[i]=v;}
};

#endif