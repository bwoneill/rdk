//TreeReader.cpp

#include "TreeReader.h"

#include "TChain.h"
#include "TString.h"
#include "TPRegexp.h"

#include <iostream>
using namespace std;

TreeReader::TreeReader(){
	tree=NULL;
	reset();
}

TreeReader::~TreeReader(){
	clean();
}

void TreeReader::openSeries(TString str){
	try{
		if(tree==NULL){
			reset();
		}
		TPRegexp regex("[SCDscd][0-9]+");
		TString tmp=str(regex).String();
		if(tmp!=str){
			cout<<"Invalid format: must be of form \"S###\", \"C###\", or \"D###\"";
		}else{
			regex=TPRegexp("[0-9]+");
			Int_t n=str(regex).String().Atoi();
			tmp.Form("%s/second_pass/ss%i/%s.root",RDKANAL,n,str.Data());
			tree->AddFile(tmp);
		}
	}catch(...){
		cerr<<"Unknown exception in TreeReader::open Series"<<endl;
	}
}

void TreeReader::reset(){
	clean();
	tree=new TChain("rdk_second_pass");
	tree->SetBranchAddress("epEvent",(Double_t*)&ep);
	tree->SetCacheSize(10000000);
	tree->AddBranchToCache("*");
}

void TreeReader::clean(){
	if(tree!=NULL){
		delete tree;
		tree=NULL;
	}
}