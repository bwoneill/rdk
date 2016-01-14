//EpFitFunction.h

#ifndef EPFITFUNCTION_H
#define EPFITFUNCTION_H

#include "Rtypes.h"

#include "Minuit2/FCNGradientBase.h"

#include <vector>

class EpFitFunction: public ROOT::Minuit2::FCNGradientBase{
//class EpFitFunction: public ROOT::Minuit2::FCNBase{
private:
	Double_t *y;
	const Int_t N;
public:
	EpFitFunction(Int_t);
	~EpFitFunction();
	Double_t operator()(const std::vector<Double_t>&) const;
	std::vector<Double_t> Gradient(const std::vector<Double_t>&) const;
	Double_t Up() const{return 1;}
	Bool_t CheckGradient() const {return kFALSE;}
	inline void setPoint(Int_t i,Double_t v){y[i]=v;}
	Double_t evaluate(Double_t *,Double_t *);
};

#endif