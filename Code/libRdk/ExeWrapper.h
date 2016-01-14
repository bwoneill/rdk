//ExeWrapper.h

#ifndef EXEWRAPPER_H
#define EXEWRAPPER_H

#include "Runnable.h"

#include "TString.h"

class ExeWrapper: public Runnable{
private:
	TString exe;
public:
	ExeWrapper(TString="");
	~ExeWrapper();
	inline void setExe(TString cmd){exe=cmd;}
	Int_t run();
	void clean();
};

#endif