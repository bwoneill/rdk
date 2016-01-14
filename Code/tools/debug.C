//debug.C

#include <cstring>

void debug(){
	TString libRDK;
	libRDK.Form("%sCode/Debug/libRdk.dll",gSystem->Getenv("RDKHOME"));
	gSystem->Load(libRDK);
	FFTW::importWisdom("D:/Documents/Physics/Research/RDK/Code/wisdom");
	gStyle->SetPalette(1);
	CalibrationData::mode.SetBitNumber(2);
	CalibrationData::mode.SetBitNumber(6);
	CalibrationData::mode.SetBitNumber(13);
	strcpy(WORKDIR,gSystem->WorkingDirectory());
}