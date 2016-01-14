//BufferedReader.h

#ifndef BUFFEREDREADER_H
#define BUFFEREDREADER_H

#include <istream>

#ifndef __CINT__
#include "bfstream.h"
#else
#include <fstream>
namespace boost{
	class ifstream:std::ifstream{
	};
};
#endif

#include "Rtypes.h"

class TString;

class BufferedReader{
private:
	Char_t *buffer;
	Int_t size,position,end;
	boost::ifstream in;
public:
	BufferedReader(Int_t=256,const Char_t* =NULL);
	~BufferedReader();
	void open(const Char_t*);
	void close();
	Bool_t is_open()const;
	Bool_t good()const;
	Bool_t eof()const;
	Bool_t eob()const;
	Long64_t tellg();
	std::istream& seekg(Long64_t);
	std::istream& seekg(Long64_t,std::ios_base::seekdir);
	void reset();
	void read();
	void read(Int_t);
	Char_t peak()const;
	Int_t getEnd()const{return end;}
	Int_t getSize()const{return size;}
	Int_t getPosition()const{return position;}
	void setPosition(Int_t p){position=p;}
	Char_t getChar();
	Short_t getShort();
	Int_t getInt();
	Float_t getFloat();
	Double_t getDouble();
	TString getString(Char_t);
	TString getString(Int_t);
	TString getLine();
	Int_t copyChar(Char_t *,Int_t);
	Int_t copyShort(Short_t *,Int_t);
	Int_t copyInt(Int_t *,Int_t);
	Int_t copyFloat(Float_t *,Int_t);
	Int_t copyDouble(Double_t *,Int_t);
	void resize(Int_t);
};

#endif