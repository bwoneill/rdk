//GFitFunction.cpp

#include "GFitFunction.h"
#include "RDK_OMP.h"

#include "TMath.h"

GFitFunction::GFitFunction(Int_t n):N(n){
	y=new Double_t[N];
}

GFitFunction::~GFitFunction(){
	delete[] y;
}

Double_t GFitFunction::operator()(const std::vector<Double_t> &par) const{
	Double_t result=0;
	const Double_t A=par[0],B=par[1],C=par[2],D=par[3],t=par[4];
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,32) reduction(+:result)
#endif
	for(Int_t i=0;i<N;i++){
		Double_t r=i>t?A+B*TMath::Exp(C*(t-i))*(1-TMath::Exp(D*(t-i)))-y[i]:A-y[i];
		result+=r*r;
	}
	return result;
}