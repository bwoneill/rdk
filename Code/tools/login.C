//login.C

#include <cstring>

void login(Bool_t debug=false){
	TString libRDK;
	TString config=gROOT->GetConfigOptions();
	if(config.Contains("--enable-minuit2") || config.Contains("--all")){
		cout<<"Loading libMinuit2... "<<flush;
		gSystem->Load("libMinuit2");
		cout<<"Done"<<endl;
	}else{
		cout<<"libRdk requires libMinuit2"<<endl;
		return;
	}
	if(config.Contains("--build=debug") || debug){
		libRDK.Form("%s/Code/bin/debug/libRdk",gSystem->Getenv("RDKHOME"));
	}else{
		libRDK.Form("%s/Code/bin/libRdk",gSystem->Getenv("RDKHOME"));
	}
	cout<<"Loading libRdk... "<<flush;
	gSystem->Load(libRDK);
	cout<<"Done"<<endl;
	TString arch=gSystem->GetBuildArch();
	if(arch.Contains("gcc")){
		cout<<"Loading libgomp... "<<flush;
		gSystem->Load("libgomp");
		cout<<"Done"<<endl;
	}else{
		cout<<"No OpenMP"<<endl;
	}
	gStyle->SetPalette(1);
	strcpy(WORKDIR,gSystem->WorkingDirectory());
}