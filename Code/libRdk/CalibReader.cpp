//CalibReader.cpp

#include "CalibReader.h"
#include "DataReader.h"
#include "Header.h"
#include "TreeReader.h"

#include "TSystem.h"
#include "TThread.h"

void CalibReader::open(Int_t series,Int_t run){
	nseries=series;
	nrun=run;
	type=TreeReader::getRawType(nseries,nrun);
	FileStat_t stats;
	TString path;
	path.Form("%s/ss%i",RDKRAW,nseries);
	if(gSystem->GetPathInfo(path,stats)){
		TString temp;
		temp.Form("File error: Unable to stat path %s",path.Data());
		throw RDKException(temp);
	}
	for(Int_t i=0;i<2;i++){
		TString temp;
		temp.Form("%s/%c%ir%ib%i.dat",path.Data(),type,nseries,nrun,i+1);
		if(gSystem->GetPathInfo(temp,stats)){
			throw RDKException("File error: Unable to stat file "+temp);
		}
		size[i]=stats.fSize;
		board[i]=new DataReader(temp);
		header[i]=new Header();
		status[i]=kTRUE;
		//status[i].reset();
	}
	isOpen=kTRUE;
	readHeader();
}

void CalibReader::initialize(){
	FileStat_t stats;
	TString path;
	path.Form("%s/ss%i",RDKRAW,nseries);
	if(gSystem->GetPathInfo(path,stats)){
		TString temp;
		temp.Form("File error: Unable to stat path %s",path.Data());
		throw RDKException(temp);
	}
	for(Int_t i=0;i<2;i++){
		TString temp;
		temp.Form("%s/%c%ir%ib%i.dat",path.Data(),type,nseries,nrun,i+1);
		if(gSystem->GetPathInfo(temp,stats)){
			throw RDKException("File error: Unable to stat file "+temp);
		}
		size[i]=stats.fSize;
		board[i]=new DataReader(temp);
		header[i]=new Header();
		status[i]=kTRUE;
		//status[i].reset();
	}
	readHeader();
}

bool CalibReader::readNext(Entry &e,RawData &r){
	for(Int_t i=0;i<2;i++){
		if(status[i]){
			status[i]=readBoard(entry[i],rdata[i],i);
			if(status[i]){
				copy(e,r,i);
				return true;
			}
		}
	}
	return false;
	//for(Int_t i=0;i<2;i++){
	//	status[i].set(GOOD,board[i]->good());
	//}
	//for(Int_t i=0;i<2;i++){
	//	if(status[i][GOOD] && !status[i][FULL] && !status[i][END]){
	//		if(readBoard(entry[i],rdata[i],i)){
	//			status[i].set(FULL);
	//		}else{
	//			status[i].set(END);
	//		}
	//	}
	//}
	//if(status[0][FULL] && status[1][FULL]){
	//	if(entry[0].timestamp<entry[1].timestamp){
	//		copy(e,r,0);
	//		status[0].reset(FULL);
	//	}else if(entry[0].timestamp>entry[1].timestamp){
	//		status[1].reset(FULL);
	//		copy(e,r,1);
	//	}else{
	//		status[0].reset(FULL);
	//		status[1].reset(FULL);
	//		merge(e,r);
	//	}
	//}else if(status[0][FULL]){
	//	status[0].reset(FULL);
	//	copy(e,r,0);
	//}else if(status[1][FULL]){
	//	status[1].reset(FULL);
	//	copy(e,r,1);
	//}else{
	//	return false;
	//}
	//return true;
}

void CalibReader::copy(Entry &e,RawData &r,Int_t N){
	for(Int_t i=0;i<NCHANNELS;i++){
		for(Int_t j=0;j<NPOINTS;j++){
			r.ch[i][j]=rdata[N].ch[i][j];
		}
	}
	e.timestamp=entry[N].timestamp;
	e.nevent=entry[N].nevent;
	e.nrun=entry[N].nrun;
	e.nseries=entry[N].nseries;
	//status[N].reset();
}

void CalibReader::merge(Entry &e,RawData &r){
	for(Int_t i=0;i<NCHANNELS;i++){
		for(Int_t j=0;j<NPOINTS;j++){
			if(i<NGAMMA/2){
				r.ch[i][j]=rdata[0].ch[i][j];
			}else if(i<NGAMMA){
				r.ch[i][j]=rdata[1].ch[i][j];
			}else{
				r.ch[i][j]=rdata[(i+1)%2].ch[i][j];
			}
		}
	}
	e.timestamp=entry[0].timestamp;
	e.nevent=entry[0].nevent;
	e.nrun=entry[0].nrun;
	e.nseries=entry[0].nseries;
	//status[0].reset();
	//status[1].reset();
}

bool CalibReader::calExists(Int_t s,Int_t r){
	FileStat_t stats;
	TString path;
	path.Form("%s/ss%i",RDKRAW,s);
	if(gSystem->GetPathInfo(path.Data(),stats)){
		return false;
	}
	for(Int_t i=0;i<2;i++){
		TString temp;
		temp.Form("%s/C%ir%ib%i.dat",path.Data(),s,r,i+1);
		if(gSystem->GetPathInfo(temp,stats)){
			return false;
		}
	}
	return true;
}

const Int_t CalibReader::FULL=0;
const Int_t CalibReader::GOOD=1;
const Int_t CalibReader::END=2;