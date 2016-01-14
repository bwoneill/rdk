//bgoPlots.cpp

#include <iomanip>
#include <fstream>
#include <utility>
using namespace std;

using namespace TMath;

TH1F **cal_hist=NULL,**delta_t_bgo=NULL,**on_peak_bgo=NULL,**off_peak_bgo=NULL;
TH1F **bgc_bgo=NULL,*cal=NULL,*sigma=NULL;
TGraphErrors *epg_plot=NULL,*epg_pos=NULL;
TF1 *bg_gaus=new TF1("background+gaus","expo+gaus(2)",8500,24000);
bool single=true;
int smoothPar=0,res=32;
double bgoAngle[11]={345,315,255,225,195,165,135,105,75,45,15},ep=0,epg=0;
const int bgoCh[11]={0,1,3,4,5,7,8,9,10,11,12};
const int delta_t_min=-50,delta_t_max=50,t_min=-50,t_max=50;
const int t_bg_min=-250,t_bg_max=550,delta_t_p=650;
const double max=950,min=9.5,bin_size=5;
const char *bgoTitles[12]={"11","12","14","15","16","21","22","23","24","25","26","sum"};
TCanvas *calfit=NULL;
CalibrationList *list=new CalibrationList(
	Form("%s\\calibration\\TCfit_511keV_calibration_21Feb11.dat",RDKANAL));
CalibrationData *calibration=NULL;

void initBgoHist(){
	if(on_peak_bgo){
		for(int i=0;i<12;i++){
			on_peak_bgo[i]->Reset("M");
		}
	}else{
		on_peak_bgo=new TH1F[12];
		for(int i=0;i<12;i++){
			on_peak_bgo[i]=new TH1F(Form("on_peak_bgo_%s",bgoTitles[i]),
				Form("On peak BGO %s;keV",bgoTitles[i]),max/bin_size,0,max);
			on_peak_bgo[i]->Sumw2();
		}
	}
	if(off_peak_bgo){
		for(int i=0;i<12;i++){
			off_peak_bgo[i]->Reset("M");
		}
	}else{
		off_peak_bgo=new TH1F[12];
		for(int i=0;i<12;i++){
			off_peak_bgo[i]=new TH1F(Form("off_peak_bgo_%s",bgoTitles[i]),
				Form("Off peak BGO %s;keV",bgoTitles[i]),max/bin_size,0,max);
			off_peak_bgo[i]->Sumw2();
		}
	}
	if(bgc_bgo){
		for(int i=0;i<12;i++){
			bgc_bgo[i]->Reset("M");
		}
	}else{
		bgc_bgo=new TH1F[12];
		for(int i=0;i<12;i++){
			bgc_bgo[i]=new TH1F(Form("bgc_bgo_%s",bgoTitles[i]),
				Form("Background corrected BGO %s;keV",bgoTitles[i]),max/bin_size,0,max);
			bgc_bgo[i]->Sumw2();
		}
	}
	if(delta_t_bgo){
		for(int i=0;i<12;i++){
			delta_t_bgo[i]->Reset("M");
		}
	}else{
		delta_t_bgo=new TH1F[12];
		for(int i=0;i<12;i++){
			delta_t_bgo[i]=new TH1F(Form("delta_t_bgo_%s",bgoTitles[i]),
				Form("BGO %s;#Delta_{t} (#mus)",bgoTitles[i]),NPOINTS*3/4,
				-NPOINTS/4*0.04-0.02,NPOINTS/2*0.04-0.02);
		}
	}
}

void initCalHist(){
	if(cal){
		cal->Reset("M");
	}else{
		cal=new TH1F("cal_scale","Calibration",200,32*0.9,32*1.1);
	}
	if(sigma){
		sigma->Reset("M");
	}else{
		sigma=new TH1F("cal_sigma","Sigma",200,0,200);
	}
	int cal_bins=int(Analyzer::MAX)/res,cal_max=cal_bins*res;
	if(cal_hist){
		for(int i=0;i<11;i++){
			cal_hist[i]->Reset();
		}
	}else{
		cal_hist=new TH1F[11];
		for(int i=0;i<11;i++){
			cal_hist[i]=new TH1F(Form("cal_hist_%s",bgoTitles[i]),bgoTitles[i],
				cal_bins,0,cal_max);
			cal_hist[i]->Sumw2();
			cal_hist[i]->SetLineColor(kRed);
			cal_hist[i]->SetMarkerColor(kRed);
			cal_hist[i]->SetMaximum(50);
			cal_hist[i]->SetMinimum(0);
		}
	}
	if(calfit){
		calfit->Close();
		delete calfit;
	}
	calfit=new TCanvas("calfit","calibrations",1024,768);
	gStyle->SetOptFit(1);
	gStyle->SetOptStat(0);
	bg_gaus->SetParLimits(0,0,20);
	bg_gaus->SetParLimits(1,-1e-3,0);
	bg_gaus->SetParLimits(2,0,20);
	bg_gaus->SetParLimits(3,10000,22000);
	bg_gaus->SetParLimits(4,500,2000);
	//bg_gaus->SetParLimits(3,14717,17987);
}

void bgoSpectra(){
	initBgoHist();
	ep=0;
	TreeReader reader;
	reader.openList(Form("%s/runlist.csv",RDKANAL));
	for(Long64_t j=0;j<reader.tree->GetEntries();j++){
		//cout<<"Getting entry "<<j<<endl;
		reader.tree->GetEntry(j);
		//cout<<"Checking if ep event"<<endl;
		if(ep_event(reader.recon)){
			ep++;
			//cout<<"Getting calibration"<<endl;
			reader.recon=list->calibrate(reader.entry,reader.recon);
			//cout<<"Checking multiplicity"<<endl;
			if(single){//Single multiplicity
				int i=getEvent(reader.recon);
				if(i!=-1){
					int CH=bgoCh[i];
					delta_t_bgo[i]->Fill((reader.recon.t_gamma[CH]-reader.recon.t_e[CH])*0.04);
					if(on_peak_window(CH,reader.recon)){
						on_peak_bgo[i]->Fill(reader.recon.E_gamma[CH]);
					}else if(off_peak_window(CH,reader.recon)){
						off_peak_bgo[i]->Fill(reader.recon.E_gamma[CH]);
					}
				}
			}else{//All events
				for(int i=0;i<11;i++){
					int CH=bgoCh[i];
					if(bgo_event(CH,reader.recon)){
						delta_t_bgo[i]->Fill((reader.recon.t_gamma[CH]-reader.recon.t_e[CH])*0.04);
						if(on_peak_window(CH,reader.recon)){
							on_peak_bgo[i]->Fill(reader.recon.E_gamma[CH]);
						}else if(off_peak_window(CH,reader.recon)){
							off_peak_bgo[i]->Fill(reader.recon.E_gamma[CH]);
						}
					}
				}
			}
		}
	}
	double scale=(t_max-t_min)/double(t_bg_max-t_bg_min-t_max+t_min);
	for(int i=0;i<11;i++){
		delta_t_bgo[11]->Add(delta_t_bgo[i]);
		bgc_bgo[i]->Add(on_peak_bgo[i],off_peak_bgo[i],1,-scale);
		on_peak_bgo[11]->Add(on_peak_bgo[i]);
		off_peak_bgo[11]->Add(off_peak_bgo[i]);
		bgc_bgo[11]->Add(bgc_bgo[i]);
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

int getEvent(const Recon &recon,int board=-1){
	int value=-1,min,max;
	if(board==-1){
		min=0;max=11;
	}else if(board==0){
		min=0;max=5;
	}else if(board==1){
		min=5;max=11;
	}
	for(int i=min;i<max;i++){
		if(bgo_event(bgoCh[i],recon)){
			if(value!=-1){
				return -1;
			}else{
				value=i;
			}
		}
	}
	return value;
}

void epg_over_ep(){
	double *xaxis=new double[12],*value=new double[12],*error=new double[12];
	if(delta_t_bgo!=NULL){
		for(int i=0;i<12;i++){
			xaxis[i]=i+1;
			double sigma_on,sigma_off,on_peak,off_peak,scale;
			on_peak=delta_t_bgo[i]->IntegralAndError(513+t_min,512+t_max,sigma_on);
			off_peak=delta_t_bgo[i]->IntegralAndError(513+t_bg_min,512+t_bg_max,sigma_off);
			scale=(double)(t_max-t_min)/(t_bg_max-t_bg_min-t_max+t_min);
			double sigma_epg=sqrt((1-pow(scale,2))*pow(sigma_on,2)+pow(scale*sigma_off,2));
			epg=on_peak-scale*(off_peak-on_peak);
			double sigma_epg_over_ep=epg/ep*sqrt(pow(sigma_epg/epg,2)+1/epg);
			value[i]=epg/ep/(i==11?11:1);
			error[i]=sigma_epg_over_ep/(i==11?11:1);
			cout<<"detector "<<bgoTitles[i]<<" epg/ep="<<value[i]<<"+/-"<<error[i]<<endl;
		}
		if(epg_plot){
			epg_plot->Delete();
		}
		if(epg_pos){
			epg_pos->Delete();
		}
		epg_plot=new TGraphErrors(12,xaxis,value,0,error);
		epg_pos=new TGraphErrors(11,bgoAngle,value,0,error);
		TAxis *axis=epg_plot->GetXaxis();
		for(int i=0;i<12;i++){
			axis->SetBinLabel(axis->FindBin(i+1),bgoTitles[i]);
		}
		epg_plot->SetMarkerStyle(7);
		epg_pos->SetMarkerStyle(7);
	}else{
		bgoSpectra();
		if(delta_t_bgo==NULL){
			cout<<"Error running bgoDeltaT"<<endl;
			return;
		}
		epg_over_ep();
	}
}

void savePlots(){
	if(delta_t_bgo==NULL || on_peak_bgo==NULL || off_peak_bgo==NULL || bgc_bgo==NULL){
		cout<<"No plots"<<endl;
		return;
	}
	gStyle->SetErrorX(0);
	TCanvas c1("c1","",1024,768);
	for(int i=0;i<12;i++){
		delta_t_bgo[i]->Draw();
		c1.SaveAs(Form("%s\\Temp\\bgo\\%s.png",RDKHOME,delta_t_bgo[i]->GetName()));
		on_peak_bgo[i]->Draw("e1");
		c1.SaveAs(Form("%s\\Temp\\bgo\\%s.png",RDKHOME,on_peak_bgo[i]->GetName()));
		off_peak_bgo[i]->Draw("e1");
		c1.SaveAs(Form("%s\\Temp\\bgo\\%s.png",RDKHOME,off_peak_bgo[i]->GetName()));
		bgc_bgo[i]->Draw("e1");
		c1.SaveAs(Form("%s\\Temp\\bgo\\%s.png",RDKHOME,bgc_bgo[i]->GetName()));
	}
}

void calibrateBgo(){
	calibrateBgo(nall,allSeries);
}

void calibrateBgo(int series,int run){
	initCalHist();
	ofstream out(Form("%s/cal.txt",RDKHOME));
	out<<"series\t";
	for(int i=0;i<11;i++){
		out<<"pk"<<bgoTitles[i]<<"\t";
	}
	out<<endl;
	resetTrees();
	openFile(series,run);
	linkTree();
	fitHist(out);
	drawCalHist();
	out.close();
}

void calibrateBgo(int length,const int *list){
	initCalHist();
	ofstream out(Form("%s/cal.txt",RDKHOME));
	out<<"series\t";
	for(int i=0;i<11;i++){
		out<<"pk"<<bgoTitles[i]<<"\t";
	}
	out<<endl;
	for(int i=0;i<length;i++){
		resetTrees();
		openSeries(list[i],"C");
		linkTree();
		fitHist(out);
	}
	drawCalHist();
	out.close();
}

void fitHist(ofstream &out){
	//if(tree->GetEntries()>0){
	//	for(int j=0;j<11;j++){
	//		cal_hist[j]->Reset();
	//	}
	//	for(Long64_t index=0;index<tree->GetEntries();index++){
	//		tree->GetEntry(index);
	//		for(int j=0;j<2;j++){
	//			int CH=getEvent(j);
	//			if(CH>=0){
	//				cal_hist[CH]->Fill(recon.E_gamma[bgoCh[CH]]);
	//			}
	//		}
	//	}
	//	out<<list[i]<<"\t";
	//	for(int j=0;j<11;j++){
	//		if(cal_hist[j]->GetEntries()>0){
	//			cal_hist[j]->Smooth(smoothPar);
	//			bg_gaus->SetParameters(4,-1.5e-4,5,16352,1000);
	//			cal_hist[j]->Fit(bg_gaus,"QRBM");
	//			double peak=bg_gaus->GetParameter(3),scale=bg_gaus->GetParameter(3)/511;
	//			cal->Fill(scale,tree->GetEntries());
	//			sigma->Fill(bg_gaus->GetParameter(4),tree->GetEntries());
	//			out<<peak<<"\t";
	//		}else{
	//			out<<"--\t";
	//		}
	//	}
	//	out<<endl;
	//}
}

void drawCalHist(){
	calfit->Clear();
	calfit->Divide(4,3);
	for(int i=0;i<11;i++){
		calfit->cd(i+1);
		cal_hist[i]->Draw();
	}
	calfit->cd(0);
}