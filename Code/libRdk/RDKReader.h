//RDKReader.h

#ifndef RDKREADER_H
#define RDKREADER_H

#include "RDKTypes.h"

class Header;
class DataReader;

class RDKReader{
protected:
	void clear(RawData&);
	void readHeader();
	void readFooter();
	Bool_t readBoard(Entry&,RawData&,Int_t);
	Long64_t *size;
	Bool_t isOpen;
	Int_t nseries,nrun;
	DataReader **board;
	Header **header;
public:
	RDKReader();
	RDKReader(Int_t,Int_t);
	~RDKReader();
	virtual void open(Int_t,Int_t)=0;
	void close();
	Header** getHeaders(){return header;}
	virtual void initialize()=0;
	virtual Bool_t readNext(Entry&,RawData&)=0;
};

#endif