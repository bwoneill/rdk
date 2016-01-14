//crunchFromFile.cpp

void crunchFromFile(const Char_t *fileName,const Char_t *types="SCD"){
	SysInfo_t info;
	gSystem->GetSysInfo(&info);
	if(info.fOS=="Linux"){
		gROOT->ProcessLine(".x tools/linuxLogin.cpp");
	}else{
		gROOT->ProcessLine(".x tools/login.C");
	}
	TThread::Sleep(100);
	crunchFromFile(fileName,types);
}