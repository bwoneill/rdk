//Quantity.h

#ifndef QUANTITY_H
#define QUANTITY_H

#include "Rtypes.h"

class Quantity{
private:
	Double_t value,uncertainty;
public:
	Quantity(Double_t=0);
	Quantity(Double_t,Double_t);
	~Quantity();
	Double_t getValue()const;
	Double_t getUncertainty()const;
	Quantity inverse()const;
	Quantity negate()const;
	Double_t operator[](Int_t)const;
	Quantity operator-()const;
	Quantity operator+(const Quantity&)const;
	Quantity operator-(const Quantity&)const;
	Quantity operator*(const Quantity&)const;
	Quantity operator/(const Quantity&)const;
	Quantity operator+(Double_t)const;
	Quantity operator-(Double_t)const;
	Quantity operator*(Double_t)const;
	Quantity operator/(Double_t)const;
	Quantity& operator+=(const Quantity&);
	Quantity& operator-=(const Quantity&);
	Quantity& operator*=(const Quantity&);
	Quantity& operator/=(const Quantity&);
	Quantity& operator+=(Double_t);
	Quantity& operator-=(Double_t);
	Quantity& operator*=(Double_t);
	Quantity& operator/=(Double_t);
};

Quantity operator+(Double_t,const Quantity&);
Quantity operator-(Double_t,const Quantity&);
Quantity operator*(Double_t,const Quantity&);
Quantity operator/(Double_t,const Quantity&);

#endif