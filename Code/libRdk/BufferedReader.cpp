//BufferedReader.cpp

#include "BufferedReader.h"

#include <iostream>
#include <iomanip>
using std::cout;
using std::endl;
using std::hex;

using std::ios_base;
using boost::ifstream;

#include "TString.h"

union c_to_s{
	c_to_s(Short_t v=0){s=v;}
	Char_t c[sizeof(Short_t)];
	Short_t s;
};

union c_to_i{
	c_to_i(Int_t v=0){i=v;}
	Char_t c[sizeof(Int_t)];
	Int_t i;
};

union c_to_f{
	c_to_f(Float_t v=0){f=v;}
	Char_t c[sizeof(Float_t)];
	Float_t f;
};

union c_to_d{
	c_to_d(Double_t v=0){d=v;}
	Char_t c[sizeof(Double_t)];
	Double_t d;
};

BufferedReader::BufferedReader(Int_t n, const Char_t *file):size(n){
	buffer=new Char_t[size];
	if(file!=NULL){
		open(file);
	}
	position=0;
	end=0;
}

BufferedReader::~BufferedReader(){
	delete[] buffer;
}

void BufferedReader::open(const Char_t *file){
	in.open(file,ios_base::binary);
}

void BufferedReader::close(){
	in.close();
}

Bool_t BufferedReader::is_open() const{
	return in.is_open();
}

Bool_t BufferedReader::good() const{
	return in.good();
}

Bool_t BufferedReader::eof() const{
	return in.eof();
}

Bool_t BufferedReader::eob() const{
	return position<end;
}

Long64_t BufferedReader::tellg(){
	return in.tellg();
}

std::istream& BufferedReader::seekg(Long64_t pos){
	return in.seekg(pos);
}

std::istream& BufferedReader::seekg(Long64_t off, std::ios_base::seekdir dir){
	return in.seekg(off,dir);
}

void BufferedReader::reset(){
	in.clear();
	in.seekg(ios_base::beg);
	position=0;
	end=0;
}

void BufferedReader::read(){
	position=0;
	in.read(buffer,size);
	end=in.gcount();
}

void BufferedReader::read(Int_t n){
	if(n<=size){
		position=0;
		in.read(&buffer[position],n);
		end=in.gcount();
	}else{
		read();
	}
}

Char_t BufferedReader::peak()const{
	return buffer[position];
}

Char_t BufferedReader::getChar(){
	Char_t result='\0';
	if(position<end){
		result=buffer[position++];
	}
	return result;
}

Short_t BufferedReader::getShort(){
	c_to_s temp;
	if(position+sizeof(Short_t)<=end){
		for(Int_t i=0;i<sizeof(Short_t);i++){
			temp.c[i]=buffer[position++];
		}
	}
	return temp.s;
}

Int_t BufferedReader::getInt(){
	c_to_i temp;
	if(position+sizeof(Int_t)<=end){
		for(Int_t i=0;i<sizeof(Int_t);i++){
			temp.c[i]=buffer[position++];
		}
	}
	return temp.i;
}

Float_t BufferedReader::getFloat(){
	c_to_f temp;
	if(position+sizeof(Float_t)<=end){
		for(Int_t i=0;i<sizeof(Float_t);i++){
			temp.c[i]=buffer[position++];
		}
	}
	return temp.f;
}

Double_t BufferedReader::getDouble(){
	c_to_d temp;
	if(position+sizeof(Double_t)<=end){
		for(Int_t i=0;i<sizeof(Double_t);i++){
			temp.c[i]=buffer[position++];
		}
	}
	return temp.d;
}

TString BufferedReader::getString(Char_t delim){
	TString result="";
	for(Int_t i=position;i<end && result=="";i++){
		if(buffer[i]==delim){
			result=TString(&buffer[position],i-position);
		}
	}
	position+=result.Length()+1;
	return result;
}

TString BufferedReader::getString(Int_t n){
	if(position+n>=size){
		n=size-position-1;
	}
	TString result(&buffer[position],n);
	position+=result.Length();
	return result;
}

TString BufferedReader::getLine(){
	return getString('\n');
}

Int_t BufferedReader::copyChar(Char_t *array,Int_t n){
	if(position+n>=size){
		n=size-position-1;
	}
	for(Int_t i=0;i<n;i++){
		array[i]=buffer[position++];
	}
	return n;
}

Int_t BufferedReader::copyShort(Short_t *array,Int_t n){
	if(position+n*sizeof(Short_t)>size){
		cout<<"n ("<<n<<") set to ";
		n=(size-position-1)/sizeof(Short_t);
		cout<<n<<endl;
	}
	c_to_s temp;
	for(Int_t i=0;i<n;i++){
		for(Int_t j=0;j<sizeof(Short_t);j++){
			temp.c[j]=buffer[position++];
		}
		array[i]=temp.s;
	}
	return n;
}

Int_t BufferedReader::copyInt(Int_t *array,Int_t n){
	if(position+n*sizeof(Int_t)>size){
		n=(size-position-1)/sizeof(Int_t);
	}
	c_to_i temp;
	for(Int_t i=0;i<n;i++){
		for(Int_t j=0;j<sizeof(Int_t);j++){
			temp.c[j]=buffer[position++];
		}
		array[i]=temp.i;
	}
	return n;
}

Int_t BufferedReader::copyFloat(Float_t *array,Int_t n){
	if(position+n*sizeof(Float_t)>size){
		n=(size-position-1)/sizeof(Float_t);
	}
	c_to_f temp;
	for(Int_t i=0;i<n;i++){
		for(Int_t j=0;j<sizeof(Float_t);j++){
			temp.c[j]=buffer[position++];
		}
		array[i]=temp.f;
	}
	return n;
}

Int_t BufferedReader::copyDouble(Double_t *array,Int_t n){
	if(position+n*sizeof(Double_t)>size){
		n=(size-position-1)/sizeof(Double_t);
	}
	c_to_d temp;
	for(Int_t i=0;i<n;i++){
		for(Int_t j=0;j<sizeof(Double_t);j++){
			temp.c[j]=buffer[position++];
		}
		array[i]=temp.d;
	}
	return n;
}

void BufferedReader::resize(Int_t n){
	delete[] buffer;
	buffer=new Char_t[n];
	size=n;
	position=0;
	end=0;
}