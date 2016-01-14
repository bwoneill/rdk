//TreeReader.h

#ifndef TREEREADER_H
#define TREEREADER_H

#include "RDKTypes.h"

#include "TString.h"

class TChain;

class TreeReader{
private:
	void clean();
public:
	TreeReader();
	~TreeReader();
	TChain *tree;
	epEvent ep;
	void openSeries(TString);
	void reset();
};

#endif