//exportHistograms.cpp

void exportHistograms(){
	TreeReader reader;
	reader.openSeries(257,"D");
	reader.openSeries(258,"D");
	reader.openSeries(260,"D");
	reader.openSeries(261,"D");
	reader.openSeries(262,"D");
	TH1F hist1("hist1","hist1",100,0,30000);
	TH1F hist2("hist2","hist2",100,0,30000);
	TH1F hist3("hist3","hist3",100,0,30000);
	for(Long64_t i=0;i<reader.tree->GetEntries();i++){
		reader.tree->GetEntry(i);
		if(fabs(reader.recon.t_gamma[2]-600)<10){
			hist1.Fill(reader.recon.E_gamma[2]);
		}
		if(fabs(reader.recon.t_gamma[6]-600)<10){
			hist2.Fill(reader.recon.E_gamma[6]);
		}
		if(fabs(reader.recon.t_gamma[13]-600)<10){
			hist3.Fill(reader.recon.E_gamma[13]);
		}
	}
	ofstream output(Form("%s/output.csv",RDKANAL));
	output<<"bin center,det02,det06,det16"<<endl;
	for(Long64_t i=1;i<=100;i++){
		output<<hist1.GetBinCenter(i)<<","<<hist1.GetBinContent(i)<<",";
		output<<hist2.GetBinContent(i)<<","<<hist3.GetBinContent(i)<<endl;
	}
	output.close();
}