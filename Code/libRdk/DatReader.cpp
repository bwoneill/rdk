//DatReader.cpp

#include "DatReader.h"
#include "DataReader.h"
#include "Header.h"

#include "TSystem.h"
#include "TThread.h"

void DatReader::open(Int_t series,Int_t run){
	nseries=series;
	nrun=run;
	FileStat_t stats;
	TString path;
	path.Form("%s/ss%i",RDKRAW,nseries);
	if(gSystem->GetPathInfo(path.Data(),stats)){
		TString temp;
		temp.Form("File error: Unable to stat path %s",path.Data());
		throw RDKException(temp);
	}
	for(Int_t i=0;i<2;i++){
		TString temp;
		temp.Form("%s/S%ir%ib%i.dat",path.Data(),nseries,nrun,i+1);
		if(gSystem->GetPathInfo(temp,stats)){
			throw RDKException("File error: Unable to stat file "+temp);
		}
		size[i]=stats.fSize;
		board[i]=new DataReader(temp);
		header[i]=new Header();
	}
	if(size[0]!=size[1]){
		TString temp;
		temp.Form("File error: sizes of %s/S%ir%i*.dat do not match",path.Data(),nseries,nrun);
		throw RDKException(temp);
	}
	isOpen=kTRUE;
	readHeader();
}

void DatReader::initialize(){
	FileStat_t stats;
	TString path;
	path.Form("%s/ss%i",RDKRAW,nseries);
	if(gSystem->GetPathInfo(path.Data(),stats)){
		TString temp;
		temp.Form("File error: Unable to stat path %s",path.Data());
		throw RDKException(temp);
	}
	for(Int_t i=0;i<2;i++){
		TString temp;
		temp.Form("%s/S%ir%ib%i.dat",path.Data(),nseries,nrun,i+1);
		if(gSystem->GetPathInfo(temp,stats)){
			throw RDKException("File error: Unable to stat file "+temp);
		}
		size[i]=stats.fSize;
		board[i]=new DataReader(temp);
		header[i]=new Header();
	}
	if(size[0]!=size[1]){
		TString temp;
		temp.Form("File error: sizes of %s/S%ir%i*.dat do not match",path.Data(),nseries,nrun);
		throw RDKException(temp);
	}
	readHeader();
}

bool DatReader::readNext(Entry &entry,RawData &rdata){
	bool startEntry=true;
	TString line[2];
	if(!board[0]->good() || !board[1]->good()){
		return false;
	}
	if(board[0]->tellg()!=board[1]->tellg()){
		TString temp;
		temp.Form("Sync error: Board out of sync at %X and %X",(Int_t)board[0]->tellg(),(Int_t)board[1]->tellg());
		throw RDKException(temp);
	}
	for(Int_t j=0;j<2;j++){
		if(!startEntry){
			break;
		}
		for(Int_t i=0;i<2;i++){
			line[i]=board[i]->readString(4);
			startEntry&=line[i]=="****";
		}
	}
	if(startEntry){
		readEntry(entry,rdata);
	}else if(line[0]=="----" && line[1]=="----"){
		readFooter();
	}else{
		TString temp;
		temp.Form("Unknown file format: unrecognized string at %X \"",(Int_t)board[0]->tellg());
		throw RDKException(temp+line[0]+"\" or \""+line[1]+"\"");
	}
	return startEntry;
}

void DatReader::readEntry(Entry &entry,RawData &rdata){
	entry.nseries=nseries;
	entry.nrun=nrun;
	entry.nevent=board[0]->readInt();
	Int_t nevent=board[1]->readInt();
	if(entry.nevent!=nevent){
		TString temp;
		temp.Form("Warning: entry events do not match %i != %i",entry.nevent,nevent);
		TThread::Printf(temp);
	}
	entry.timestamp[0]=board[0]->readDouble();
	entry.timestamp[1]=board[1]->readDouble();
	Int_t n[2],hp[2];
	for(Int_t i=0;i<2;i++){
		n[i]=board[i]->readInt();	//number of waves=8
		if(n[i]!=NCHANNELS/2){
			TString temp;
			temp.Form("Unknown file format: %X != %X in board %i at %X",n[i],NCHANNELS/2,i+1,(Int_t)board[i]->tellg()-4);
			throw RDKException(temp);
		}
	}
	for(Int_t i=0;i<2;i++){
		hp[i]=board[i]->readInt();
		if(hp[i]!=0xFF){
			TString temp;
			temp.Form("Unknown file format: %X != FF in board %i at %X",hp[i],i+1,(Int_t)board[i]->tellg()-4);
			throw RDKException(temp);
		}
	}
	for(Int_t i=0;i<NGAMMA;i++){
		if(i<NGAMMA/2){
			board[0]->readData(NPOINTS,rdata.ch[i]);
		}else{
			board[1]->readData(NPOINTS,rdata.ch[i]);
		}
	}
	Int_t mult=(NCHANNELS-NGAMMA)/2;
	for(Int_t i=0;i<mult;i++){
		board[1]->readData(NPOINTS,rdata.ch[NGAMMA+2*i]);
		board[0]->readData(NPOINTS,rdata.ch[NGAMMA+2*i+1]);
	}
}

bool DatReader::datExists(Int_t s,Int_t r){
	FileStat_t stats;
	Long_t size[2];
	TString path;
	path.Form("%s/ss%i",RDKRAW,s);
	if(gSystem->GetPathInfo(path.Data(),stats)){
		return false;
	}
	for(Int_t i=0;i<2;i++){
		TString temp;
		temp.Form("%s/S%ir%ib%i.dat",path.Data(),s,r,i+1);
		if(gSystem->GetPathInfo(temp,stats)){
			return false;
		}
		size[i]=stats.fSize;
	}
	if(size[0]!=size[1]){
		return false;
	}
	return true;
}