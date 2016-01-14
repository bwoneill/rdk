//EpFitFunction.cpp

#include "EpFitFunction.h"
#include "RDK_OMP.h"

#include "TMath.h"

using std::vector;

EpFitFunction::EpFitFunction(Int_t size):N(size){
	y=new Double_t[N];
}

EpFitFunction::~EpFitFunction(){
	delete[] y;
}

Double_t EpFitFunction::operator ()(const vector<Double_t> &par) const{
	Double_t result=0;
	const Double_t A=par[0],B=par[1],C=par[2],D=par[3],E=par[4],F=par[5],G=par[6],alpha=par[7];
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,32) reduction(+:result)
#endif
	for(Int_t i=0;i<N;i++){
		Double_t r=A*TMath::Gaus(i,B,C)*(1+TMath::Erf(alpha*(i-B)/C))+D*TMath::Gaus(i,E,F)+G-y[i];
		result+=r*r;
	}
	return result;
}

vector<Double_t> EpFitFunction::Gradient(const vector<Double_t> &par) const{
	vector<Double_t> result(8);
	Double_t dfdA=0,dfdB=0,dfdC=0,dfdD=0,dfdE=0,dfdF=0,dfdG=0,dfda=0;
	const Double_t isqrtPi=1/TMath::Sqrt(TMath::Pi());
	const Double_t A=par[0],B=par[1],C=par[2],D=par[3],E=par[4],F=par[5],G=par[6],alpha=par[7];
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,32) reduction(+:dfdA,dfdB,dfdC,dfdD,dfdE,dfdF,dfdG,dfda)
#endif
	for(Int_t i=0;i<N;i++){
		Double_t diff_erf=2*TMath::Gaus(i,B,C/TMath::Sqrt(1+2*alpha*alpha))*isqrtPi;
		Double_t drdA=TMath::Gaus(i,B,C)*(1+TMath::Erf(alpha*(i-B)/C));
		Double_t drdD=TMath::Gaus(i,E,F);
		Double_t r=A*drdA+D*drdD+G-y[i];
		dfdA+=r*drdA;
		dfdB+=r*((i-B)/(C*C)*A*drdA-2*alpha*A/C*diff_erf);
		dfdC+=r*(TMath::Power(i-B,2)/TMath::Power(C,3)*A*drdA-2*alpha*A*(i-B)/(C*C)*diff_erf);
		dfdD+=r*drdD;
		dfdE+=r*(1-E)/(F*F)*D*drdD;
		dfdF+=r*TMath::Power(1-E,2)/TMath::Power(F,3)*D*drdD;
		dfdG+=r;
		dfda+=r*A*(i-B)/(C*C)*diff_erf;
	}
	result[0]=2*dfdA;
	result[1]=2*dfdB;
	result[2]=2*dfdC;
	result[3]=2*dfdD;
	result[4]=2*dfdE;
	result[5]=2*dfdF;
	result[6]=2*dfdG;
	result[7]=2*dfda;
	return result;
}

//vector<Double_t> EpFitFunction::Gradient(const vector<Double_t> &par) const{
//	vector<Double_t> result(7);
//	Double_t dfdA=0,dfdB=0,dfdC=0,dfdD=0,dfdE=0,dfdF=0,dfdG=0;
//	const Double_t A=par[0],B=par[1],C=par[2],D=par[3],E=par[4],F=par[5],G=par[6];
//#pragma omp parallel for schedule(dynamic,32) reduction(+:dfdA,dfdB,dfdC,dfdD,dfdE,dfdF,dfdG)
//	for(Int_t i=0;i<N;i++){
//		Double_t drdA=TMath::Gaus(i,B,C);
//		Double_t drdD=TMath::Gaus(i,E,F);
//		Double_t r=A*drdA+D*drdD+G-y[i];
//		dfdA+=r*drdA;
//		dfdB+=r*(i-B)/(C*C)*A*drdA;
//		dfdC+=r*TMath::Power(i-B,2)/TMath::Power(C,3)*A*drdA;
//		dfdD+=r*drdD;
//		dfdE+=r*(1-E)/(F*F)*D*drdD;
//		dfdF+=r*TMath::Power(1-E,2)/TMath::Power(F,3)*D*drdD;
//		dfdG+=r;
//	}
//	result[0]=2*dfdA;
//	result[1]=2*dfdB;
//	result[2]=2*dfdC;
//	result[3]=2*dfdD;
//	result[4]=2*dfdE;
//	result[5]=2*dfdF;
//	result[6]=2*dfdG;
//	return result;
//}

Double_t EpFitFunction::evaluate(Double_t *x, Double_t *par){
	const Double_t A=par[0],B=par[1],C=par[2],D=par[3],E=par[4],F=par[5],G=par[6],alpha=par[7];
	Double_t i= *x;
	return A*TMath::Gaus(i,B,C)*(1+TMath::Erf(alpha*(i-B)/C))+D*TMath::Gaus(i,E,F)+G;
}