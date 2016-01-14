//DatReader.h

#ifndef DATREADER_H
#define DATREADER_H

#include "RDKTypes.h"
#include "Header.h"
#include "RDKReader.h"

class DatReader: public RDKReader{
private:
	void readEntry(Entry&,RawData&);
public:
	DatReader():RDKReader(){}
	DatReader(Int_t s,Int_t r):RDKReader(s,r){}
	Header** getHeaders(){return header;}
	void open(Int_t,Int_t);
	void initialize();
	bool readNext(Entry&,RawData&);
	static bool datExists(Int_t,Int_t);
};

#endif