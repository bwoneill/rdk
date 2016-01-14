//RawTreeReader.h

#ifndef RawTreeReader_H
#define RawTreeReader_H

#include "RDKTypes.h"
#include <vector>
#include <fstream>

class TChain;
class TreeFilter;

class RawTreeReader{
private:
	std::ofstream log;
	const TreeFilter *filter;
	const Char_t *path;
	Char_t *logPath;
	void clean();
	void openLog(Bool_t=kFALSE);
public:
	RawTreeReader(Bool_t=kFALSE);
	RawTreeReader(TreeFilter*,const Char_t* =RDKANAL,Bool_t=kFALSE);
	RawTreeReader(Double_t,Double_t,Double_t,const Char_t* =RDKANAL,Bool_t=kFALSE);
	~RawTreeReader();
	TChain *tree,*ndat;
	Recon recon;
	Entry entry;
	RawData rdata;
	NData ndata;
	void reset(Bool_t=kFALSE);
	NData openFile(Char_t,Int_t,Int_t);
	NData openSeries(Int_t,const Char_t*);
	NData openSeries(Int_t,const Int_t*,const Char_t*);
	NData openList(const Char_t*);
	static const Int_t nSeries=35,allSeries[nSeries],n1400=45,all1400[n1400];
	static std::vector<Task> getList(Int_t,const Char_t*,const Char_t* =RDKANAL);
	static std::vector<Task> getRawList(Int_t,const Char_t*,const Char_t* =RDKRAW);
	static std::vector<Int_t> getSeries(const Char_t*);
};

#endif