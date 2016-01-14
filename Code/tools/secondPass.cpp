//secondPass.cpp

TFile f(Form("%s/second_pass/ss256/S256.root",RDKANAL));
TTree *tree=(TTree*)f.Get("rdk_second_pass");
ULong64_t t0,t1,mask=SecondPass::mask,max=1ll<<39;
epEvent epevent;
TGraph g;
Long64_t bitError=Long64_t(1)<<31;

//void secondPass(){
//	Int_t k=0;
//	tree->SetBranchAddress("epEvent",&epevent);
//	for(Int_t i=0;i<tree->GetEntries();i++){
//		tree->GetEntry(i);
//		t0=epevent.timestamp[0];
//		t1=epevent.timestamp[1];
//		if(t0>0 && t1>0 && t0<max && t1<max){
//			if(fabs(Double_t(t0)-Double_t(t1))>fabs(Double_t(t0&mask)-Double_t(t1))){
//				t0&=mask;
//			}
//			g.SetPoint(k++,t1,Double_t(t0)-Double_t(t1));
//		}
//	}
//	g.Draw("ap");
//}

void secondPass(){
	TreeReader reader;
	reader.openSeries(256,"S");
	Int_t i=1,r0,r1;
	Long64_t t0,t1;
	reader.tree->GetEntry(0);
	r0=reader.entry.nrun;
	t0=150*reader.entry.timestamp[0];
	reader.tree->GetEntry(i);
	r1=reader.entry.nrun;
	t1=150*reader.entry.timestamp[1];
	while(r0==r1){
		if(t1-t0<bitError && t0-t1<bitError){
			g.SetPoint(i-1,i-1,t1-t0);
		}else{
			g.SetPoint(i-1,i-1,0);
		}
		reader.tree->GetEntry(++i);
		r0=r1;
		t0=t1;
		r1=reader.entry.nrun;
		t1=150*reader.entry.timestamp[0];
	}
}