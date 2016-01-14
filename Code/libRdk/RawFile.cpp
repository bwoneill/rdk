//RawFile.cpp

#include "RawFile.h"
#include "Analyzer.h"
#include "NdatReader.h"
#include "NdatWriter.h"
#include "DatReader.h"
#include "CalibReader.h"
#include "RDKTools.h"
#include "TreeReader.h"
#include "DatReader.h"
#include "Threads.h"

#include "TTree.h"
#include "TFile.h"
#include "TSystem.h"

#include <climits>

RawFile::RawFile(Int_t nseries,Int_t nrun){
	entry.nseries=nseries;
	entry.nrun=nrun;
	type=TreeReader::getType(entry.nseries,entry.nrun,RDKRAW,"b1.dat;b2.dat");
	if(type=='S'){
		reader=new DatReader(entry.nseries,entry.nrun);
	}else if(type=='C' || type=='D'){
		reader=new CalibReader(entry.nseries,entry.nrun,type);
	}else{
		throw RDKException("Unable to determine file type");
	}
	file=NULL;
}

RawFile::RawFile(const Task &task){
	entry.nseries=task.series;
	entry.nrun=task.run;
	type=TreeReader::getType(entry.nseries,entry.nrun,RDKRAW,"b1.dat;b2.dat");
	if(type=='S'){
		reader=new DatReader(entry.nseries,entry.nrun);
	}else if(type=='C' || type=='D'){
		reader=new CalibReader(entry.nseries,entry.nrun,type);
	}else{
		throw RDKException("Unable to determine file type");
	}
	file=NULL;
}


RawFile::RawFile(AnalyzerMode mode,const Task &task){
	entry.nseries=task.series;
	entry.nrun=task.run;
	type=TreeReader::getType(entry.nseries,entry.nrun,RDKRAW,"b1.dat;b2.dat");
	if(type=='S'){
		reader=new DatReader(entry.nseries,entry.nrun);
	}else if(type=='C' || type=='D'){
		reader=new CalibReader(entry.nseries,entry.nrun,type);
	}else{
		throw RDKException("Unable to determine file type");
	}
	file=NULL;
}

Int_t RawFile::run(){
	try{
		reader->initialize();
		TThread::Lock();
		createTree();
		TThread::UnLock();
		while(reader->readNext(entry,rdata)){
			TThread::Lock();
			rawData->Fill();
			TThread::UnLock();
		}
		header=reader->getHeaders();
		TString temp=header[0]->getValue("Running Parameters.Acquisition start time");
		Double_t start=temp.IsFloat()?temp.Atof():0;
		TThread::Lock();
		NdatWriter writer(type,entry.nseries,entry.nrun,start,file);
		writer.write();
		temp.Form("Saving file ss%i/%c%ir%i.rdat",entry.nseries,type,entry.nseries,entry.nrun);
		TThread::Printf(temp);
		file->WriteTObject(rawData,"rawData","overwrite");
		for(Int_t i=0;i<2;i++){
			temp.Form("header%i",i+1);
			file->WriteTObject(header[i]->getHeader(),temp,"overwrite");
		}
		file->Close();
		TThread::UnLock();
		return 1;
	}catch(RDKException &e){
		TString temp;
		temp.Form("Fatal error reading from %s/ss%i/%c%ir%i*.dat files",RDKRAW,entry.nseries,type,entry.nseries,entry.nrun);
		TThread::Printf(temp);
		TThread::Printf(e.what());
		if(file!=NULL){
			file->Delete("*;*");
			file->Close();
		}
		return 0;
	}
}

void RawFile::clean(){
	if(reader){
		delete reader;
	}
	if(header){
		delete[] header;
	}
	if(file){
		file->Close();
	}
}

void RawFile::createTree(){
	TString path;
	path.Form("%s/ss%i",RDKANAL,entry.nseries);
	FileStat_t stats;
	if(gSystem->GetPathInfo(path,stats)){
		gSystem->mkdir(path,true);
	}
	TString temp;
	temp.Form("%s/%c%ir%i.rdat",path.Data(),type,entry.nseries,entry.nrun);
	file=new TFile(temp,"recreate");
	temp.Form("rawData-%c%ir%i",type,entry.nseries,entry.nrun);
	rawData=new TTree(temp,"RDK raw data");
	Long64_t maxSize=Long64_t(1)<<54-Long64_t(1)<<16;
	if(TTree::GetMaxTreeSize()<maxSize){
		TTree::SetMaxTreeSize(maxSize);
	}
	temp.Form("ch[%i][%i]/S",NCHANNELS,NPOINTS);
	rawData->Branch("rdata",&rdata,temp);
	rawData->Branch("entry",&entry,"time/D:nevent/I:nrun:nseries");
}
