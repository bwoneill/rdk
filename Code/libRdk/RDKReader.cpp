//RDKReader.cpp

#include "RDKReader.h"
#include "DataReader.h"
#include "Header.h"

#include "TSystem.h"

#include <iostream>
using namespace std;

RDKReader::RDKReader():nseries(0),nrun(0){
	board=new DataReader*[2];
	header=new Header*[2];
	size=new Long64_t[2];
	isOpen=kFALSE;
}

RDKReader::RDKReader(Int_t s,Int_t r):nseries(s),nrun(r){
	board=new DataReader*[2];
	header=new Header*[2];
	size=new Long64_t[2];
	isOpen=kTRUE;
}

RDKReader::~RDKReader(){
	close();
}

void RDKReader::close(){
	if(board){
		for(Int_t i=0;i<2;i++){
			if(board[i]){
				if(board[i]->is_open()){
					board[i]->close();
				}
			}
		}
		delete[] board;
	}
	if(header){
		for(Int_t i=0;i<2;i++){
			if(header[i]){
				delete header[i];
			}
		}
	}
	if(size){
		delete[] size;
	}
	isOpen=kFALSE;
}

void RDKReader::readHeader(){
	for(Int_t i=0;i<2;i++){		//reader file header
		board[i]->reset();
		if(board[i]->readInt()!=100){
			TString temp;
			temp.Form("Unknown file format: Board %i does not start with 100",i+1);
			throw RDKException(temp);
		}
		TString currentSection="Header",description="",line;
		while(board[i]->peek() && board[i]->tellg()<5000){
			line=board[i]->readLine();
			if(line=="" || line.BeginsWith(";")){
				continue;
			}else if(line.BeginsWith("[")){
				const Char_t* temp="[] ";
				currentSection=line;
				for(Int_t j=0;temp[j]!='\0';j++){
					currentSection=currentSection.Strip(TString::kBoth,temp[j]);
				}
				header[i]->addSection(currentSection);
			}else if(currentSection=="Header"){
				description+=line+"\n";
			}else if(line.Contains("=") || line.Contains(":")){
				TString d=line.Contains("=")?"=":":";
				Int_t j=line.Index(d);
				TString key=line(0,j),value=line(j+1,line.Length());
				key=key.Strip();
				if(value.Contains(";")){
					value=value(0,value.Index(";"));
				}
				value=value.Strip();
				TString temp=currentSection+"."+key;
				header[i]->setValue(temp,value);
			}else{
				description+=line+"\n";
				//throw RDKException("Unknown header format: Unrecognized string \""+line+"\"");
			}
		}
		header[i]->setValue("Header.Descritption",description);
		board[i]->seekg(5000,ios::beg);
	}
}

void RDKReader::readFooter(){
	for(Int_t i=0;i<2;i++){
		while(!board[i]->eof()){
			TString line=board[i]->readLine(),key,value;
			Int_t j;
			if(line=="" || line.BeginsWith(";")){
				continue;
			}else	if(line.Contains("=")){
				j=line.Index("=");
			}else if(line.Contains(":")){
				j=line.Index(":");
			}else{
				j=line.Length();
			}
			key=line(0,j);
			key=key.Strip();
			value=line(j+1,line.Length());
			value=value.Strip();
			header[i]->setValue("Footer."+key,value);
		}
	}
}

Bool_t RDKReader::readBoard(Entry &entry,RawData &rdata,Int_t N){
	if(N!=0 && N!=1){
		throw RDKException("Index out of range");
	}
	if(!board[N]->good()){
		return false;
	}
	Bool_t startEntry=true;
	TString line;
	for(Int_t j=0;j<2;j++){
		if(!startEntry){
			break;
		}
		line=board[N]->readString(4);
		startEntry&=line=="****";
	}
	if(!startEntry){
		return false;
	}
	entry.nseries=nseries;
	entry.nrun=nrun;
	entry.nevent=board[N]->readInt();
	entry.timestamp[N]=board[N]->readDouble();
	//Double_t timestamp=board[1]->readDouble();
	Int_t n=board[N]->readInt();	//number of waves=8
	if(n!=NCHANNELS/2){
		TString temp;
		temp.Form("Unknown file format: %X != %X in board %i at %X",n,NCHANNELS/2,N+1,(Int_t)board[N]->tellg()-4);
		throw RDKException(temp);
	}
	Int_t hp=board[N]->readInt();
	if(hp!=0xFF){
		TString temp;
		temp.Form("Unknown file format: %X != FF in board %i at %X",hp,N+1,(Int_t)board[N]->tellg()-4);
		throw RDKException(temp);
	}
	Int_t min,max;
	if(N==0){
		min=0;
		max=NGAMMA/2;
	}else{
		min=NGAMMA/2;
		max=NGAMMA;
	}
	for(Int_t i=min;i<max;i++){
		board[N]->readData(NPOINTS,rdata.ch[i]);
	}
	Int_t mult=(NCHANNELS-NGAMMA)/2;
	for(Int_t i=0;i<mult;i++){
		board[N]->readData(NPOINTS,rdata.ch[NGAMMA+2*i+1-N]);
	}
	return true;
}

void RDKReader::clear(RawData &rdata){
	for(Int_t i=0;i<NCHANNELS;i++){
		for(Int_t j=0;j<NPOINTS;j++){
			rdata.ch[i][j]=0;
		}
	}
}