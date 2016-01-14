// crunchAll.cpp

void crunchAll(){
	gROOT->ProcessLine(".x tools/linuxLogin.cpp");
	crunch(buildQueue("SCD"),-4);
	gROOT->ProcessLine(".q");
}