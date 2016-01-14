//RawTreeReader.cpp

#include "RawTreeReader.h"
#include "TreeFilter.h"
#include "RawFileReader.h"

#include "TChain.h"
#include "TFile.h"
#include "TPRegexp.h"
#include "TSystem.h"
#include "TSystemDirectory.h"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <algorithm>

using namespace std;

RawTreeReader::RawTreeReader(Bool_t erase){
	path=RDKANAL;
	filter=TreeFilter::std;
	tree=NULL;
	ndat=NULL;
	logPath=NULL;
	reset(erase);
}

RawTreeReader::RawTreeReader(TreeFilter *f,const Char_t *path,Bool_t erase){
	this->path=path;
	filter=f;
	tree=NULL;
	ndat=NULL;
	logPath=NULL;
	reset(erase);
}

RawTreeReader::RawTreeReader(Double_t HVT,Double_t spark,Double_t flux,const Char_t *path,
											 Bool_t erase){
	this->path=path;
	filter=new TreeFilter(HVT,spark,flux);
	tree=NULL;
	ndat=NULL;
	logPath=NULL;
	reset(erase);
}

RawTreeReader::~RawTreeReader(){
	clean();
}

void RawTreeReader::openLog(Bool_t erase){
	if(!log.is_open()){
		if(!logPath){
			logPath=new Char_t[strlen(path)+9];
			sprintf(logPath,"%s/log.txt",path);
		}
		ios_base::openmode mode=ios_base::out;
		if(erase){
			mode|=ios_base::trunc;
		}else{
			mode|=ios_base::app;
		}
		log.open(logPath,mode);
	}
}

void RawTreeReader::clean(){
	if(tree){
		delete tree;
		tree=NULL;
	}
	if(ndat){
		delete ndat;
		ndat=NULL;
	}
	if(logPath){
		delete logPath;
		logPath=NULL;
	}
	if(log.is_open()){
		log.close();
	}
}

void RawTreeReader::reset(Bool_t eraseLog){
	if(eraseLog){
		if(log.is_open()){
			log.close();
		}
		openLog(kTRUE);
	}
	clean();
	tree=new TChain("rdk");
	ndat=new TChain("ndat");
	tree->SetBranchAddress("recon",(Double_t*)&recon);
	tree->SetBranchAddress("entry",(Double_t*)&entry);
	ndat->SetBranchAddress("ndata",(Double_t*)&ndata);
	tree->SetCacheSize(10000000);
	tree->AddBranchToCache("*");
}

NData RawTreeReader::openFile(Char_t type,Int_t series,Int_t run){
	NData total;
	Bool_t closeAtEnd=!log.is_open();
	//cout<<"Open log"<<endl;
	openLog();
	//cout<<"Getting type"<<endl;
	//Char_t type=getAnalyzedType(series,run,path);
	if(type){
		//TString buff;
		//buff.Form("%c%ir%i",type,series,run);
		Char_t buff[256];
		sprintf(buff,"%c%ir%i",type,series,run);
		//cout<<"Checking "<<buff<<endl;
		log<<"Checking "<<buff<<endl;
		//TString fpath;
		//fpath.Form("%s/ss%i/%s.root",path,series,buff.Data());
		Char_t fpath[256];
		sprintf(fpath,"%s/ss%i/%s.root",path,series,buff);
		//cout<<"Openning file"<<endl;
		TFile f(fpath);
		//cout<<"Getting ndat tree"<<endl;
		TTree *temp=(TTree*)f.Get("ndat");
		if(temp){
			NData tdata;
			//cout<<"Setting branch address"<<endl;
			temp->SetBranchAddress("ndata",(Double_t*)&tdata);
			//cout<<"Getting entry"<<endl;
			temp->GetEntry(0);
			//cout<<"Deleting tree"<<endl;
			delete temp;
			temp=(TTree*)f.Get("rdk");
			Long64_t size=temp->GetEntries()*RawFileReader::eventSize+5154;
			//cout<<"Deleting tree"<<endl;
			delete temp;
			if(size>=1ll<<31){	//2GB file limit
				log<<"Warning: File size greater than 2GB: ";
				log<<size<<" bytes"<<endl;
			}
			if(size>=1ll<<32){	//4GB file limit
				log<<"Warning: File size greater than 4GB: ";
				log<<size<<" bytes"<<endl;
			}
			if(filter->valid(tdata,log)){
				total=tdata;
				ndat->Add(fpath);
				tree->Add(fpath);
			}
		}else{
			log<<"Unable to find neutron data"<<endl;
		}
		//cout<<"Closing file"<<endl;
		f.Close();
		f.Delete();
	}else{
		log<<"Unable to find file"<<endl;
	}
	if(closeAtEnd){
		//cout<<"Closing log"<<endl;
		log.close();
	}
	//cout<<"Done"<<endl;
	return total;
}

NData RawTreeReader::openSeries(Int_t series,const Char_t *types){
	Bool_t closeAtEnd=!log.is_open();
	openLog();
	NData total;
	vector<Task> list=getList(series,types,path);
	//vector<Int_t> list=getList(series,types,path);
	Int_t size=list.size();
	log<<"Opening series "<<series<<endl;
	for(Int_t i=0;i<size;i++){
		total+=openFile(list[i].type,series,list[i].run);
		//total+=openFile(series,list[i]);
	}
	if(closeAtEnd){
		log.close();
	}
	return total;
}

NData RawTreeReader::openSeries(Int_t n,const Int_t *list,const Char_t *types){
	Bool_t closeAtEnd=!log.is_open();
	openLog();
	NData total;
	for(Int_t i=0;i<n;i++){
		total+=openSeries(list[i],types);
	}
	if(closeAtEnd){
		log.close();
	}
	return total;
}

NData RawTreeReader::openList(const Char_t* file){
	Bool_t closeAtEnd=!log.is_open();
	openLog();
	NData total;
	ifstream input(file);
	TString buffer;
	input>>buffer;
	while(!input.eof()){
		input>>buffer;
		if(buffer!=""){
			TStringToken token(buffer,",");
			token.NextToken();
			Char_t type=token[0];
			token.NextToken();
			Int_t series=token.Atoi();
			token.NextToken();
			Int_t run=token.Atoi();
			total+=openFile(type,series,run);
		}
	}
	if(closeAtEnd){
		log.close();
	}
	return total;
}

//Char_t RawTreeReader::getType(Int_t series,Int_t run,const Char_t *p,const Char_t *suff){
//	FileStat_t stats;
//	const Char_t types[3]={'S','C','D'};
//	TString path;
//	path.Form("%s/ss%i",p,series);
//	if(!gSystem->GetPathInfo(path,stats)){
//		for(Int_t i=0;i<3;i++){
//			TStringToken token(suff,";",kTRUE);
//			Bool_t good=kTRUE;
//			while(token.NextToken()){
//				TString spath;
//				spath.Form("%s/ss%i/%c%ir%i%s",p,series,types[i],series,run,token.Data());
//				good&=!gSystem->GetPathInfo(spath,stats);
//			}
//			if(good){
//				return types[i];
//			}
//		}
//	}
//	return 0;
//}
//
//Char_t RawTreeReader::getRawType(Int_t series,Int_t run,const Char_t *p){
//	return getType(series,run,p,"b1.dat;b2.dat");
//}
//
//Char_t RawTreeReader::getAnalyzedType(Int_t series,Int_t run,const Char_t *p){
//	return getType(series,run,p,".root");
//}

vector<Task> RawTreeReader::getList(Int_t series,const Char_t *types,const Char_t *p){
//vector<Int_t> RawTreeReader::getList(Int_t series,const Char_t *types,const Char_t *p){
	//vector<Int_t> result;
	vector<Task> result;
	FileStat_t stats;
	TString temp;
	temp.Form("[%s]\\d+r\\d+.root",types);
	TPRegexp regex(temp),digit("\\d+");
	Char_t *shortPath=new Char_t[8];
	sprintf(shortPath,"ss%i",series);
	TString path;
	path.Form("%s/ss%i",p,series);
	if(!gSystem->GetPathInfo(path,stats)){
		TSystemDirectory dir(".",path);
		TList *list=dir.GetListOfFiles();
		for(Int_t i=0;i<list->GetEntries();i++){
			TSystemFile *file=(TSystemFile*)list->At(i);
			TString name=file->GetName();
			if(!file->IsDirectory() && name==name(regex)){
				Int_t run=TString(name(digit,name.Index("r"))).Atoi();
				//Char_t type=getAnalyzedType(series,run,p);
				Char_t type=name[0];
				if(type && (series<311 || type!='D')){
					//result.push_back(run);
					result.push_back(Task(shortPath,series,run,type));
				}
			}
		}
		delete list;
		dir.Delete();
	}
	sort(result.begin(),result.end());
	gSystem->ChangeDirectory(WORKDIR);
	return result;
}

vector<Task> RawTreeReader::getRawList(Int_t series,const Char_t *types,const Char_t *p){
	vector<Task> result;
	FileStat_t stats;
	TString temp;
	try{
		temp.Form("[%s]\\d+r\\d+b1.dat",types);
		TPRegexp regex(temp),digit("\\d+");
		Char_t *shortPath=new Char_t[8];
		sprintf(shortPath,"ss%i",series);
		TString path;
		path.Form("%s/ss%i",p,series);
#ifdef WIN32
		path.ReplaceAll("/","\\");
#endif
		if(!gSystem->GetPathInfo(path,stats)){
			TSystemDirectory dir(".",path);
			TList *list=dir.GetListOfFiles();
			for(Int_t i=0;i<list->GetEntries();i++){
				TSystemFile *file=(TSystemFile*)list->At(i);
				TString name=file->GetName();
				if(!file->IsDirectory() && name==name(regex)){
					Int_t run=TString(name(digit,name.Index("r"))).Atoi();
					Char_t type=name[0];
					result.push_back(Task(shortPath,series,run,type));
				}
			}
		}
		sort(result.begin(),result.end());
	}catch(...){
		cout<<"Unknown exception in RawTreeReader::getRawList"<<endl;
	}
	return result;
}

vector<Int_t> RawTreeReader::getSeries(const Char_t *path){
	//cout<<"Looking for files in : "<<path<<endl;
	vector<Int_t> result;
	try{
		FileStat_t stats;
		//cout<<"Create regex"<<endl;
		TPRegexp regex("ss\\d+"),digit("\\d+");
		//cout<<"Get path info"<<endl;
		if(!gSystem->GetPathInfo(path,stats)){
			//cout<<"Init directory"<<endl;
			TSystemDirectory dir(".",path);
			//cout<<"Get list of files"<<endl;
			TList *list=dir.GetListOfFiles();
			//cout<<"Check each file"<<endl;
			for(Int_t i=0;i<list->GetEntries();i++){
				//cout<<"Getting file"<<endl;
				TSystemFile *file=(TSystemFile*)list->At(i);
				//cout<<"Getting name"<<endl;
				TString name=file->GetName();
				//cout<<"Checking if "<<name<<" is a director and satisfies regex"<<endl;
				if(file->IsDirectory() && name==name(regex)){
					//cout<<"Pushing result"<<endl;
					result.push_back(TString(name(digit)).Atoi());
					//cout<<"Done with "<<name<<endl;
				}
			}
		}
		//cout<<"Sorting"<<endl;
		sort(result.begin(),result.end());
		//cout<<"Done"<<endl;
	}catch(...){
		cerr<<"Unknown exception in RawTreeReader::getSeries(const Char_t*)"<<endl;
	}
	return result;
}

const Int_t RawTreeReader::allSeries[nSeries]={248,256,257,258,260,261,262,263,264,265,267
,270,284,285,286,287,288,289,290,291,292,302,304,305,310,311,312,313,314,315,316,317,318
,319,321};
const Int_t RawTreeReader::all1400[n1400]={76,77,78,79,87,89,91,93,94,95,104,105,106,
	119,120,126,127,151,153,156,160,161,162,165,166,175,176,185,187,214,215,223,227,
	228,253,270,289,290,291,292,304,305,311,303,317};