//exportTimestamps.cpp

void exportTimestamps(){
	TFile f(Form("%s/second_pass/ss256/S256.root",RDKANAL));
	TTree *tree=(TTree*)f.Get("rdk_second_pass");
	epEvent event;
	tree->SetBranchAddress("epEvent",&event);
	ofstream out(Form("%s/second_pass/ss256/S256.csv",RDKANAL));
	out<<"#,nrun,nevent0,nevent1,timestamp0,timestamp1"<<endl;
	for(Int_t i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		out<<i<<','<<event.nrun<<','<<event.nevent0<<','<<event.nevent1<<',';
		out<<event.timestamp[0]<<','<<event.timestamp[1]<<endl;
	}
	out.close();
}