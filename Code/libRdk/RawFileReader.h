//RawFileReader.h

#ifndef RAWFILEREADER_H
#define RAWFILEREADER_H

#include "RDKTypes.h"

class Header;
class DataReader;
class BufferedReader;

class RawFileReader{
private:
	Char_t type;
	Int_t current;
	void clear(RawData&);
	void readHeader();
	//void readFooter();
	Bool_t readBoard(Entry&,RawData&,Int_t);
	Long64_t *size,*foot_size;
	Bool_t isOpen;
	Int_t nseries,nrun;
	//DataReader *board;
	BufferedReader *bboard;
	Header *header;
public:
	RawFileReader();
	RawFileReader(Char_t,Int_t,Int_t);
	~RawFileReader();
	void close();
	void open(Char_t,Int_t,Int_t);
	void initialize();
	Bool_t readNext(Entry&,RawData&);
	Bool_t seek(Int_t);
	Long64_t tell()const;
	Long64_t getSize(Int_t)const;
	Header* getHeaders(){return header;}
	static const Long64_t eventSize;
};

#endif