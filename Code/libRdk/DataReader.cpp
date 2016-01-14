// DataReader.cpp

#include "DataReader.h"
#include "bfstream.h"
#include "TString.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::hex;

using std::ios_base;
using boost::ifstream;

DataReader::DataReader(){
}

DataReader::DataReader(const Char_t *file){
	open(file);
}

DataReader::~DataReader(){
	in.close();
	//delete in;
}

void DataReader::open(const Char_t *file){
	in.open(file,ios_base::binary);
}

void DataReader::close(){
	in.close();
}

Bool_t DataReader::is_open()const{
	return in.is_open();
}

Bool_t DataReader::good()const{
	return in.good();
}

Bool_t DataReader::eof()const{
	return in.eof();
}

Long64_t DataReader::tellg(){
	return in.tellg();
}

Int_t DataReader::peek(){
	return in.peek();
}

istream& DataReader::seekg(Long64_t pos){
	return in.seekg(pos);
}

istream& DataReader::seekg(Long64_t off,ios_base::seekdir dir){
	return in.seekg(off,dir);
}

Char_t DataReader::readChar(){
	in.read(buff.Txt,1);
	return buff.Txt[0];
}

TString DataReader::readString(Int_t n){
	if(n>256){
		n=256;
	}
	in.read(buff.Txt,n);
	TString value(buff.Txt,n);
	return value;
}

TString DataReader::readLine(Int_t n){
	if(n>256){
		n=256;
	}
	in.getline(buff.Txt,n);
	TString value(buff.Txt);
	return value;
}

Short_t DataReader::readShort(){
	in.read(buff.Txt,2);
	return buff.Shrt[0];
}

void DataReader::readData(Int_t n,Short_t *array){
	for(Int_t i=0;i<n;i++){
		array[i]=readShort();
	}
}

Int_t DataReader::readInt(){
	in.read(buff.Txt,4);
	return buff.Int[0];
}

void DataReader::readData(Int_t n,Int_t *array){
	//if(n<=64){
	//	for(Int_t i=0;i<n;i++){
	//		cout<<hex<<in.tellg()<<":";
	//		in>>array[i];
	//		cout<<hex<<array[i]<<endl;
	//	}
	//	//in.read(buff.Txt,4*n);
	//	//for(Int_t i=0;i<n;i++){
	//	//	array[i]=buff.Int[i];
	//	//}
	//	//for(Int_t i=0;i<64;i++){
	//	//	cout<<hex<<buff.Int[i]<<endl;
	//	//}
	//}else{
	//	for(Int_t i=0;i<n;i++){
	//		array[i]=readInt();
	//	}
	//}
	for(Int_t i=0;i<n && good();i++){
		array[i]=readInt();
		//cout<<hex<<array[i]<<endl;
	}
}

Long_t DataReader::readLong(){
	in.read(buff.Txt,8);
	return buff.Lng[0];
}

void DataReader::readData(Int_t n,Long_t *array){
	for(Int_t i=0;i<n;i++){
		array[i]=readLong();
	}
}

Double_t DataReader::readDouble(){
	in.read(buff.Txt,8);
	return buff.Dbl[0];
}

void DataReader::readData(Int_t n,Double_t *array){
	//Int_t i=0;
	//while(n>0){
	//	if(n>32){
	//		in.read(buff.Txt,256);
	//	}else{
	//		in.read(buff.Txt,8*n);
	//	}
	//	for(Int_t j=0;j<32 && n>0;j++){
	//		array[i++]=buff.Dbl[j];
	//		n--;
	//	}
	//}
	for(Int_t i=0;i<n;i++){
		array[i]=readDouble();
	}
}

void DataReader::reset(){
	in.clear();
	in.seekg(ios_base::beg);
}