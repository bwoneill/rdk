//testFit.cpp

void testFit(Double_t offset,Double_t amp,Double_t decay){
	TRandom *rand=new TRandom();
	TH1F *hist=new TH1F("hist","Test histogram",100,0,20);
	for(Int_t i=0;i<100;i++){
		hist->SetBinContent(i+1,offset+amp*TMath::Exp(-decay*i/4)+amp/5*(rand->Rndm()-0.5));
	}
	TF1 *f1=new TF1("func","[0]+[1]*exp(-[2]*x)",0,20);
	f1->SetParameters(offset,amp,decay);
	hist->Fit(f1);
	hist->Draw();
}