//Smoother.h

#ifndef SMOOTHER_H
#define SMOOTHER_H

#include "TVectorD.h"
#include "TMatrixTUtils.h"

class Smoother{
private:
	UInt_t radius,size,order;
	Double_t power;
	TVectorD *vectors;
	void clean();
public:
	Smoother(UInt_t=0,Double_t=3,UInt_t=1);
	~Smoother();
	void init(UInt_t,Double_t,UInt_t);
	TVectorD smooth(const TVectorD&)const;
	void smooth(const TVectorD&,TVectorD&)const;
	void smooth(const TMatrixTRow<Double_t>&,TMatrixTRow<Double_t>&,Int_t)const;
	Double_t sigma_v()const;
	Double_t sigma_h()const;
};

#endif