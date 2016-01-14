//RawFileReader.cpp

#include "RawFileReader.h"
#include "DataReader.h"
#include "Header.h"
#include "RawTreeReader.h"
#include "BufferedReader.h"

#include "TSystem.h"

#include <climits>
#include <iostream>
using namespace std;

RawFileReader::RawFileReader(){
	nseries=0;
	nrun=0;
	isOpen=kFALSE;
	header=NULL;
	size=NULL;
	foot_size=NULL;
	bboard=NULL;
}

RawFileReader::RawFileReader(Char_t t,Int_t series,Int_t run){
	type=t;
	nseries=series;
	nrun=run;
	isOpen=kFALSE;
	header=NULL;
	size=NULL;
	foot_size=NULL;
	bboard=NULL;
	open(type,nseries,nrun);
}

RawFileReader::~RawFileReader(){
	close();
}

void RawFileReader::open(Char_t t,Int_t series,Int_t run){
	if(isOpen){
		close();
	}
	bboard=new BufferedReader[2];
	bboard[0].resize(eventSize);
	bboard[1].resize(eventSize);
	header=new Header[2];
	size=new Long64_t[2];
	foot_size=new Long64_t[2];
	nseries=series;
	nrun=run;
	type=t;
	if(type=='S'){
		current=-1;
	}else if(type=='C' || type=='D'){
		current=0;
	}
	TString path;
	if(type){
		FileStat_t stats;
		for(Int_t i=0;i<2;i++){
			path.Form("%s/ss%i/%c%ir%ib%i.dat",RDKRAW,nseries,type,nseries,nrun,i+1);
			if(!gSystem->GetPathInfo(path,stats)){
				size[i]=stats.fSize;
				bboard[i].open(path);
				header[i].getHeader()->DeleteAll();
			}else{
				path.Form("Unable to stat ss%i/%c%ir%ib%i.dat",nseries,type,nseries,nrun,i+1);
				throw RDKException(path);
			}
		}
		if(type=='S' && size[0]!=size[1]){
			close();
			path.Form("File error: sizes of ss%i/S%ir%i*.dat do not match",nseries,nseries,nrun);
			throw RDKException(path);
		}
		for(Int_t i=0;i<2;i++){
			foot_size[i]=size[i];
			size[i]=(size[i]-5000)/eventSize;
			foot_size[i]-=5000+size[i]*eventSize;
		}
		isOpen=kTRUE;
		readHeader();
	}else{
		path.Form("Unable to type ss%i/*%ir%i",nseries,nseries,nrun);
		throw RDKException(path);
	}
}

void RawFileReader::close(){
	if(bboard){
		for(Int_t i=0;i<2;i++){
			if(bboard[i].is_open()){
				bboard[i].close();
			}
		}
		delete[] bboard;
		bboard=NULL;
	}
	if(header){
		delete[] header;
		header=NULL;
	}
	if(size){
		delete[] size;
		size=NULL;
	}
	isOpen=kFALSE;
}

void RawFileReader::initialize(){
	open(type,nseries,nrun);
}

void RawFileReader::readHeader(){
	for(Int_t i=0;i<2;i++){		//reader file header
		bboard[i].reset();
		bboard[i].read(5000);
		if(bboard[i].getInt()!=100){
			TString temp;
			temp.Form("Unknown file format: Board %i does not start with 100",i+1);
			throw RDKException(temp);
		}
		TString currentSection="Header",description="",line;
		while(bboard[i].peak() && !bboard[i].eob()){
			line=bboard[i].getLine();
			if(line=="" || line.BeginsWith(";")){
				continue;
			}else if(line.BeginsWith("[")){
				const Char_t* temp="[] ";
				currentSection=line;
				for(Int_t j=0;temp[j]!='\0';j++){
					currentSection=currentSection.Strip(TString::kBoth,temp[j]);
				}
				header[i].addSection(currentSection);
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
				header[i].setValue(temp,value);
			}else{
				description+=line+"\n";
			}
		}
		header[i].setValue("Header.Descritption",description);
	}
	if(type=='S'){
		seek(size[0]);
	}else{
		seek(size[0]+size[1]);
	}
	for(Int_t i=0;i<2;i++){
		bboard[i].read(foot_size[i]);
		while(!bboard[i].eob()){
			TString line=bboard[i].getLine(),key,value;
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
			header[i].setValue("Footer."+key,value);
		}
		bboard[i].reset();
	}
	seek(0);
}

Bool_t RawFileReader::readBoard(Entry &entry,RawData &rdata,Int_t N){
	if(N!=0 && N!=1){
		throw RDKException("Index out of range");
	}
	if(!bboard[N].good()){
		return false;
	}
	bboard[N].read();
	Bool_t startEntry=true;
	TString line;
	for(Int_t j=0;j<2;j++){
		if(!startEntry){
			break;
		}
		line=bboard[N].getString(4);
		startEntry&=line=="****";
	}
	if(!startEntry){
		return false;
	}
	entry.nseries=nseries;
	entry.nrun=nrun;
	bboard[N].getInt();
	entry.timestamp[N]=bboard[N].getDouble();
	Int_t n=bboard[N].getInt();
	if(n!=NCHANNELS/2){
		TString temp;
		temp.Form("Unknown file format: %X != %X in board %i at %X",n,NCHANNELS/2,N+1,(Int_t)bboard[N].tellg()-4);
		throw RDKException(temp);
	}
	Int_t hp=bboard[N].getInt();
	if(hp!=0xFF){
		TString temp;
		temp.Form("Unknown file format: %X != FF in board %i at %X",hp,N+1,(Int_t)bboard[N].tellg()-4);
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
		bboard[N].copyShort(rdata.ch[i],NPOINTS);
	}
	Int_t mult=(NCHANNELS-NGAMMA)/2;
	for(Int_t i=0;i<mult;i++){
		bboard[N].copyShort(rdata.ch[NGAMMA+2*i+1-N],NPOINTS);
	}
	return true;
}

void RawFileReader::clear(RawData &rdata){
	for(Int_t i=0;i<NCHANNELS;i++){
		for(Int_t j=0;j<NPOINTS;j++){
			rdata.ch[i][j]=0;
		}
	}
}

Bool_t RawFileReader::readNext(Entry &entry,RawData &rdata){
	Bool_t success=kTRUE;
	Long64_t position=tell();
	if(type=='S'){
		for(Int_t i=0;i<2;i++){
			success&=readBoard(entry,rdata,i);
		}
		entry.nevent=position;
	}else if(type=='C' || type=='D'){
		if(current==0 || current==1){
			clear(rdata);
			success=readBoard(entry,rdata,current);
			if(success){
				entry.nevent=position;
				position++;
				if(position>=0 && position<size[0]){
					current=0;
				}else if(position<size[0]+size[1]){
					current=1;
				}else{
					current=-1;
				}
			}else{
				current=-1;
			}
		}else{
			success=kFALSE;
		}
	}
	return success;
}

Bool_t RawFileReader::seek(Int_t i){
	Bool_t success=kFALSE;
	if(type=='S'){
		if(i>=0 && i<=size[0]){
			Long64_t position=5000+eventSize*i;
			bboard[0].seekg(position);
			bboard[1].seekg(position);
			success=kTRUE;
		}
	}else if(type=='C' || type=='D'){
		if(i>=0 && i<size[0]){
			current=0;
			Long64_t position=5000+eventSize*i;
			bboard[0].seekg(position);
			bboard[1].seekg(5000);
		}else if(i<=size[0]+size[1]){
			current=1;
			Long64_t position0=5000+size[0]*eventSize;
			Long64_t position1=5000+(i-size[0])*eventSize;
			bboard[0].seekg(position0);
			bboard[1].seekg(position1);
		}else{
			current=-1;
			TString temp;
			temp.Form("Out of bounds error: %ll",i);
			throw RDKException(temp);
		}
		success=kTRUE;
	}
	return success;
}

Long64_t RawFileReader::tell() const{
	Long64_t result=-1;
	if(type=='S' && bboard[0].good()){
		result=(bboard[0].tellg()-5000)/eventSize;
	}else if((type=='C' || type=='D') && (current==0 || current==1) && bboard[current].good()){
		result=size[0]*current+(bboard[current].tellg()-5000)/eventSize;
	}
	return result;
}

Long64_t RawFileReader::getSize(Int_t board)const{
	if(board==0 || board==1){
		return size[board];
	}
	return -1;
}

const Long64_t RawFileReader::eventSize=8*NPOINTS*sizeof(Short_t)
	+8*sizeof(Char_t)+3*sizeof(Int_t)+sizeof(Double_t);
