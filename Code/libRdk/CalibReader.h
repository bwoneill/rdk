//CalibReader.h

#ifndef CALIBREADER_H
#define CALIBREADER_H

#include "RDKReader.h"
#include <bitset>
using namespace std;

class CalibReader: public RDKReader{
public:
	Char_t type;
	Entry entry[2];
	RawData rdata[2];
	//bitset<3> status[2];
	Bool_t status[2];
	void copy(Entry&,RawData&,Int_t);
	void merge(Entry&,RawData&);
	static const Int_t FULL,GOOD,END;
public:
	CalibReader():RDKReader(){}
	CalibReader(Int_t s,Int_t r,Char_t c):RDKReader(s,r),type(c){}
	void open(Int_t,Int_t);
	void initialize();
	bool readNext(Entry&,RawData&);
	static Bool_t calExists(Int_t,Int_t);
};

#endif