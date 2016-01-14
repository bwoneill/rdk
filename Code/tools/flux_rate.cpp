//flux_rate.cpp

void flux_rate(){
	const Char_t *temp="/mnt/data/rdk2/analysis/bwoneill/second_pass/ss%i/S%i.root";
	Int_t series[13]={253,257,270,289,290,291,292,304,305,311,313,314,317};
	Int_t bapd[3]={2,6,13};
	TString file,header="Series,EP,\"onpeak 02\",\"offpeak 02\",\"bgc 02\","
		"\"onpeak 06\",\"offpeak 06\",\"bgc 06\","
		"\"onpeak 16\",\"offpeak 16\",\"bgc 16\"";
	Double_t scale=(Plotter2::windows[1][2]-Plotter2::windows[1][1])/
		(Plotter2::windows[1][3]-Plotter2::windows[1][2]+Plotter2::windows[1][1]-
		Plotter2::windows[1][0]);
	ofstream out[2];
	out[0].open("flux_rate_0.5_20.csv");
	out[1].open("flux_rate_1.0_20.csv");
	out[0]<<header<<endl;
	out[1]<<header<<endl;
	for(Int_t i=0;i<13;i++){
		Quantity ep,epg[2][3],onpeak[2][3],offpeak[2][3];
		file.Form(temp,series[i],series[i]);
		TFile in(file);
		TTree *tree=(TTree*)in.Get("rdk_second_pass");
		epEvent event;
		tree->SetBranchAddress("epEvent",(Double_t*)&event);
		for(Int_t j=0;j<tree->GetEntries();j++){
			tree->GetEntry(j);
			Double_t tof=event.t_p-event.t_e;
			Bool_t result=event.timestamp[0]>0 && event.timestamp[1]>0
					&& tof>Plotter2::tofmin && tof<Plotter2::tofmax
					&& event.E_e>Plotter2::emin && event.E_e<Plotter2::emax
					&& event.E_p>Plotter2::pmin && event.E_p<Plotter2::pmax;
			if(result){
				Int_t set=-1;
				if(i<9){
					set=0;
				}else{
					set=4;
				}
				if(set>=0){
					result &= event.chi2_ep<Plotter2::p0[set]+Plotter2::p2[set]*
						event.E_e*event.E_e;
				}else{
					result=kFALSE;
				}
			}
			if(result){
				ep+=1;
				for(Int_t k=0;k<3;k++){
					tof=event.t_gamma[bapd[k]]-event.t_e;
					Double_t E=event.E_gamma[bapd[k]];
					if(tof>Plotter2::windows[1][1] && tof<Plotter2::windows[1][2]){
						if(E>0.5 && E<20){
							onpeak[0][k]+=1;
						}
						if(E>1 && E<20){
							onpeak[1][k]+=1;
						}
					}else if(tof>Plotter2::windows[1][0] && tof<Plotter2::windows[1][3]){
						if(E>0.5 && E<20){
							offpeak[0][k]+=1;
						}
						if(E>1 && E<20){
							offpeak[1][k]+=1;
						}
					}
				}
			}
		}
		for(Int_t j=0;j<2;j++){
			for(Int_t k=0;k<3;k++){
				epg[j][k]=onpeak[j][k]-scale*offpeak[j][k];
			}
		}
		for(Int_t j=0;j<2;j++){
			out[j]<<series[i]<<","<<ep.getValue();
			for(Int_t k=0;k<3;k++){
				out[j]<<","<<onpeak[j][k].getValue()<<","<<offpeak[j][k].getValue()
					<<","<<epg[j][k].getValue();
			}
			out[j]<<endl;
		}
	}
	out[0].close();
	out[1].close();
}