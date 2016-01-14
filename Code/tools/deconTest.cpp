//deconTest.cpp

#include <vector>
#include <fstream>
using namespace std;

vector<double> n_event,n_ch,t_fit,t_sm,t_wd,E_fit,E_sm,E_wd;
DeconFitter *fitter=NULL;

void deconTest(Int_t s,Int_t r,TString flags="ME"){
	int detectors[3]={2,6,13};
	openRawFile(s,r);
	if(fitter){
		delete fitter;
	}
	Analyzer analyzer;
	int count=0;
	TStopwatch bench[4];
	for(int i=0;i<4;i++){
		bench[i].Reset();
	}
	bench[3].Start();
	fitter=new DeconFitter(NPOINTS,flags);
	bench[3].Stop();
	for(int i=0;i<raw->GetEntries();i++){
		raw->GetEntry(i);
		for(int j=0;j<3;j++){
			if(benchmark(analyzer,fitter,bench,i,detectors[j])){
				//cout<<i<<":"<<detectors[j]<<endl;
				count++;
			}
		}
	}
	cout<<"Total number of events: "<<count<<endl;
	cout<<"fit time: "<<bench[0].CpuTime()<<endl;
	cout<<"smooth time: "<<bench[1].CpuTime()<<endl;
	cout<<"decon time: "<<bench[2].CpuTime()<<endl;
	cout<<"fftw time: "<<bench[3].CpuTime()<<endl;
}

bool benchmark(Analyzer &anzlyer,DeconFitter *f,TStopwatch *bench,Int_t nevent,Int_t detector){
	if(recon.t_gamma[detector]>0 && recon.E_gamma[detector]>500 && recon.E_gamma[detector]<Analyzer::MAX){
		Analyzer::FIT=true;
		bench[0].Start(false);
		analyzer.recon_gamma(rdata,recon,detector);
		bench[0].Stop();
		t_fit.push_back(recon.t_gamma[detector]);
		E_fit.push_back(recon.E_gamma[detector]);
		Analyzer::FIT=false;
		bench[1].Start(false);
		analyzer.recon_gamma(rdata,recon,detector);
		bench[1].Stop();
		t_sm.push_back(recon.t_gamma[detector]);
		E_sm.push_back(recon.E_gamma[detector]);
		double input[NPOINTS],*output;
		bench[2].Start(false);
		f->setData(rdata.ch[detector]);
		f->fit();
		bench[2].Stop();
		delete[] output;
		t_wd.push_back(f->getT());
		E_wd.push_back(f->getE());
		n_event.push_back(nevent);
		n_ch.push_back(detector);
		return true;
	}
	return false;
}

void printTest(){
	ofstream out(Form("%s/%s",RDKHOME,"deconlog.csv"));
	out<<"n_event,n_det,t_fit,t_sm,t_wd,E_fit,E_sm,E_wd"<<endl;
	for(int i=0;i<t_fit.size();i++){
		out<<n_event[i]<<","<<n_ch[i]<<","<<t_fit[i]<<","<<t_sm[i]<<","<<t_wd[i];
		out<<","<<E_fit[i]<<","<<E_sm[i]<<","<<E_wd[i]<<endl;
	}
	out.close();
}