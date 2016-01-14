//plotRates.cpp

#include "tools/plotSeries.cpp"

TChain *rates;
TH1 *delta_t[12];
TH1 *E_gamma,*E_e,*E_p,*t_p,*Delta_t;
TFile *plotFile;

void plotRates(int *list,int n,const char* save="disk7"){
	plotFile=new TFile(TString::Format("%s/%s.root",gSystem->Getenv("RDKANAL"),save),"recreate");
	rates=new TChain("sums");
	for(int i=0;i<12;i++){
		delta_t[i]=new TH1F(TString::Format("delta_t%i",i+1),TString::Format("#Deltat_{#gamma,%i}",i+1),TGAMMA_BINS,TGAMMA_MIN,TGAMMA_MAX);
	}
	E_gamma=new TH1F("E_gamma","E_{#gamma}",EGAMMA_BINS,0,EGAMMA_RANGE);
	E_e=new TH1F("E_e","E_{e}",EE_BINS,0,EE_RANGE);
	E_p=new TH1F("E_p","E_{p}",EP_BINS,0,EP_RANGE);
	t_p=new TH1F("t_p","#Deltat_{p}",TP_BINS,0,TP_RANGE);
	Delta_t=new TH1F("delta_t","#Deltat_{#gamma}",TGAMMA_BINS,TGAMMA_MIN,TGAMMA_MAX);
	for(int i=0;i<n;i++){
		plotSeries(list[i],rates);
		TString fileName=TString::Format("P%i.root",list[i]);
		TFile file(TString::Format("%s/ss%i/%s",gSystem->Getenv("RDKANAL"),list[i],fileName.Data()));
		for(int j=0;j<12;j++){
			TH1 *tempHist=(TH1*)file.Get(TString::Format("%i_delta_t%i",list[i],j+1));
			delta_t[j]->Add(tempHist);
			Delta_t->Add(tempHist);
		}
		E_gamma->Add((TH1*)file.Get(TString::Format("%i_E_gamma",list[i])));
		E_e->Add((TH1*)file.Get(TString::Format("%i_E_e",list[i])));
		E_p->Add((TH1*)file.Get(TString::Format("%i_E_p",list[i])));
		t_p->Add((TH1*)file.Get(TString::Format("%i_t_p",list[i])));
	}
	plotFile->WriteTObject(sums);
	plotFile->Write();
}