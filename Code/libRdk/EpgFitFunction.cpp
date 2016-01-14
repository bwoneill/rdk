//EpgFitFunction.h

#include "EpgFitFunction.h"
#include "RDK_OMP.h"

#include "TMath.h"

EpgFitFunction::EpgFitFunction(Int_t n):N(n){
	y=new Double_t[N];
	mode=GFIT;
}

EpgFitFunction::~EpgFitFunction(){
	delete[] y;
}

Double_t EpgFitFunction::operator()(const std::vector<Double_t>& par) const{
	Double_t result=0;
	if(mode==EPFIT){
		const Double_t E_e=par[0],t_e=par[1],sigma_e=par[2],E_p=par[3],t_p=par[4],
			sigma_p=par[5],offset=par[6];
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,32) reduction(+:result)
#endif
		for(Int_t i=0;i<N;i++){
			Double_t r=E_e*TMath::Gaus(i,t_e,sigma_e)+E_p*TMath::Gaus(i,t_p,sigma_p)
				+offset-y[i];
			result+=r*r;
		}
	}else if(mode==GFIT){
		const Double_t A=par[0],B=par[1],C=par[2],D=par[3],t=par[4];
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,32) reduction(+:result)
#endif
		for(Int_t i=0;i<N;i++){
			Double_t r=i>t?A+B*TMath::Exp(C*(t-i))*(1-TMath::Exp(D*(t-i)))-y[i]:A-y[i];
			result+=r*r;
		}
	}
	return result;
}

void EpgFitFunction::setMode(Int_t m){
	if(m==EPFIT || m==GFIT){
		mode=m;
	}
}

const Int_t EpgFitFunction::EPFIT=0;
const Int_t EpgFitFunction::GFIT=1;