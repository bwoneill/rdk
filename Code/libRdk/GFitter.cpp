//GFitter.cpp

#include "GFitter.h"

#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"
using namespace TMath;

Double_t gfitfunc(Double_t *x,Double_t *par){
	Double_t value,A=par[0],B=par[1],C=par[2],D=par[3],t=par[4],F=par[5];
	if(x[0]<=t){
		value=A+x[0]*F;
	}else{
		value=A+B*Exp(-C*(x[0]-t))*(1-Exp(-D*(x[0]-t)))+x[0]*F;
	}
	return value;
}

GFitter::GFitter(){
}

GFitter::~GFitter(){
	//delete func;
	//delete graph;
}

GFitter* GFitter::getFitter(){
	if(!fitter){
		fitter=new GFitter();
	}
	return fitter;
}

TFitResultPtr GFitter::fit(const Short_t *sig,const Double_t *param){
	for(Int_t i=0;i<7;i++){
		func->SetParameter(i,param[i]);
	}
	for(Int_t i=0;i<2048;i++){
		graph->SetPoint(i,i,sig[i]);
	}
	return graph->Fit(func,"WQBNS");
}

void GFitter::FixParamter(Int_t i,Double_t val){
	func->FixParameter(i,val);
}

void GFitter::ReleaseParameter(Int_t i){
	func->ReleaseParameter(i);
}

TF1 *GFitter::func=new TF1("epfunc",gfitfunc,0,2048,6);
TGraph *GFitter::graph=new TGraph(2048);
GFitter *GFitter::fitter=new GFitter();