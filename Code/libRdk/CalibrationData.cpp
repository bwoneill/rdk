//CalibrationData.cpp

#include "CalibrationData.h"
#include "RawTreeReader.h"
#include "RDKConfig.h"

#include "TChain.h"
#include "TString.h"
#include "TPRegexp.h"
#include "TH1.h"
#include "TF1.h"
#include <utility>
#include <string>
#include <sstream>
using namespace std;

ClassImp(CalibrationData)

CalibrationData::CalibrationData(){
	file="";
	run=0;
	series=0;
	start=0;
	for(Int_t i=0;i<14;i++){
		for(Int_t j=0;j<4;j++){
			fit[i][j]=0;
		}
	}
}

CalibrationData::CalibrationData(const Char_t *line){
	parseString(line);
}

CalibrationData::CalibrationData(Char_t type,Int_t series,Int_t run,Long64_t start){
	stringstream temp;
	temp<<type<<series<<'r'<<run;
	file=temp.str();
	this->run=run;
	this->series=series;
	this->start=start;
	for(Int_t i=0;i<14;i++){
		for(Int_t j=0;j<4;j++){
			fit[i][j]=0;
		}
	}
	config=rdk2config->getConfig(this->series);
}

CalibrationData::CalibrationData(Char_t type,Int_t series,Int_t run,Long64_t start,
																 const Float_t** cal){
	stringstream temp;
	temp<<type<<series<<'r'<<run;
	file=temp.str();
	this->run=run;
	this->series=series;
	this->start=start;
	for(Int_t i=0;i<14;i++){
		for(Int_t j=0;j<4;j++){
			fit[i][j]=cal[i][j];
		}
	}
}

CalibrationData::~CalibrationData(){
}

void CalibrationData::parseString(const Char_t *input){
	TStringToken token(input,",",true);
	if(token.NextToken()){	//File
		//cout<<token<<",";
		TPRegexp tmp("[SCD*]\\d+r\\d+");
		file=token(tmp).String();
		//Int_t index=fileName.Index("r");
		//TPRegexp digits("\\d+");
		//series=TString(token(digits)).Atoi();
		//run=TString(token(digits,index)).Atoi();
		token.NextToken();	//SeriesNum
		//cout<<token<<",";
		series=token.Atoi();
		token.NextToken();	//RunNum
		//cout<<token<<",";
		run=token.Atoi();
		//token.NextToken();	//BoardNum
		//cout<<token<<",";
		//token.NextToken();	//StartDate2
		//cout<<token;
		//start=token.Atoll();
		for(Int_t i=0;i<12;i++){
			token.NextToken();	//C511a_##
			//cout<<","<<token;
			if(i<6){
				fit[i][0]=token.Atof();
			}else{
				fit[i+1][0]=token.Atof();
			}
		}
		//fit[2][0]=0;
		//fit[6][0]=0;
		//fit[13][0]=0;
		//cout<<endl;
	}
}

string CalibrationData::toString() const{
	stringstream result;
	result<<file.c_str()<<","<<series<<","<<run<<","<<0<<","<<start;
	for(int i=0;i<14;i++){
		result<<","<<fit[i][0];
	}
	return result.str();
}

void CalibrationData::calibrate(Recon &recon) const{
	for(Int_t i=0;i<14;i++){
		if(fit[i][0]>0){
			UInt_t m=RDKConfig::getMode(config,i);
			if(m==BAPD_MODE){
				recon.E_gamma[i]*=5.0;
			}else if(m==BGO_MODE){
				recon.E_gamma[i]*=511;
			}else{
				recon.E_gamma[i]=-1;
			}
			recon.E_gamma[i]/=fit[i][0];
		}else{
			recon.E_gamma[i]=0;
		}
	}
}

Int_t CalibrationData::Compare(const TObject* o) const{
	if(o->ClassName()==this->ClassName()){
		CalibrationData *b=(CalibrationData*)o;
		pair<Int_t,Int_t> c(b->series,b->run);
		if(*this<c){
			return -1;
		}else if(*this==c){
			return 0;
		}else{
			return 1;
		}
	}else{
		return -1;
	}
}

Bool_t CalibrationData::operator <(const pair<Int_t,Int_t> &a)const{
	return series<a.first || (series==a.first && run<a.second);
}
Bool_t CalibrationData::operator <=(const pair<Int_t,Int_t> &a)const{
	return series<a.first || (series==a.first && run<=a.second);
}
Bool_t CalibrationData::operator ==(const pair<Int_t,Int_t> &a)const{
	return series==a.first && run==a.second;
}
Bool_t CalibrationData::operator >=(const pair<Int_t,Int_t> &a)const{
	return series>a.first || (series==a.first && run>=a.second);
}
Bool_t CalibrationData::operator >(const pair<Int_t,Int_t> &a)const{
	return series>a.first || (series==a.first && run>a.second);
}

CalibrationData *CalibrationData::calibrate(Char_t type,Int_t series,Int_t run,
																						Long64_t start){
	CalibrationData *data=new CalibrationData(type,series,run,start);
	for(Int_t i=0;i<14;i++){
		UInt_t m=RDKConfig::getMode(data->config,i);
		if(hist[i]->GetEntries()>0){
			bg_gaus->SetParLimits(0,0,20000);
			bg_gaus->SetParLimits(1,-1e-3,0);
			bg_gaus->SetParLimits(2,0,20000);
			if(m==BAPD_MODE){
				bg_gaus->SetParLimits(3,2500,6000);
				bg_gaus->SetParameters(1e-1,-1.5e-5,5,3600,100);
				bg_gaus->SetRange(2016,7008);
			}else if(m==BGO_MODE){
				bg_gaus->SetParLimits(3,10000,22000);
				bg_gaus->SetParameters(4,-1.5e-4,5,16352,1000);
				bg_gaus->SetRange(8480,24000);
			}
			hist[i]->Fit(bg_gaus,"QRBMN");
			data->fit[i][0]=bg_gaus->GetParameter(3);
			data->fit[i][1]=bg_gaus->GetParError(3);
			data->fit[i][2]=bg_gaus->GetChisquare()/bg_gaus->GetNDF();
		}
		if(m==BAPD_MODE){
			data->fit[i][3]=hist[i]->Integral(63,219);
		}else if(m==BGO_MODE){
			data->fit[i][3]=hist[i]->Integral(265,750);
		}
	}
	return data;
}

void CalibrationData::initCalibration(){
	if(hist==NULL){
		hist=new TH1*[14];
		for(Int_t i=0;i<14;i++){
			Int_t b=i/7,ch=i%7,nBins=MAXHEIGHT/res,max=nBins*res;
			Char_t name[256],title[256];
			sprintf(name,"cal_%i%i",b,ch);
			sprintf(title,"Calibration %i%i",b,ch);
			hist[i]=new TH1F(name,title,nBins,0,max);
			hist[i]->Sumw2();
		}
	}else{
		for(Int_t i=0;i<14;i++){
			hist[i]->Reset("M");
		}
	}
}

void CalibrationData::fill(RawTreeReader *reader){
	initCalibration();
	for(Long64_t i=0;i<reader->tree->GetEntries();i++){
		reader->tree->GetEntry(i);
		for(Int_t j=0;j<14;j++){
			if(reader->recon.t_gamma[j]>0 && reader->recon.t_gamma[j]<NPOINTS
				&& reader->recon.E_gamma[j]>300 && reader->recon.E_gamma[j]<MAXHEIGHT){
				hist[j]->Fill(reader->recon.E_gamma[j]);
			}
		}
	}
}

TH1 **CalibrationData::hist=NULL;
TF1 *CalibrationData::bg_gaus=new TF1("background+gaus","expo+gaus(2)");
Int_t CalibrationData::res=32;
TBits CalibrationData::mode(14);

ostream& operator<<(ostream &out,const CalibrationData &data){
	out<<data.toString().c_str()<<endl;
	return out;
}