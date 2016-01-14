//analyzerWrapper.cpp

#include "tools/login.C"

void analyzerWrapper(Char_t t,Int_t s,Int_t r,UInt_t config){
	login();
	Task task(Form("ss%i",s),s,r,t,config);
	cout<<task<<endl;
	ThreadedAnalyzer analyzer(task);
	analyzer.run();
	analyzer.clean();
	gSystem->Exit(0);
}