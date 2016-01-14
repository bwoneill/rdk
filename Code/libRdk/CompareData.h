//CompareData.h

#ifndef COMPAREDATA_H
#define COMPAREDATA_H

#include "RDKTypes.h"
#include "TTree.h"
#include "TFile.h"

#include <list>
#include <deque>

struct epComp{
public:
	epComp();
	epComp(const epComp&);
	Int_t nseries,nrun,nevent;
	Double_t d_delta_t,d_E_e,d_E_p;
	Double_t d_t_gamma[14],d_E_gamma[14];
	Double_t p_delta_t,p_E_e,p_E_p;
	Double_t p_t_gamma[14],p_E_gamma[14];
	static const Char_t *BRANCH;
};

class CompareData{
private:
	TTree *result;
	TFile *file;
	epEvent event;
	epComp comp;
	std::deque<epEvent> data1,data2;
public:
	CompareData(TString);
	~CompareData();
	void compare(TTree*,TTree*,TString="",TString="");
	TTree* getResult(){return result;}
};

#endif