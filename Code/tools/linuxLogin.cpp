//linuxLogin.cpp

#include <cstring>

void linuxLogin(){
	TString config=gROOT->GetConfigOptions();
	if(config.Contains("debug")){
		gSystem->Load(Form("%s/Code/bin/debug/libRdk.so",gSystem->Getenv("RDKHOME")));
	}else{
		gSystem->Load("libRdk");
	}
	gStyle->SetPalette(1);
	CalibrationData::mode.SetBitNumber(2);
	CalibrationData::mode.SetBitNumber(6);
	CalibrationData::mode.SetBitNumber(13);
	strcpy(WORKDIR,gSystem->WorkingDirectory());
}