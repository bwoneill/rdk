//ExeWrapper.h

#include "ExeWrapper.h"
#include "TSystem.h"

ExeWrapper::ExeWrapper(TString cmd){
	setExe(cmd);
}

ExeWrapper::~ExeWrapper(){
}

Int_t ExeWrapper::run(){
	return gSystem->Exec(exe);
}

void ExeWrapper::clean(){
}