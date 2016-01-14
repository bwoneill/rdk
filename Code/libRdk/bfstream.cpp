//bfstream.cpp

#include "bfstream.h"
#include "boost/iostreams/positioning.hpp"

using std::ios_base;
using boost::iostreams::file_source;
using boost::iostreams::stream;

namespace boost{
	ifstream::ifstream(){
		source=NULL;
	}

	ifstream::ifstream(const Char_t *path,ios_base::openmode mode){
		source=NULL;
		open(path,mode);
	}

	ifstream::~ifstream(){
		close();
		delete source;
	}

	void ifstream::open(const Char_t *path,ios_base::openmode mode){
		if(source){
			delete source;
		}
		if(is_open()){
			close();
		}
		source=new file_source(path,mode);
		stream<file_source>::open(*source);
	}

	std::istream& ifstream::seekg(Long64_t pos){
		boost::iostreams::seek(*this,pos,BOOST_IOS::beg);
		return *this;
	}

	std::istream& ifstream::seekg(Long64_t pos,std::ios_base::seekdir dir){
		boost::iostreams::seek(*this,pos,dir);
		return *this;
	}

	Long64_t ifstream::tellg(){
		return boost::iostreams::position_to_offset(stream<file_source>::tellg());
	}
}