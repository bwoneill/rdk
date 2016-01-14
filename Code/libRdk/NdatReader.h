//NdatReader.h

#ifndef NDATREADER_H
#define NDATREADER_H

#include "RDKTypes.h"

class DataReader;

class NdatReader{
private:
	const Int_t nseries,nrun;
	const Char_t type;
	DataReader *reader;
public:
	NdatReader(Char_t t,Int_t s,Int_t r);
	~NdatReader();
	bool readNext(NData&);
};

#endif