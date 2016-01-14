//EpFitter.cpp

#include "EpFitter.h"

#include "TGraph.h"
#include "TF1.h"
#include "TMath.h"
using namespace TMath;

Double_t epfitfunc(Double_t *x,Double_t *par){
	return par[0]+par[1]*TMath::Gaus(x[0],par[2],par[3])+par[4]*TMath::Gaus(x[0],par[5],par[6]);
}

EpFitter::EpFitter(){
}

EpFitter::~EpFitter(){
	//delete func;
	//delete graph;
}

EpFitter* EpFitter::getFitter(){
	if(!fitter){
		fitter=new EpFitter();
	}
	return fitter;
}

TFitResultPtr EpFitter::fit(const Short_t *sig,const Double_t *param){
	for(Int_t i=0;i<7;i++){
		func->SetParameter(i,param[i]);
	}
	for(Int_t i=0;i<2048;i++){
		graph->SetPoint(i,i,sig[i]);
	}
	return graph->Fit(func,"WQBNS");
}

TF1 *EpFitter::func=new TF1("epfunc",epfitfunc,0,2048,7);
TGraph *EpFitter::graph=new TGraph(2048);
EpFitter *EpFitter::fitter=new EpFitter();
