//bgoPlotter.cpp

vector<TH1F*> delta_t_bgo,on_peak_bgo,off_peak_bgo,bgc_bgo;

const int delta_t_min=-50,delta_t_max=50,t_min=-50,t_max=50;
const int t_bg_min=-250,t_bg_max=550,delta_t_p=650;
const double max=950,min=9.5,bin_size=5;

void bgoPlotter(){
	CalibrationList list(Form("%s/calibration/TCfit_511keV_calibration_21Feb11.dat",RDKANAL));
	TreeReader reader;
	reader.openList(Form("%s/runlist.csv",RDKANAL));
	initBgoHist();
	//for(Long64_t j=0;j<reader.tree->GetEntries();j++){
	//	reader.tree->GetEntry(j);
	//}
}

void initBgoHist(){
	if(on_peak_bgo.size()){
		for(int i=0;i<12;i++){
			on_peak_bgo[i]->Reset("M");
		}
	}else{
		for(int i=0;i<12;i++){
			on_peak_bgo.push(new TH1F(Form("on_peak_bgo_%s",bgoTitles[i]),
				Form("On peak BGO %s;keV",bgoTitles[i]),(max/32)/bin_size,0,max/32));
			on_peak_bgo[i]->Sumw2();
		}
	}
	if(off_peak_bgo.size()){
		for(int i=0;i<12;i++){
			off_peak_bgo[i]->Reset("M");
		}
	}else{
		for(int i=0;i<12;i++){
			off_peak_bgo.push(new TH1F(Form("off_peak_bgo_%s",bgoTitles[i]),
				Form("Off peak BGO %s;keV",bgoTitles[i]),(max/32)/bin_size,0,max/32));
			off_peak_bgo[i]->Sumw2();
		}
	}
	if(bgc_bgo.size()){
		for(int i=0;i<12;i++){
			bgc_bgo[i]->Reset("M");
		}
	}else{
		for(int i=0;i<12;i++){
			bgc_bgo.push(new TH1F(Form("bgc_bgo_%s",bgoTitles[i]),
				Form("Background corrected BGO %s;keV",bgoTitles[i]),(max/32)/bin_size,0,max/32));
			bgc_bgo[i]->Sumw2();
		}
	}
	if(delta_t_bgo.size()){
		for(int i=0;i<12;i++){
			delta_t_bgo[i]->Reset("M");
		}
	}else{
		for(int i=0;i<12;i++){
			delta_t_bgo.push(new TH1F(Form("delta_t_bgo_%s",bgoTitles[i]),
				Form("BGO %s;#Delta_{t} (#mus)",bgoTitles[i]),NPOINTS*3/4,
				-NPOINTS/4*0.04-0.02,NPOINTS/2*0.04-0.02));
		}
	}
}

bool ep_event(const Recon &recon){
	return recon.t_p-recon.t_e<delta_t_p;
}

bool bgo_event(int CH,const Recon &recon){
	return recon.t_gamma[CH]>0 && recon.t_gamma[CH]<NPOINTS && 
		recon.E_gamma[CH]>min && recon.E_gamma[CH]<max;
}

bool on_peak_window(int CH,const Recon &recon){
	return recon.t_gamma[CH]-recon.t_e[CH]>=t_min && recon.t_gamma[CH]-recon.t_e[CH]<t_max;
}

bool off_peak_window(int CH,const Recon &recon){
	return recon.t_gamma[CH]-recon.t_e[CH]>=t_bg_min &&
		recon.t_gamma[CH]-recon.t_e[CH]<t_bg_max;
}

int getEvent(const Recon &recon){
	int value=-1,min,max;
	for(int i=0;i<14;i++){
		if(i!=2 && i!=6 && i!=13 && bgo_event(i,recon)){
			if(value!=-1){
				return -1;
			}else{
				value=i;
			}
		}
	}
	return value;
}