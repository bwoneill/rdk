//RawFile.h

#ifndef RAWFILE_H
#define RAWFILE_H

#include "RDKTypes.h"
#include "Threads.h"

class RDKReader;
class DataReader;
class Header;
class TTree;
class TFile;
enum AnalyzerMode;

class RawFile: public Runnable{
private:
	void createTree();
	RDKReader *reader;
	Long_t size[2];
	DataReader *board[2];
	Header **header;
	RawData rdata;
	Entry entry;
	TTree *rawData;
	TFile *file;
	Char_t type;
public:
	RawFile(Int_t,Int_t);
	RawFile(const Task&);
	RawFile(AnalyzerMode,const Task&);
	Int_t run();
	void clean();
};

#endif