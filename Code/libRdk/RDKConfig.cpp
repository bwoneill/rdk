//RDKConfig.cpp

#include "RDKConfig.h"
#include "TPRegexp.h"

#include <iostream>
#include <fstream>
using namespace std;

RDKConfig::RDKConfig(TString path){
	if(!config_map.empty()){
		config_map.clear();
	}
	ifstream in(path);
	if(in.is_open()){
		TString line;
		line.ReadLine(in);
		while(!in.eof()){
			line.ReadLine(in);
			TStringToken token(line,",",kTRUE);
			token.NextToken();
			UInt_t series=token.Atoi();
			token.NextToken();
			token.NextToken();
			token.NextToken();
			token.NextToken();
			token.NextToken();
			TString temp(token);
			if(temp.BeginsWith("0x")){
				temp.Remove(0,2);
				temp=TString::BaseConvert(temp,16,10);
			}
			UInt_t config=temp.Atoi();
			config_map[series]=config;
		}
	}else{
		cerr<<"No configuration file found"<<endl;
	}
}

RDKConfig::~RDKConfig(){
}

UInt_t RDKConfig::getConfig(UInt_t series){
	return UInt_t(config_map[series]);
}

UInt_t RDKConfig::getMode(UInt_t config,UInt_t ch){
	UInt_t i=ch;
	if(i<14){
		i=(i+i/7)<<1;
	}else if(i==15){
		i<<=1;
	}
	UInt_t mode=(config>>i)&3U;
	if(mode==BGO_MODE && ch==2){
		mode=0U;
	}
	return mode;
}