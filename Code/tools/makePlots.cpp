//makePlots.cpp

#include <fstream>
using namespace std;

Plotter *plotter=Plotter::getPlotter();
TreeReader *reader=new TreeReader();
TFile f(Form("%s/cal.root",RDKANAL));
CalibrationList *list;
TGraphErrors **graph=NULL;
TH1F **plots=NULL;

void makePlots(){
	ofstream log(Form("%s/calibration/rates.csv",RDKANAL));
	log<<"series,det00,err00,det01,err01,det02,err02,det03,err03,";
	log<<"det04,err04,det05,err05,det06,err06,det10,err10,det11,err11,";
	log<<"det12,err12,det13,err13,det14,err14,det15,err15,det16,err16"<<endl;
	list=(CalibrationList*)f.Get("CalibrationList");
	reader->openSeries(256,"CD");
	plotter->fill(reader,list,kTRUE);
	plotter->draw(Form("%s/calibration/spectra/c256%%s.png",RDKANAL));
	plotter->reset();
	reader->reset();
	reader->openSeries(260,"CD");
	plotter->fill(reader,list,kTRUE);
	plotter->draw(Form("%s/calibration/spectra/c260%%s.png",RDKANAL));
	plotter->reset();
	reader->reset();
	reader->openSeries(261,"CD");
	plotter->fill(reader,list,kTRUE);
	plotter->draw(Form("%s/calibration/spectra/c261%%s.png",RDKANAL));
	plotter->reset();
	reader->reset();
	if(graph){
		delete graph[];
	}
	graph=new TGraphErrors[16];
	for(int i=0;i<14;i++){
		graph[i]=new TGraphErrors(TreeReader::n1400+1);
		graph[i]->SetMarkerStyle(kFullDotSmall);
	}
	graph[14]=new TGraphErrors(14);
	graph[14]->SetMarkerStyle(kFullDotSmall);
	graph[15]=new TGraphErrors(14);
	graph[15]->SetMarkerStyle(kFullDotSmall);
	for(int i=0;i<TreeReader::n1400;i++){
		int series=TreeReader::all1400[i];
		reader->openSeries(series,"S");
		plotter->fill(reader,list);
		log<<series;
		for(int ch=0;ch<14;ch++){
			TH1 *onPeak=plotter->getOnPeak(ch),*offPeak=plotter->getOffPeak(ch);
			double temp;
			Quantity on(onPeak->IntegralAndError(1,onPeak->GetNbinsX(),temp),temp);
			Quantity off(offPeak->IntegralAndError(1,offPeak->GetNbinsX(),temp),temp);
			Quantity epg=on-off/7,ep(plotter->getEp());
			Quantity epg_over_ep=epg/ep;
			log<<","<<epg_over_ep[0]<<","<<epg_over_ep[1];
			graph[ch]->SetPoint(i,series,epg_over_ep[0]);
			graph[ch]->SetPointError(i,0,epg_over_ep[1]);
		}
		log<<endl;
		plotter->draw(Form("%s/calibration/spectra/s%i%%s.png",RDKANAL,series));
		plotter->reset();
		reader->reset();
	}
	reader->openSeries(TreeReader::n1400,TreeReader::all1400,"S");
	plotter->fill(reader,list);
	log<<"sum";
	for(int ch=0;ch<14;ch++){
		int series=TreeReader::all1400[TreeReader::n1400-1]+1;
		TH1 *onPeak=plotter->getOnPeak(ch),*offPeak=plotter->getOffPeak(ch);
		double temp;
		Quantity on(onPeak->IntegralAndError(1,onPeak->GetNbinsX(),temp),temp);
		Quantity off(offPeak->IntegralAndError(1,offPeak->GetNbinsX(),temp),temp);
		Quantity epg=on-off/7,ep(plotter->getEp());
		Quantity epg_over_ep=epg/ep;
		log<<","<<epg_over_ep[0]<<","<<epg_over_ep[1];
		graph[ch]->SetPoint(i,series,epg_over_ep[0]);
		graph[ch]->SetPointError(i,0,epg_over_ep[1]);
		graph[14]->SetPoint(ch,ch,epg_over_ep[0]);
		graph[14]->SetPointError(ch,0,epg_over_ep[1]);
		graph[15]->SetPoint(ch,ch,epg[0]);
		graph[15]->SetPointError(ch,0,epg[1]);
	}
	TCanvas c1;
	for(int ch=0;ch<14;ch++){
		graph[ch]->Draw("ap");
		c1.SaveAs(Form("%s/calibration/epg_over_ep_%i%i.png",RDKANAL,ch/7,ch%7));
	}
	graph[14]->Draw("ap");
	c1.SaveAs(Form("%s/calibration/epg_over_ep_sum.png",RDKANAL));
	log<<endl;
	plotter->draw(Form("%s/calibration/spectra/sum_%%s.png",RDKANAL));
	log.close();
}

void plotCal(){
	list=(CalibrationList*)f.Get("CalibrationList");
	if(graph){
		delete graph[];
	}
	graph=new TGraphErrors[14];
	for(int i=0;i<14;i++){
		graph[i]=new TGraphErrors(list->GetEntries());
		graph[i]->SetMarkerStyle(kFullDotSmall);
	}
	for(int in=0;in<list->GetEntries();in++){
		CalibrationData *data=(CalibrationData*)list->At(in);
		for(int i=0;i<14;i++){
			if(data->getSigma(i)/data->getPeak(i)<0.1){
				graph[i]->SetPoint(in,in,data->getPeak(i));
				graph[i]->SetPointError(in,0,data->getSigma(i));
			}
		}
	}
}

void plotEpgOverEp(){
	//bool j=CalibrationData::mode.TestBitNumber(0);
	list=(CalibrationList*)f.Get("CalibrationList");
	if(graph){
		delete graph[];
	}
	graph=new TGraphErrors[2];
	for(int i=0;i<2;i++){
		graph[i]=new TGraphErrors(14);
		graph[i]->SetMarkerStyle(kFullDotSmall);
	}
	reader->reset();
	reader->openSeries(TreeReader::n1400,TreeReader::all1400,"S");
	plotter->fill(reader,list);
	for(int ch=0;ch<14;ch++){
		//cout<<"Checking mode"<<endl;
		bool j=CalibrationData::mode.TestBitNumber(ch);
		//cout<<"Getting on peak weight"<<endl;
		float onWeight=Plotter::windows[j][2]-Plotter::windows[j][1];
		//cout<<"Getting off peak weight"<<endl;
		float offWeight=Plotter::windows[j][3]-Plotter::windows[j][0]-onWeight;
		TH1 *onPeak=plotter->getOnPeak(ch),*offPeak=plotter->getOffPeak(ch);
		double temp;
		Quantity on(onPeak->IntegralAndError(1,onPeak->GetNbinsX(),temp),temp);
		Quantity off(offPeak->IntegralAndError(1,offPeak->GetNbinsX(),temp),temp);
		Quantity epg=on-off*(onWeight/offWeight),ep(plotter->getEp());
		Quantity epg_over_ep=epg/ep;
		graph[0]->SetPoint(ch,ch,epg_over_ep[0]);
		graph[0]->SetPointError(ch,0,epg_over_ep[1]);
		graph[1]->SetPoint(ch,ch,epg[0]);
		graph[1]->SetPointError(ch,0,epg[1]);
	}
}

void plotDeltaTeg(){
	if(plots){
		for(int i=0;i<14;i++){
			plots[i]->Reset("M");
		}
	}else{
		plots=new TH1F*[14];
		for(int i=0;i<14;i++){
			TString name=Form("delta_t_%i%i",i/7,i%7);
			plots[i]=new TH1F(name,Form("%s;#mus",name.Data()),1700,-600*0.04,1100*0.04);
		}
	}
	reader->reset();
	reader->openSeries(TreeReader::n1400,TreeReader::all1400,"S");
	list=(CalibrationList*)f.Get("CalibrationList");
	for(int i=0;i<reader->tree->GetEntries();i++){
		reader->tree->GetEntry(i);
		list->getCal(reader->entry)->calibrate(reader->recon);
		for(int ch=0;ch<14;ch++){
			if(Plotter::epEvent(reader->recon) && Plotter::gEvent(reader->recon,ch)){
				double delta_t=reader->recon.t_gamma[ch]-reader->recon.t_e;
				//cout<<"Filling "<<ch<<": "<<i<<", "<<delta_t<<endl;
				plots[ch]->Fill(delta_t);
				//cout<<"Done"<<endl;
			}
		}
	}
}

void plotAllDeltaT(){
	double bounds[1701];
	for(int i=0;i<1701;i++){
		bounds[i]=-24+0.04*i-1e-4;
	}
	plots=new TH1F*[2];
	plots[0]=new TH1F("delta_t_ep","Delta_t E-P;#mus",500,0,20);
	plots[1]=new TH1F("delta_t_eg","Delta_t E-#gamma;#mus",1700,bounds);
	reader->reset();
	reader->openSeries(TreeReader::n1400,TreeReader::all1400,"S");
	list=(CalibrationList*)f.Get("CalibrationList");
	for(Long64_t index=0;index<reader->tree->GetEntries();index++){
		reader->tree->GetEntry(index);
		list->getCal(reader->entry)->calibrate(reader->recon);
		if(Plotter::epEvent(reader->recon)){
			double delta_t=(reader->recon.t_p-reader->recon.t_e)*0.04;
			plots[0]->Fill(delta_t);
			for(int ch=0;ch<14;ch++){
				if(Plotter::gEvent(reader->recon,ch)){
					delta_t=(reader->recon.t_gamma[ch]-reader->recon.t_e)*0.04;
					plots[1]->Fill(delta_t);
				}
			}
		}
	}
}