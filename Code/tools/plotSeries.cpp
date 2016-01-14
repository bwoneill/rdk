//plotSeries.cpp

const int EGAMMA_BINS=500,EGAMMA_RANGE=15000;
const int EE_BINS=500,EE_RANGE=35000;
const int EP_RANGE=1200,EP_BINS=EP_RANGE;
const int TP_RANGE=1000,TP_BINS=TP_RANGE;
const int TGAMMA_BINS=100,TGAMMA_MIN=-75,TGAMMA_MAX=25;

TString plotSeries(int series,TChain *chain=NULL){
	TSystemDirectory *dir=new TSystemDirectory(".",TString::Format("%s/ss%i",gSystem->Getenv("RDKANAL"),series));
	TList *list=dir->GetListOfFiles();
	for(int i=0;i<list->GetEntries();i++){
		TSystemFile *file=(TSystemFile*)list->At(i);
		TString name=file->GetName();
		if(name.BeginsWith("S")){
			//open data
			TFile *in=new TFile(TString::Format("%s/ss%i/%s",gSystem->Getenv("RDKANAL"),series,name.Data()));
			TTree *tree=(TTree*)in->Get("rdk"),*ndat=(TTree*)in->Get("ndat");
			Header header((TMap*)in->Get("header1"));
			Recon recon;
			tree->SetBranchStatus("*",0);
			tree->SetBranchStatus("recon",1);
			tree->SetBranchAddress("recon",&recon);
			int ndata[12];
			ndat->SetBranchAddress("ndata",ndata);
			//create plots
			name.Replace(0,1,"P");
			TFile *out=new TFile(TString::Format("%s/ss%i/%s",gSystem->Getenv("RDKANAL"),series,name.Data()),"RECREATE");
			TTree *sums=new TTree("sums","Sum of ndat values");
			TString temp=name(name.Index("r")+1,name.Length());
			temp=temp(0,temp.Index("."));
			int nflux=0,ep_count=0,time=0,run=0;
			double start=0;
			temp=header.getValue("Running Parameters.Acquisition start time");
			if(temp.IsFloat()){
				start=temp.Atof();
			}
			sums->Branch("nflux",&nflux,"nflux/I");
			sums->Branch("ep_count",&ep_count,"ep_count/I");
			sums->Branch("time",&time,"time/I");
			sums->Branch("run",&run,"run/I");
			sums->Branch("series",&series,"series/I");
			sums->Branch("start",&start,"start/D");
			TH1 *delta_t[12];
			for(int j=0;j<12;j++){
				delta_t[j]=new TH1F(TString::Format("delta_t%i",j+1),TString::Format("#Deltat_{#gamma,%i}",j+1),TGAMMA_BINS,TGAMMA_MIN,TGAMMA_MAX);
				tree->Draw(TString::Format("t_gamma[%i]-t_e[%i]>>delta_t%i",j,j,j+1),TString::Format("t_gamma[%i]>0 && E_gamma[%i]>2*noise[%i]",j,j,j),"goff");
			}
			TH1 *E_gamma=new TH1F("E_gamma","E_{#gamma}",EGAMMA_BINS,0,EGAMMA_RANGE),
				*E_p=new TH1F("E_p","E_{p}",EP_BINS,0,EP_RANGE),
				*E_e=new TH1F("E_e","E_{e}",EE_BINS,0,EE_RANGE),
				*t_p=new TH1F("t_p","#Deltat_{p}",TP_BINS,0,TP_RANGE);
			tree->Draw("E_gamma>>E_gamma","t_gamma>0 && E_gamma>2*noise","goff");
			tree->Draw("E_p>>E_p","","goff");
			tree->Draw("E_e>>E_e","","goff");
			tree->Draw("t_p-t_e>>t_p","","goff");
			nflux=0;ep_count=0;time=0;
			for(int j=0;j<ndat->GetEntries()-1;j++){
				ndat->GetEntry(j);
				nflux+=ndata[0]+ndata[2]+ndata[4]+ndata[6];
				ep_count+=ndata[10];
				time+=ndata[11];
				double test=ndata[0]+ndata[2]+ndata[4]+ndata[6];
			}
			sums->Fill();
			out->Write();
			out->Close();
			in->Close();
		}
	}
	//create totals for series
	TString name=TString::Format("P%i.root",series);
	TFile *out=new TFile(name,"RECREATE");
	TChain *localChain=new TChain("sums");
	list=dir->GetListOfFiles();
	TH1 *delta_t[12];
	for(int j=0;j<12;j++){
		delta_t[j]=new TH1F(TString::Format("%i_delta_t%i",series,j+1),TString::Format("#Deltat_{#gamma,%i}",j+1),TGAMMA_BINS,TGAMMA_MIN,TGAMMA_MAX);
	}
	TH1 *E_gamma=new TH1F(TString::Format("%i_E_gamma",series),"E_{#gamma}",EGAMMA_BINS,0,EGAMMA_RANGE),
		*E_p=new TH1F(TString::Format("%i_E_p",series),"E_{p}",EP_BINS,0,EP_RANGE),
		*E_e=new TH1F(TString::Format("%i_E_e",series),"E_{e}",EE_BINS,0,EE_RANGE),
		*t_p=new TH1F(TString::Format("%i_t_p",series),"#Deltat_{p}",TP_BINS,0,TP_RANGE);
	for(int i=0;i<list->GetEntries();i++){
		TSystemFile *file=(TSystemFile*)list->At(i);
		TString temp=file->GetName();
		if(temp.BeginsWith("P") && temp.CompareTo(name)){
			//sum over runs
			temp.Form("%s/ss%i/%s",gSystem->Getenv("RDKANAL"),series,temp.Data());
			localChain->Add(temp);
			if(chain!=NULL){
				chain->Add(temp);
			}
			TFile *in=new TFile(temp);
			for(int j=0;j<12;j++){
				delta_t[j]->Add((TH1*)in->Get(TString::Format("delta_t%i",j+1)));
			}
			E_gamma->Add((TH1*)in->Get("E_gamma"));
			E_p->Add((TH1*)in->Get("E_p"));
			E_e->Add((TH1*)in->Get("E_e"));
			t_p->Add((TH1*)in->Get("t_p"));
			in->Close();
		}
	}
	out->Write();
	out->WriteTObject(localChain);
	out->Close();
	gSystem->cd(Form("%s/Code",RDKHOME));
	return name;
}