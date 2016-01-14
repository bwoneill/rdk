//NdatWriter.h

#ifndef NDATWRITER_H
#define NDATWRITER_H

#include "RDKTypes.h"
#include "NdatReader.h"

class TFile;
class TTree;

class NdatWriter{
private:
	NdatReader *reader;
	NData ndata,total;
	const Int_t nseries,nrun;
	const Double_t start;
	const Bool_t closeAtEnd;
	const Char_t type;
	TFile *file;
	TTree *ndat,*Ndat;
public:
	NdatWriter(Char_t,Int_t,Int_t,Double_t,TFile* =NULL);
	~NdatWriter();
	void write();
};

#endif