//plot_each_series.cpp

Int_t series[62]={76,77,78,79,87,89,91,93,94,95,104,105,106,108,119,120,126,127,151,153,
	154,155,156,160,161,162,165,166,175,176,185,187,213,214,215,223,227,228,253,256,257,
	258,260,261,262,263,265,267,270,289,290,291,292,304,305,311,313,314,317,318,319,321};
TH1F **hist=new TH1F*[62];

void plot_each_series(){
	TCanvas tc1("tc1","tc1",1920,1080);
	for(Int_t i=0;i<62;i++){
		Int_t s=series[i];
		TString name=Form("hist-S%i",s),title=Form("S%i;E_e (keV)",s);
		hist[i]=new TH1F(name,title,100,0,800);
		hist[i]->Sumw2();
		TString path=Form("/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root",s,s);
		TFile *f=new TFile(path);
		TTree *tree=(TTree*)f->Get("rdk_second_pass");
		epEvent event;
		tree->SetBranchAddress("epEvent",(Double_t*)&event);
		Int_t count=0;
		for(Int_t j=0;j<tree->GetEntries();j++){
			tree->GetEntry(j);
			Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
			if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
				hist[i]->Fill(E_e);
				count++;
			}
		}
		hist[i]->Scale(1./count);
		hist[i]->SetMaximum(0.03);
		hist[i]->SetMinimum(0);
		f->Close();
		hist[i]->Draw("colz");
		tc1.SaveAs(Form("../Plots/ep_fit/20130401/E_e_S%03i.png",s));
	}
}

//TH2F **hist=new TH2F*[62];
//
//void plot_each_series(){
//	TCanvas tc1("tc1","tc1",1920,1080);
//	for(Int_t i=0;i<62;i++){
//		Int_t s=series[i];
//		TString name=Form("hist-S%i",s),title=Form("S%i;E_e (keV);#chi^{2} (keV^{2})",s);
//		hist[i]=new TH2F(name,title,800,0,800,800,0,1e5);
//		TString path=Form("/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root",s,s);
//		TFile *f=new TFile(path);
//		TTree *tree=(TTree*)f->Get("rdk_second_pass");
//		epEvent event;
//		tree->SetBranchAddress("epEvent",(Double_t*)&event);
//		for(Int_t j=0;j<tree->GetEntries();j++){
//			tree->GetEntry(j);
//			Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
//			if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
//				hist[i]->Fill(E_e,chi2);
//			}
//		}
//		f->Close();
//		hist[i]->Draw("colz");
//		tc1.SaveAs(Form("../Plots/ep_fit/20130401/chi2_E_e_S%03i.png",s));
//	}
//}