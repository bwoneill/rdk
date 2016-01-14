//DataReader.h

#ifndef DATAREADER_H
#define DATAREADER_H

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

union buffer_t{
	Char_t Txt[256];
	Short_t Shrt[128];
	Int_t Int[64];
	Float_t Flt[64];
	Long64_t Lng[32];
	Double_t Dbl[32];
};

class DataReader{
	protected:
		buffer_t buff;
		boost::ifstream in;
	public:
		DataReader();
		DataReader(const Char_t*);
		~DataReader();
		void open(const Char_t*);
		void close();
		Bool_t is_open()const;
		Bool_t good()const;
		Bool_t eof()const;
		Long64_t tellg();
		std::istream& seekg(Long64_t);
		std::istream& seekg(Long64_t,std::ios_base::seekdir);
		Int_t peek();
		Char_t readChar();
		Short_t readShort();
		Int_t readInt();
		Long_t readLong();
		Double_t readDouble();
		TString readString(Int_t);
		TString readLine(Int_t=256);
		void readData(Int_t,Short_t*);
		void readData(Int_t,Int_t*);
		void readData(Int_t,Long_t*);
		void readData(Int_t,Double_t*);
		void reset();
};

#endif