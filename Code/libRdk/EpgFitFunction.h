//EpgFitFunction.h

#ifndef EPGFITFUNCTION_H
#define EPGFITFUNCTION_H

#include "Rtypes.h"

#include "Minuit2/FCNBase.h"

#include <vector>

class EpgFitFunction: public ROOT::Minuit2::FCNBase{
private:
	Double_t *y;
	const Int_t N;
	Int_t mode;
public:
	EpgFitFunction(Int_t);
	~EpgFitFunction();
	Double_t operator()(const std::vector<Double_t>&) const;
	Double_t Up() const{return 1;}
	inline void setPoint(Int_t i,Double_t v){y[i]=v;}
	void setMode(Int_t);
	static const Int_t EPFIT,GFIT;
};

#endif