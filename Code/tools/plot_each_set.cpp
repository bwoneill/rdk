//plot_each_set.cpp

#include "tools/openAllTrees.cpp"

void plot_each_set(){
	TCanvas tc1("tc1","tc1",1920,1080);
	TH2F hist1("hist1","Set 1;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	TH2F hist2("hist2","Set 2;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	TH2F hist2a("hist2a","Set 2a;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	TH2F hist2b("hist2b","Set 2b;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	TH2F hist3("hist3","Set 3;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	TH2F hist4("hist4","Set 4;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	TH2F hist4a("hist4a","Set 4a;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	TH2F hist4b("hist4b","Set 4b;E_{e} (keV);#chi^{2}_{ep} (keV^{2})",800,0,800,500,0,1e5);
	epEvent event;
	openSet1();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist1.Fill(E_e,chi2);
		}
	}
	openSet2();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist2.Fill(E_e,chi2);
		}
	}
	openSet2a();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist2a.Fill(E_e,chi2);
		}
	}
	openSet2b();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist2b.Fill(E_e,chi2);
		}
	}
	openSet3();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist3.Fill(E_e,chi2);
		}
	}
	openSet4();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist4.Fill(E_e,chi2);
		}
	}
	openSet4a();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist4a.Fill(E_e,chi2);
		}
	}
	openSet4b();
	chain->SetBranchAddress("epEvent",(Double_t*)&event);
	for(Int_t j=0;j<chain->GetEntries();j++){
		chain->GetEntry(j);
		Double_t E_e=event.E_e,E_p=event.E_p,chi2=event.chi2_ep,dt=(event.t_p-event.t_e)*0.04;
		if(E_e>50 && E_e<800 && E_p>7 && E_p<31 && dt>2 && dt<25){
			hist4b.Fill(E_e,chi2);
		}
	}
	hist1.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set1.png");
	hist2.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set2.png");
	hist2a.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set2a.png");
	hist2b.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set2b.png");
	hist3.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set3.png");
	hist4.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set4.png");
	hist4a.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set4a.png");
	hist4b.Draw("colz");
	tc1.SaveAs("../Plots/ep_fit/20130408/E_e_v_chi2_set4b.png");
}