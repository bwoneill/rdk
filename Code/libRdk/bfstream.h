//bfstream.h

#ifndef BFSTREAM_H
#define BFSTREAM_H

#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/device/file.hpp"

#include "Rtypes.h"

namespace boost{
	class ifstream:public boost::iostreams::stream<boost::iostreams::file_source>{
	private:
		boost::iostreams::file_source *source;
	public:
		ifstream();
		ifstream(const Char_t*,std::ios_base::openmode=std::ios_base::in);
		~ifstream();
		void open(const Char_t*,std::ios_base::openmode=std::ios_base::in);
		std::istream& seekg(Long64_t);
		std::istream& seekg(Long64_t,std::ios_base::seekdir);
		Long64_t tellg();
	};
}

#endif