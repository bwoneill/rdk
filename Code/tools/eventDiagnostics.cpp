//eventDiagnostics.cpp

RawFileReader reader;
TreeReader treader;
ThreadedAnalyzer tanal;
CalibrationList list(Form("%s/calibration/TCfit_511keV_calibration_21Feb11.dat",RDKANAL));
TLine line[5];
TGraph graph(2048),graphSmooth(2048);
Int_t threshold=50;

Entry entry;
Recon recon,recon2;
RawData rdata;

Double_t ratio=0.05;


void getEvent(Char_t c,Int_t series,Int_t run,Int_t n,Int_t ch=2){
	ThreadedAnalyzer::DEBUG=kTRUE;
	reader.open(c,series,run);
	reader.seek(n);
	reader.readNext(entry,rdata);
	vector<double> in(2048),temp(2048),out(2048);
	for(Int_t i=0;i<2048;i++){
		in[i]=rdata.ch[ch][i];
	}
	tanal.analyze(rdata,recon,ch);
	line[0].SetX1(recon.t_gamma[ch]*0.04);
	line[0].SetX2(recon.t_gamma[ch]*0.04);
	line[0].SetY1(recon.E_gamma[ch]*1.05);
	line[0].SetY2(recon.E_gamma[ch]*-0.05);
	line[0].SetLineColor(kRed);
	for(Int_t i=0;i<2048;i++){
		graph.SetPoint(i,i*0.04,in[i]);
	}
	double E=recon.E_gamma[ch];
	recon=list.calibrate(entry,recon);
	cout<<"E_gamma: "<<E<<" = "<<recon.E_gamma[ch]<<" keV"<<endl;
	cout<<"delta_t: "<<recon.t_gamma[ch]-recon.t_e<<"="<<(recon.t_gamma[ch]-recon.t_e)*0.04<<"\\mu s"<<endl;
	cout<<"t_gamma: "<<recon.t_gamma[ch]<<"="<<recon.t_gamma[ch]*0.04<<"\\mu s"<<endl;
	cout<<"E_e: "<<recon.E_e<<endl;
	cout<<"t_p-t_e: "<<recon.t_p-recon.t_e<<"="<<(recon.t_p-recon.t_e)*0.04<<"\\mu s"<<endl;
	cout<<"t_half: "<<recon.t_half-recon.t_e<<"="<<(recon.t_half-recon.t_e)*0.04<<"\\mu s"<<endl;
	cout<<"t_dip: "<<recon.t_dip-recon.t_e<<"="<<(recon.t_dip-recon.t_e)*0.04<<"\\mu s"<<endl;
	reader.close();
}

void nextEvent(Char_t c,Int_t series,Int_t run,Int_t n,Int_t ch=2){
	Bool_t current=ThreadedAnalyzer::DEBUG;
	ThreadedAnalyzer::DEBUG=kFALSE;
	reader.open(c,series,run);
	bool stop=false;
	do{
		reader.seek(n);
		stop=!reader.readNext(entry,rdata);
		tanal.analyze(rdata,recon);
		if(recon.t_gamma[ch]>0 && recon.t_gamma[ch]<2048 && recon.E_gamma[ch]>300 && recon.E_gamma[ch]<30000){
			stop=true;
		}else{
			n++;
		}
	}while(!stop);
	reader.close();
	cout<<"series: "<<series<<endl;
	cout<<"run: "<<run<<endl;
	cout<<"event: "<<n<<endl;
	cout<<"channel: "<<ch<<endl;
	ThreadedAnalyzer::DEBUG=kTRUE;
	getEvent(c,series,run,n,ch);
	ThreadedAnalyzer::DEBUG=current;
}

void drawEpEvent(Char_t c,Int_t series,Int_t run,Int_t n){
	Bool_t current=ThreadedAnalyzer::DEBUG;
	ThreadedAnalyzer::DEBUG=kTRUE;
	reader.open(c,series,run);
	reader.seek(n);
	reader.readNext(entry,rdata);
	tanal.analyze(rdata,recon,15);
	ThreadedAnalyzer::DEBUG=current;
}