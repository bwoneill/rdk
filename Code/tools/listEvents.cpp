//listEvents.cpp

#include <fstream>
#include <cmath>
using namespace std;

Int_t PEAK=-26,ONPEAK=20,CH=2;
Double_t MIN=0.5,MAX=40;

void listEvents(){
	Recon recon;
	//TFile f(Form("%s/cal.root",RDKANAL));
	//CalibrationList *list=(CalibrationList*)f.Get("CalibrationList");
	TString path=Form("%s/calibration/TCfit_511keV_calibration_21Feb11.dat",RDKANAL);
	CalibrationList *list=new CalibrationList(path);
	TreeReader reader;
	reader.reset(kTRUE);
	//NData ndata=reader.openList(Form("%s/runlist.csv",RDKANAL));
	NData ndata=reader.openSeries(256,"S");
	//NData ndata=reader.openSeries(TreeReader::n1400,TreeReader::all1400,"S");
	logNdat(ndata);
	ofstream log(Form("%s/eventLists/list.csv",RDKANAL));
	log<<"Series,Run,EventNum,PeakHeight,Energy,DeltaTime"<<endl;
	Int_t series=0,run=0,evnt=0;
	for(Long64_t i=0;i<reader.tree->GetEntries();i++){
		if(i%10000==0){
			cout<<i<<endl;
		}
		reader.tree->GetEntry(i);
		//if(series==reader.entry.nseries && run==reader.entry.nrun){
		//	reader.entry.nevent=++evnt;
		//}else{
		//	series=reader.entry.nseries;
		//	run=reader.entry.nrun;
		//	evnt=0;
		//}
		recon=list->calibrate(reader.entry,reader.recon);
		Int_t delta_t=recon.t_gamma[CH]-recon.t_e-PEAK;
		if(recon.E_gamma[CH]>=MIN && recon.E_gamma[CH]<=MAX && recon.t_p-recon.t_e<650
			&& delta_t>-ONPEAK && delta_t<=ONPEAK && recon.E_e<30000 && recon.E_p>300 && recon.E_p<1000){
			log<<reader.entry.nseries<<","<<reader.entry.nrun<<","<<reader.entry.nevent<<",";
			log<<reader.recon.E_gamma[CH]<<","<<recon.E_gamma[CH]<<",";
			log<<(recon.t_gamma[CH]-recon.t_e)<<endl;
		}
	}
	log.close();
	delete list;
	//f.close();
	gSystem->ChangeDirectory(WORKDIR);
}