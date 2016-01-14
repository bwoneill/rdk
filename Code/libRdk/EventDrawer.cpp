//EventDrawer.cpp

#include "EventDrawer.h"
#include "EpFitter.h"
#include "GFitter.h"

#include "TGraph.h"
#include "TF1.h"
#include "TChain.h"
#include "TString.h"

EventDrawer::EventDrawer(){
	reader=NULL;
	tree=NULL;
	if(epFunc==NULL){
		epFunc=EpFitter::func;
	}
	if(gFunc==NULL){
		gFunc=GFitter::func;
	}
}

EventDrawer::~EventDrawer(){
	if(reader){
		delete reader;
	}
	if(tree){
		delete tree;
	}
}

void EventDrawer::openSeries(Char_t type,Int_t series){
	if(tree){
		delete tree;
	}
	tree=new TChain("rdk_second_pass","rdk_second_pass");
	TString path;
	path.Form("%s/second_pass/ss%i/%c%i.root",RDKANAL,series,type,series);
	tree->Add(path);
}

void EventDrawer::drawEvent(Int_t e,Int_t c){
	if(e<0){
		event++;
	}else{
		event=e;
	}
	if(c>=0){
		ch=c;
	}
	// TODO draw event
}

void EventDrawer::draw(const epEvent &e,const RawData &rdata,Int_t c){
	if(c==15){
		ch=15;
		for(Int_t i=0;i<NPOINTS;i++){
			graph->SetPoint(i,i*0.04,rdata.ch[15][i]);
		}
		graph->Draw();
		//set ep func parameters
		epFunc->Draw("same");
		//draw ep markers and lines
	}else if(c>=0 && c<=13){
		ch=c;
		for(Int_t i=0;i<NPOINTS;i++){
			graph->SetPoint(i,i*0.04,rdata.ch[ch][i]);
		}
		gFunc->SetParameters(e.g_param[ch]);
		graph->Draw();
		gFunc->Draw("same");
		//draw photon markers and lines
	}
}

TF1 *EventDrawer::epFunc=NULL;
TF1 *EventDrawer::gFunc=NULL;
TGraph *EventDrawer::graph=new TGraph(2048);