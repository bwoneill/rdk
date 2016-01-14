//CalibrationList.cpp

#include "CalibrationList.h"
#include "CalibrationData.h"
#include "TPRegexp.h"
#include <fstream>
#include <iostream>

using namespace std;

ClassImp(CalibrationList);

CalibrationList::CalibrationList(){
	current=NULL;
}

CalibrationList::CalibrationList(TString file){
	current=NULL;
	ifstream infile(file.Data());
	TString buff1,buff2;
	if(infile.good()){
		buff1.ReadLine(infile);
	}
	while(infile.good()){
		buff1.ReadLine(infile);
		TPRegexp tmp("[SC*]\\d+r\\d+[b]*\\d*.dat");
		TString fileName=buff1(tmp).String();
		//cout<<fileName<<endl;
		if(!fileName.Contains("b")){
			Add(new CalibrationData(buff1));
		}else	if(infile.good()){
			buff2.ReadLine(infile);
			Add(new CalibrationData(buff1));
		}
	}
	infile.close();
}

CalibrationList::~CalibrationList(){
	current=NULL;
}

CalibrationData *CalibrationList::getCal(Int_t series,Int_t run){
	return getCal(make_pair(series,run));
}

CalibrationData *CalibrationList::getCal(const Entry &entry){
	return getCal(make_pair(entry.nseries,entry.nrun));
}

CalibrationData *CalibrationList::getCal(const pair<Int_t,Int_t> &time){
	if(!current){
		current=FirstLink();
	}
	while(current){
		CalibrationData *here=(CalibrationData*)current->GetObject();
		if(*here==time){
			return here;
		}else if(*here>time){
			current=current->Prev();
			if(current){
				CalibrationData *prev=(CalibrationData*)current->GetObject();
				if(*prev<time){
					return prev;
				}
			}
		}else if(*here<time){
			current=current->Next();
			if(current){
				CalibrationData *next=(CalibrationData*)current->GetObject();
				if(*next>time){
					return here;
				}
			}
		}else{
			return NULL;
		}
	}
	return NULL;
}

Recon CalibrationList::calibrate(const Entry &entry,const Recon &recon){
	Recon temp(recon);
	CalibrationData *data=this->getCal(entry);
	data->calibrate(temp);
	return temp;
}

ostream& operator<<(ostream &out,const CalibrationList &list){
	out<<"File,SeriesNum,RunNum,BoardNum,StartDate2,C511a_00,C511a_01,";
	out<<"C511a_02,C511a_03,C511a_04,C511a_05,C511a_06,C511a_10,C511a_11,C511a_12,";
	out<<"C511a_13,C511a_14,C511a_15,C511a_16"<<endl;
	for(Int_t i=0;i<list.GetEntries();i++){
		CalibrationData *data=(CalibrationData*)list.At(i);
		out<<*data;
	}
	return out;
}