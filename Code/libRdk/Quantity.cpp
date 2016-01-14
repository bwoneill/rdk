//Quantity.cpp

#include "Quantity.h"
#include "TMath.h"
using namespace TMath;

Quantity::Quantity(Double_t v){
	value=v;
	uncertainty=Sqrt(value);
}

Quantity::Quantity(Double_t v,Double_t u){
	value=v;
	uncertainty=Abs(u);
}

Quantity::~Quantity(){
}

Double_t Quantity::getValue()const{
	return value;
}

Double_t Quantity::getUncertainty()const{
	return uncertainty;
}

Quantity Quantity::inverse()const{
	return Quantity(1/value,uncertainty/value/value);
}

Quantity Quantity::negate()const{
	return Quantity(-value,uncertainty);
}

Double_t Quantity::operator[](Int_t i) const{
	if(i==0){
		return getValue();
	}else if(i==1){
		return getUncertainty();
	}else{
		return 0;
	}
}

Quantity Quantity::operator-()const{
	return Quantity(-value,uncertainty);
}

Quantity Quantity::operator+(const Quantity &a)const{
	Double_t v=value+a.value,u=Sqrt(uncertainty*uncertainty+a.uncertainty*a.uncertainty);
	return Quantity(v,u);
}

Quantity Quantity::operator-(const Quantity &a) const{
	Double_t v=value-a.value,u=Sqrt(uncertainty*uncertainty+a.uncertainty*a.uncertainty);
	return Quantity(v,u);
}

Quantity Quantity::operator*(const Quantity &a) const{
	Double_t v=value*a.value,u;
	u=v*Sqrt(Power(uncertainty/value,2)+Power(a.uncertainty/a.value,2));
	return Quantity(v,u);
}

Quantity Quantity::operator/(const Quantity &a) const{
	Double_t v=value/a.value,u;
	u=v*Sqrt(Power(uncertainty/value,2)+Power(a.uncertainty/a.value,2));
	return Quantity(v,u);
}

Quantity Quantity::operator+(Double_t a)const{
	return Quantity(value+a,uncertainty);
}

Quantity Quantity::operator-(Double_t a)const{
	return Quantity(value-a,uncertainty);
}

Quantity Quantity::operator*(Double_t a) const{
	return Quantity(value*a,uncertainty*a);
}

Quantity Quantity::operator/(Double_t a) const{
	return Quantity(value/a,uncertainty/a);
}

Quantity& Quantity::operator+=(const Quantity &q){
	value+=q.value;
	uncertainty=Sqrt(Power(uncertainty,2)+Power(q.uncertainty,2));
	return *this;
}

Quantity& Quantity::operator-=(const Quantity &q){
	value-=q.value;
	uncertainty=Sqrt(Power(uncertainty,2)+Power(q.uncertainty,2));
	return *this;
}

Quantity& Quantity::operator*=(const Quantity &q){
	uncertainty=value*q.value*Sqrt(Power(uncertainty/value,2)
		+Power(q.uncertainty/q.value,2));
	value*=q.value;
	return *this;
}

Quantity& Quantity::operator/=(const Quantity &q){
	uncertainty=value/q.value*Sqrt(Power(uncertainty/value,2)
		+Power(q.uncertainty/q.value,2));
	value/=q.value;
	return *this;
}

Quantity& Quantity::operator+=(Double_t a){
	value+=a;
	return *this;
}

Quantity& Quantity::operator-=(Double_t a){
	value-=a;
	return *this;
}

Quantity& Quantity::operator*=(Double_t a){
	value*=a;
	uncertainty*=a;
	return *this;
}

Quantity& Quantity::operator/=(Double_t a){
	value/=a;
	uncertainty/=a;
	return *this;
}

Quantity operator+(Double_t a,const Quantity &b){
	return b+a;
}

Quantity operator-(Double_t a,const Quantity &b){
	return -b+a;
}

Quantity operator*(Double_t a,const Quantity &b){
	return b*a;
}

Quantity operator/(Double_t a,const Quantity &b){
	return b.inverse()*a;
}