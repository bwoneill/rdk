//cooper.cpp

TFile *file=NULL;
TTree *tree;
TH1F *hist1=new TH1F("hist1","E_{e};E_{e}",150,0,30000);
TH2F *hist2=new TH2F("hist2","E_{e} vs FW5%M;FW5%M;E_{e}",100,0,100,150,0,30000),
	*hist3=new TH2F("hist3","E_{e} vs FWHM;FWHM;E_{e}",100,0,100,150,0,30000),
	*hist4=new TH2F("hist4","Normalized E_{e} vs FW5%M;FW5%M;E_{e}",100,0,100,150,0,30000),
	*hist5=new TH2F("hist5","Normalized E_{e} vs FWHM;FWHM;E_{e}",100,0,100,150,0,30000),
	*hist6=new TH2F("hist6","FW5%M vs FWHM;FWHM;FW5%M",70,0,70,100,0,100),
	*hist7=new TH2F("hist7","Normalized FW5%M vs FWHM;FWHM;FW5%M",70,0,70,100,0,100);

void cooper(Double_t threshold=0.05){
	if(file!=NULL && file->IsOpen()){
		file->Close();
	}
	file=TFile::Open(Form("%s/cooper.root",RDKANAL),"recreate");
	tree=new TTree("cooperTest","cooperTest");
	Int_t list[36]={4,5,6,7,8,9,14,15,20,21,22,23,24
		,25,30,31,32,33,34,39,40,41,42,42,44,49
		,50,51,52,53,54,59,60,65,66,67};
	RawFileReader *reader=new RawFileReader();
	Int_t t[5];
	Double_t E;
	tree->Branch("t_e",t,"t[5]/I");
	tree->Branch("E_e",&E,"E/D");
	for(Int_t i=0;i<36;i++){
		reader->open(260,list[i]);
		cooperTest(reader,tree,threshold);
	}
	delete reader;
	tree->Write();
}

void test(){
	RawFileReader reader;
	Analyzer analyzer(kSmooth);
	reader.open(260,4);
	Entry entry;
	Recon recon;
	RawData rdata;
	Double_t E;
	Int_t t[5];
	for(int i=0;i<20;i++){
		reader.readNext(entry,rdata);
		cooperAnalysis(rdata,t,E,0.05);
		analyzer.recon_ep(rdata,recon);
		cout<<E-recon.E_e<<endl;
	}
	reader.close();
}

void makePlots(){
	if(file==NULL || !file->IsOpen()){
		file=TFile::Open(Form("%s/cooper.root",RDKANAL),"read");
		tree=(TTree*)file->Get("cooperTest");
	}
	Int_t t[5];
	Double_t E;
	tree->SetBranchAddress("t_e",t);
	tree->SetBranchAddress("E_e",&E);
	cout<<"filling hist1-3,6"<<endl;
	for(Int_t i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		if(t[3]-t[1]<21 && t[3]-t[1]>16 && t[4]-t[0]-t[3]+t[1]<25 && t[4]-t[0]-t[3]+t[1]>11
				&& E<4000*(44-t[4]+t[0]) && E<4000*(t[4]-t[0]-29)){
			hist1->Fill(E);
			hist2->Fill(t[4]-t[0],E);
			hist3->Fill(t[3]-t[1],E);
			hist6->Fill(t[3]-t[1],t[4]-t[0]);
		}
	}
	cout<<"filling hist4-5,7"<<endl;
	for(Int_t i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		if(t[3]-t[1]<21 && t[3]-t[1]>16 && t[4]-t[0]-t[3]+t[1]<25 && t[4]-t[0]-t[3]+t[1]>11
				&& E<4000*(44-t[4]+t[0]) && E<4000*(t[4]-t[0]-29)){
			Double_t weight=10000/hist1->Interpolate(E);
			hist4->Fill(t[4]-t[0],E,weight);
			hist5->Fill(t[3]-t[1],E,weight);
			hist7->Fill(t[3]-t[1],t[4]-t[0],weight);
		}
	}
	cout<<"done"<<endl;
}