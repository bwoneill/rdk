//Header.h

#ifndef RDKHEADER_H
#define RDKHEADER_H

#include "TMap.h"
#include "TString.h"

#include <vector>

using std::vector;

class Header{
private:
	Bool_t isOwner;
	TMap *map;
	static void parseString(TString,TString&,TString&);
	void initialize();
public:
	Header();
	Header(TMap*);
	~Header();
	inline TMap* getHeader() const {return map;}
	TMap* getSection(TString)const;
	TString& getValue(TString) const;
	void addSection(TString name);
	Int_t setValue(TString,TString);
	Int_t appendValue(TString,TString);
	vector<TString> getListOfSections() const;
	vector<TString> getListOfKeys(TString) const;
	void printTree() const;
};

#endif