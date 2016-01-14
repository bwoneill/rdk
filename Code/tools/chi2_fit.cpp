//chi2_fit.cpp

TString filter="timestamp[0]>0 && timestamp[1]>0 && E_e>50 && "
		"E_e<800 && E_p>10 && E_p<31 && t_p-t_e>2 && t_p-t_e<10";

TH1* chi2_fit(TTree *tree){
	TH2F *hist=new TH2F("hist","E_{e} vs #chi^{2};E_{e} (keV);#chi^{2} (keV^2)",
		800,0,800,500,0,80000);
	TString fill;
	tree->Draw("chi2_ep:E_e>>hist",filter,"goff");
	TF1 f1("f1","[0]+[1]*x^2",0,800);
	hist->Fit(&f1,"N","goff");
	TH1F *hist2=new TH1F("res","Residuals from fit;#chi^{2} (keV^{2})",1000,-20000,40000);
	fill.Form("chi2_ep-%f-%f*E_e*E_e>>res",f1.GetParameter(0),f1.GetParameter(1));
	tree->Draw(fill,filter,"goff");
	hist2->Fit("gaus","N+","goff");
	delete hist;
	return hist2;
}

TH1* chi2_fit_2d(TTree *tree){
	TH2F *hist=new TH2F("hist","E_{e} vs #chi^{2};E_{e} (keV);#chi^{2} (keV^2)",
		800,0,800,500,0,80000);
	TString fill;
	tree->Draw("chi2_ep:E_e>>hist",filter,"goff");
	TF2 f1("f1","([0]+[1]*x^2)*exp(-(y-[2])^2/[3])",0,800,0,80000);
	f1.SetParameters(4e8,1400,0,1000);
	hist->Fit(&f1,"N+","goff");
	return hist;
}

TH1* chi2_plot(TTree *tree,TF1 *f,Int_t set){
	f->SetParameters(Plotter2::p0[set],Plotter2::p2[set]);
	f->SetLineColor(kRed);
	TString name,fill;
	name.Form("hist_set_%i",set);
	TH2F *hist=new TH2F(name,"E_{e} vs #chi^{2};E_{e} (keV);#chi^{2} (keV^2)",
		800,0,800,500,0,80000);
	fill.Form("chi2_ep:E_e>>%s",name.Data());
	tree->Draw(fill,filter,"goff");
	return hist;
}