//bapdPlotter.cpp

#include <iomanip>
#include <fstream>
using namespace std;

TH1 **on_peak=NULL,**off_peak=NULL,**bgc=NULL;
int bApdCh[3]={2,6,13};
double PEAK=-26,ONPEAK=20,OFFPEAK=320;
double MAX=20,MIN=0.25;
char *titles[4]={"02","06","16","total"};
Long64_t nep=0;

void bapdPlotter(){
	makeHists();
	TFile file(Form("%s/cal.root",RDKANAL));
	CalibrationList *list=(CalibrationList*)file.Get("CalibrationList");
	TreeReader reader;
	reader.reset(kTRUE);
	NData ndata=reader.openList(Form("%s/runlist.csv",RDKANAL));
	//NData ndata=reader.openSeries(256,"S");
	//NData ndata=reader.openSeries(TreeReader::n1400,TreeReader::all1400,"S");
	logNdat(ndata);
	nep=0;
	for(Long64_t idx=0;idx<reader.tree->GetEntries();idx++){
		reader.tree->GetEntry(idx);
		CalibrationData *cal=list->getCal(reader.entry);
		cal->calibrate(reader.recon);
		if(reader.recon.t_p-reader.recon.t_e<650){
			nep++;
			for(int j=0;j<3;j++){
				int ch=bApdCh[j];
				if(reader.recon.t_gamma[ch]>0 && reader.recon.t_gamma[ch]<NPOINTS
						&& reader.recon.E_gamma[ch]>MIN && reader.recon.E_gamma[ch]<MAX){
					double delta_t=reader.recon.t_gamma[ch]-reader.recon.t_e-PEAK;
					if(delta_t<=ONPEAK && delta_t>-ONPEAK){
					//if(TMath::Abs(delta_t)<=ONPEAK){
						on_peak[j]->Fill(reader.recon.E_gamma[ch],4);
						on_peak[3]->Fill(reader.recon.E_gamma[ch],4);
					//}else if(TMath::Abs(delta_t<=OFFPEAK)){
					}else if(delta_t<=OFFPEAK && delta_t>-OFFPEAK){
						off_peak[j]->Fill(reader.recon.E_gamma[ch],4);
						off_peak[3]->Fill(reader.recon.E_gamma[ch],4);
					}
				}
			}
		}
	}
	double w=ONPEAK/(ONPEAK-OFFPEAK);
	//double w=(ONPEAK+0.5)/(ONPEAK-OFFPEAK);
	for(int j=0;j<4;j++){
		bgc[j]->Add(on_peak[j],off_peak[j],1,w);
	}
	savePlots();
	cout<<"nep: "<<nep<<endl;
	gSystem->ChangeDirectory(WORKDIR);
}

void makeHists(){
	if(!on_peak){
		on_peak=new TH1F[4];
		for(int i=0;i<4;i++){
			TString name=Form("E_gamma bAPD %s",titles[i]);
			on_peak[i]=new TH1F(Form("on_peak-%s",titles[i]),name,4*MAX,0,MAX);
			on_peak[i]->Sumw2();
			on_peak[i]->SetMarkerStyle(kFullCircle);
		}
	}
	if(!off_peak){
		off_peak=new TH1F[4];
		for(int i=0;i<4;i++){
			TString name=Form("E_gamma bAPD %s",titles[i]);
			off_peak[i]=new TH1F(Form("off_peak-%s",titles[i]),name,4*MAX,0,MAX);
			off_peak[i]->Sumw2();
			off_peak[i]->SetMarkerStyle(kFullCircle);
		}
	}
	if(!bgc){
		bgc=new TH1F[4];
		for(int i=0;i<4;i++){
			TString name=Form("E_gamma bAPD %s",titles[i]);
			bgc[i]=new TH1F(Form("bgc-%s",titles[i]),name,4*MAX,0,MAX);
			bgc[i]->Sumw2();
			bgc[i]->SetMarkerStyle(kFullCircle);
		}
	}
	for(int i=0;i<4;i++){
		on_peak[i]->Reset("M");
		off_peak[i]->Reset("M");
		bgc[i]->Reset("M");
	}
}

void savePlots(){
	TCanvas *c1=new TCanvas();
	gStyle->SetErrorX(0);
	for(int i=0;i<4;i++){
		on_peak[i]->Draw("E1");
		c1->SaveAs(Form("%s/Temp/bapd/on_peak/E_gamma_%s.png",RDKHOME,titles[i]));
		off_peak[i]->Draw("E1");
		c1->SaveAs(Form("%s/Temp/bapd/off_peak/E_gamma_%s.png",RDKHOME,titles[i]));
		bgc[i]->Draw("E1");
		c1->SaveAs(Form("%s/Temp/bapd/bg_corrected/E_gamma_%s.png",RDKHOME,titles[i]));
	}
	c1->Close();
	ofstream out(Form("%s/Temp/bapd/bg_corrected/bgc_data.csv",RDKHOME));
	Quantity results[4];
	out<<"Bin center (keV),det02,err02,det06,err06,det16,err16,Total,Uncertainty"<<endl;
	for(int i=1;i<=bgc[3]->GetNbinsX();i++){
		out<<bgc[3]->GetBinCenter(i);
		for(int j=0;j<4;j++){
			Quantity q(bgc[j]->GetBinContent(i),bgc[j]->GetBinError(i));
			q*=bgc[j]->GetBinWidth(i);
			out<<","<<q.getValue()<<","<<q.getUncertainty();
			results[j]+=q;
		}
		out<<endl;
	}
	out<<"total";
	for(int j=0;j<4;j++){
		out<<","<<results[j].getValue()<<","<<results[j].getUncertainty();
	}
	out<<endl;
	out<<"ep,"<<nep<<flush;
}