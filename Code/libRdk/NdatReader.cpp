//NdatReader.cpp

#include "NdatReader.h"
#include "DataReader.h"

#include "TSystem.h"

NdatReader::NdatReader(Char_t t,Int_t s,Int_t r):type(t),nseries(s),nrun(r){
	TString temp;
	temp.Form("%s/ss%i/%c%ir%ib1.ndat",RDKRAW,nseries,type,nseries,nrun);
	reader=new DataReader(temp);
}

NdatReader::~NdatReader(){
	reader->close();
	delete reader;
}

bool NdatReader::readNext(NData &data){
	Int_t buff[18];
	reader->readData(18,buff);
	data=buff;
	return reader->good();
}