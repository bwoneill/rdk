//FitTest.cpp

TFitterMinuit *fitter=NULL;
EpFitFunction epfunc(2048);
GFitFunction gfunc(2048);
EpgFitFunction epgfunc(2048);

void FitTest(){
	RawFileReader reader('C',72,0);
	Entry entry;
	RawData rdata;
	reader.readNext(entry,rdata);
	if(fitter==NULL){
		cout<<"Initializing fitter"<<endl;
		TVirtualFitter::SetDefaultFitter("Minuit2");
		fitter=(TFitterMinuit*)TVirtualFitter::Fitter(0,7);
		fitter->SetPrintLevel(-1);
		fitter->CreateMinimizer();
		fitter->SetMinuitFCN(&epgfunc);
	}
	cout<<"Filling fit function"<<endl;
	for(Int_t i=0;i<2048;i++){
		//epfunc.setPoint(i,rdata.ch[15][i]);
		epgfunc.setPoint(i,rdata.ch[15][i]);
	}
	cout<<"Starting ep fit"<<endl;
	fitEP();
	for(Int_t ch=0;ch<14;ch++){
		cout<<"Filling fit function"<<endl;
		for(Int_t i=0;i<2048;i++){
			//gfunc.setPoint(i,rdata.ch[ch][i]);
			epgfunc.setPoint(i,rdata.ch[ch][i]);
		}
		cout<<"Starting g fit"<<endl;
		fitG();
	}
}

void fitEP(){
	cout<<"Setting ep parameters"<<endl;
	cout<<"p0,"<<flush;
	fitter->SetParameter(0,"p0",5000,1,0,30000);
	cout<<"p1,"<<flush;
	fitter->SetParameter(1,"p1",660,1,0,2048);
	cout<<"p2,"<<flush;
	fitter->SetParameter(2,"p2",5,1,0,100);
	cout<<"p3,"<<flush;
	fitter->SetParameter(3,"p3",500,1,0,30000);
	cout<<"p4,"<<flush;
	fitter->SetParameter(4,"p4",800,1,0,2048);
	cout<<"p5,"<<flush;
	fitter->SetParameter(5,"p5",5,1,0,100);
	if(fitter->IsFixed(5)){
		cout<<"Unlocking parameter 5"<<endl;
		fitter->ReleaseParameter(5);
	}
	cout<<"p6"<<endl;
	fitter->SetParameter(6,"p6",0,1,-3000,3000);
	if(fitter->IsFixed(6)){
		cout<<"Unlocking parameter 6"<<endl;
		fitter->ReleaseParameter(6);
	}
	cout<<"Minimize"<<endl;
	fitter->Minimize();
	cout<<"Done"<<endl;
}

void fitG(){
	cout<<"Setting parameters"<<endl;
	fitter->SetParameter(0,"A",0,10,-30000,30000);
	fitter->SetParameter(1,"B",1000,100,0,30000);
	fitter->SetParameter(2,"C",1e-3,1e-3,0,1);
	fitter->SetParameter(3,"D",4e-3,1e-3,0,1);
	fitter->SetParameter(4,"t",660,1,0,2048);
	cout<<"Locking parameters"<<endl;
	fitter->FixParameter(5);
	fitter->FixParameter(6);
	cout<<"Minimize"<<endl;
	fitter->Minimize();
	cout<<"Done"<<endl;
}