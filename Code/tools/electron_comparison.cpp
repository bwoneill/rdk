//electron_comparison.cpp

TH1F **hist=new TH1F*[7];
TLegend *legend;

void electron_comparison(){
	Int_t series[7]={311,313,314,317,318,319,321};
	EColor color[7]={kRed,kBlue,kGreen,kCyan,kMagenta,kOrange,kBlack}
	TFile *file;
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	TString path;
	legend=new TLegend(0.7,0.7,1,1);
	for(Int_t i=0;i<7;i++){
		Int_t s=series[i];
		hist[i]=new TH1F(Form("hist%i",s),"E_e;E_e (keV)",100,0,800);
		hist[i]->SetLineColor(color[i]);
		hist[i]->Sumw2();
		path=path.Format(temp,s,s);
		file=TFile::Open(path);
		TTree *tree=(TTree*)file->Get("rdk_second_pass");
		epEvent event;
		tree->SetBranchAddress("epEvent",(Double_t*)&event);
		Double_t count=0;
		for(Int_t j=0;j<tree->GetEntries();j++){
			tree->GetEntry(j);
			Double_t E_e=event.E_e,E_p=event.E_p,delta_t=(event.t_p-event.t_e)*0.04;
			if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && delta_t>2 && delta_t<25){
				count++;
				hist[i]->Fill(E_e);
			}
		}
		hist[i]->Scale(1/count);
		legend->AddEntry(hist[i],Form("S%i",s));
		file->Close();
	}
	hist[4]->Draw();
	for(Int_t i=0;i<7;i++){
		hist[i]->Draw("same");
	}
	legend->Draw("same");
}