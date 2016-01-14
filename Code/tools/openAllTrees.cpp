//openAllTrees.cpp

TChain *chain=NULL;

void openAllTrees(){
	if(chain){
		delete chain;
	}
	chain=new TChain("rdk_second_pass");
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[47]={76,77,78,79,87,89,91,93,94,95,104,105,106,119,120,126,127,151,153,155,156,160,161,162,165,166,175,176,185,187,215,223,227,228,253,257,270,289,290,291,292,304,305,311,313,314,317};
	for(Int_t i=0;i<47;i++){
		TString path;
		path.Form(temp,series[i],series[i]);
		chain->Add(path);
	}
}

void openSet1(){
	if(chain){
		delete chain;
	}
	chain=new TChain("rdk_second_pass");
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[11]={227,228,253,257,270,289,290,291,292,304,305};
	for(Int_t i=0;i<11;i++){
		TString path;
		path.Form(temp,series[i],series[i]);
		chain->Add(path);
	}
}

void openSet2(){
	if(chain){
		delete chain;
	}
	chain=new TChain("rdk_second_pass");
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[12]={76,77,78,79,87,89,91,93,94,95,215,223};
	for(Int_t i=0;i<12;i++){
		TString path;
		path.Form(temp,series[i],series[i]);
		chain->Add(path);
	}
}

void openSet2a(){
	if(chain){
		delete chain;
	}
	chain=new TChain("rdk_second_pass");
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[10]={76,77,78,79,87,89,91,93,94,95};
	for(Int_t i=0;i<10;i++){
		TString path;
		path.Form(temp,series[i],series[i]);
		chain->Add(path);
	}
}

void openSet2b(){
	if(chain){
		delete chain;
	}
	chain=new TChain("rdk_second_pass");
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[2]={215,223};
	for(Int_t i=0;i<2;i++){
		TString path;
		path.Form(temp,series[i],series[i]);
		chain->Add(path);
	}
}

void openSet3(){
	if(chain){
		delete chain;
	}
	chain=new TChain("rdk_second_pass");
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[20]={104,105,106,119,120,126,127,151,153,155,156,160,161,162,165,166,175,176,185,187};
	for(Int_t i=0;i<20;i++){
		TString path;
		path.Form(temp,series[i],series[i]);
		chain->Add(path);
	}
}

void openSet4(){
	if(chain){
		delete chain;
	}
	chain=new TChain("rdk_second_pass");
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[4]={311,313,314,317};
	for(Int_t i=0;i<4;i++){
		TString path;
		path.Form(temp,series[i],series[i]);
		chain->Add(path);
	}
}