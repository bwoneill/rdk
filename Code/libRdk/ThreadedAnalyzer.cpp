//ThreadedAnalyzer.cpp

#include "ThreadedAnalyzer.h"
#include "Analyzer.h"
#include "CalibrationData.h"
#include "RawTreeReader.h"
#include "NdatWriter.h"
#include "Header.h"
#include "EpgFitFunction.h"
#include "EpFitter.h"
#include "RDK_OMP.h"

#include "TThread.h"
#include "TMutex.h"
#include "TFile.h"
#include "TTree.h"
#include "TStopwatch.h"
#include "TMath.h"
#include "TSystem.h"
#include "TBits.h"
#include "TGraph.h"
#include "TF1.h"
#include "TMarker.h"
#include "TLatex.h"
#include "TAxis.h"
#include "TLine.h"
#include "TFitResultPtr.h"
#include "TFitResult.h"
using namespace TMath;

#include <iostream>
using namespace std;

typedef TThread::VoidRtnFunc_t VoidRtnFunc_t;

#define BGO_OFFSET 64
#define BAPD_OFFSET 32
#define MIN_BASELINE 128
#define CHI2MIN 4900

ThreadedAnalyzer::ThreadedAnalyzer():tSmoother(4,1),eSmoother(32,2),epfunc(2048){
	analyzers=new Analyzer*[14];
	for(Int_t i=0;i<14;i++){
		analyzers[i]=new Analyzer(&eSmoother,&tSmoother);
	}
	tree=NULL;
	file=NULL;
	fitter=EpFitter::getFitter();
	initialize();
}

ThreadedAnalyzer::ThreadedAnalyzer(const Task &t):tSmoother(4,1),eSmoother(32,2),
task(t),epfunc(2048){
	analyzers=new Analyzer*[14];
	for(Int_t i=0;i<14;i++){
		analyzers[i]=new Analyzer(&eSmoother,&tSmoother,task.config);
	}
	tree=NULL;
	file=NULL;
	openned=kFALSE;
	fitter=EpFitter::getFitter();
	initialize();
}

ThreadedAnalyzer::~ThreadedAnalyzer(){
	close();
	delete[] analyzers;
}

void ThreadedAnalyzer::initialize(){
	fileLock->Lock();
	//if(!fitter){
	//	TVirtualFitter::SetDefaultFitter("Minuit2");
	//	fitter=(TFitterMinuit*)TVirtualFitter::Fitter(0,7);
	//	if(!DEBUG){
	//		fitter->SetPrintLevel(-1);
	//	}
	//	fitter->CreateMinimizer();
	//	fitter->SetMinuitFCN(func);
	//}
	if(!INITIALIZED){
		INITIALIZED=kTRUE;
		original->SetLineColor(kBlack);
		original->SetLineWidth(2);
		smooth1->SetLineColor(kRed);
		smooth1->SetLineWidth(2);
		marker1->SetMarkerColor(kRed);
		marker1->SetMarkerStyle(kFullDotLarge);
		smooth2->SetLineColor(kBlue);
		smooth2->SetLineWidth(2);
		marker2->SetMarkerColor(kBlue);
		marker2->SetMarkerStyle(kFullDotLarge);
		function->SetLineColor(kGreen+2);
		marker3->SetMarkerColor(kGreen+2);
		marker3->SetMarkerStyle(kFullDotLarge);
		function->SetLineStyle(9);
		t_line->SetLineColor(kRed);
		t_line->SetLineStyle(7);
		E_line->SetLineColor(kBlue);
		E_line->SetLineStyle(7);
		sigfunc->SetLineColor(kGreen);
		sigfunc->SetLineWidth(2);
	}
	fileLock->UnLock();
}

void ThreadedAnalyzer::open(const Task &t){
	try{
		fileLock->Lock();
		close();
		task.series=t.series;
		task.run=t.run;
		task.type=t.type;
		task.config=t.config;
		analyzers[0]->setConfig(task.config);
		analyzers[1]->setConfig(task.config);
		reader.open(task.type,task.series,task.run);
		TString temp;
		temp.Form("%s/ss%i",RDKANAL,task.series);
		FileStat_t stats;
		if(gSystem->GetPathInfo(temp,stats)){
			gSystem->mkdir(temp,kTRUE);
		}
		temp.Form("%s/ss%i/%c%ir%i.root",RDKANAL,task.series,task.type,task.series,task.run);
		file=new TFile(temp,"recreate");
		Header *headers=reader.getHeaders();
		for(Int_t i=0;i<2;i++){
			file->WriteTObject(headers[i].getHeader(),"headers");
		}
		Double_t start=headers[0].getValue(
				"Running Parameters.Acquisition start time").Atof();
		temp.Form("rdk_%c%ir%i",task.type,task.series,task.run);
		tree=new TTree(temp,"RDK data");
		tree->Branch("recon",&recon,Recon::BRANCH);
		tree->Branch("entry",&entry,Entry::BRANCH);
		NdatWriter writer(task.type,task.series,task.run,start,file);
		writer.write();
		openned=kTRUE;
	}catch(RDKException e){
		if(task.type=='\0'){
			task.type='*';
		}
		TString temp;
		temp.Form("Fatal error openning ss%i/%c%ir%i files",task.series,task.type,
			task.series,task.run);
		TThread::Printf(temp);
		TThread::Printf(e.what());
		openned=kFALSE;
	}
	fileLock->UnLock();
}

void ThreadedAnalyzer::open(Char_t type,Int_t series,Int_t run,UInt_t config){
	open(Task("",series,run,type,config));
}

void ThreadedAnalyzer::close(){
	reader.close();
	if(file && tree){
		fileLock->Lock();
		//tree->Write("rdk",TObject::kOverwrite);
		file->WriteTObject(tree,"rdk","overwrite");
		file->Close();
		fileLock->UnLock();
		tree=NULL;
		file=NULL;
	}
	openned=kFALSE;
}

void ThreadedAnalyzer::analyzeEP(const RawData &rdata,Recon &recon,Bool_t print){
	const Short_t *sig=rdata.ch[15];
	Int_t t_e=0;
	Double_t E_e=sig[0];
	for(Int_t i=0;i<896;i++){
		if(sig[i]>E_e){
			E_e=sig[i];
			t_e=i;
		}
	}
	//the baseline average is incorrect !!!!!
	//now correct?
	Double_t avg=0;
	Int_t start=0,stop=t_e-128;
	for(Int_t i=start;i<stop;i++){
		avg+=sig[i];
	}
	start=t_e+896;
	stop=2048;
	for(Int_t i=start;i<stop;i++){
		avg+=sig[i];
	}
	avg/=1024;
	stop=t_e+625;
	Int_t t_p=t_e+50;
	Double_t E_p=sig[t_p];
	Bool_t descend=kTRUE;
	for(Int_t i=t_p;i<stop;i++){
		if(descend && i<2043){
			descend=kFALSE;
			for(Int_t j=1;j<5;j++){
				descend|=sig[i]>sig[i+j];
			}
			E_p=sig[i];
			t_p=i;
			if(!descend){
				recon.t_dip=i;
			}
		}else if(sig[i]>E_p){
			E_p=sig[i];
			t_p=i;
		}
	}
	Int_t t_q=recon.t_dip;
	Double_t E_q=sig[t_q];
	for(Int_t i=t_q;i<stop;i++){
		if((i<t_p-25 || i>t_p+25) && sig[i]>E_q){
			E_q=sig[i];
			t_q=i;
		}
	}
	recon.t_q=t_q;
	recon.E_q=E_q;
	if(EPFIT){
		param[0]=avg;
		param[1]=E_e-avg;
		param[2]=t_e;
		param[3]=5;
		param[4]=E_p-avg;
		param[5]=t_p;
		param[6]=5;
		TFitResultPtr result=fitter->fit(rdata.ch[15],param);
		recon.ep_offset=result->Parameter(0);
		recon.E_e=result->Parameter(1);
		recon.t_e=result->Parameter(2);
		recon.sigma_e=result->Parameter(3);
		recon.E_p=result->Parameter(4);
		recon.t_p=result->Parameter(5);
		recon.E_q-=recon.ep_offset;
		recon.sigma_p=result->Parameter(6);
		recon.ep_chi2=result->Chi2();
		recon.ep_edm=result->Edm();
	}else{
		recon.E_e=E_e-avg;
		recon.t_e=t_e;
		recon.E_p=E_p-avg;
		recon.t_p=t_p;
		recon.t_half=0;
		recon.E_q-=avg;
		for(Int_t i=t_e;i<2048 && recon.t_half==0;i++){
			if(2*sig[i]<E_e+avg){
				recon.t_half=i;
			}
		}
	}
	if(DEBUG && print){
		for(Int_t i=0;i<2048;i++){
			original->SetPoint(i,0.04*i,sig[i]);
		}
		original->SetTitle("SBD signal trace;Time (#mus);Voltage (arbitrary units, approx. 32/keV)");
		if(EPFIT){
			sigfunc->SetParameters(recon.E_e,recon.t_e*0.04,recon.sigma_e*0.04,
				recon.E_p,recon.t_p*0.04,recon.sigma_p*0.04,recon.ep_offset);
			original->Draw("al");
			original->GetXaxis()->SetRangeUser(0,2048*0.04);
			sigfunc->Draw("same");
			TString text;
			text.Form("#splitline{t_{e}=%0.2f}{E_{e}=%0.2f}",recon.t_e*0.04,recon.E_e);
			marker1->DrawMarker(t_e*0.04,recon.E_e+recon.ep_offset);
			text1->DrawLatex(t_e*0.04+1,recon.E_e+recon.ep_offset,text);
			marker2->DrawMarker(t_p*0.04,recon.E_p+recon.ep_offset);
			text.Form("#splitline{t_{p}=%0.2f}{E_{p}=%0.2f}",recon.t_p*0.04,recon.E_p);
			text2->DrawLatex(t_p*0.04-5,recon.E_p+recon.ep_offset+1000,text);
			marker3->DrawMarker(t_q*0.04,recon.E_q+recon.ep_offset);
			text.Form("#splitline{t_{q}=%0.2f}{E_{q}=%0.2f}",recon.t_q*0.04,recon.E_q);
			text3->DrawLatex(t_q*0.04+1,recon.E_q+recon.ep_offset-1000,text);
		}else{
			function->SetParameters(avg,0);
			original->Draw("al");
			original->GetXaxis()->SetRangeUser(0,2048*0.04);
			function->Draw("same");
			marker1->DrawMarker(t_e*0.04,E_e);
			marker2->DrawMarker(t_p*0.04,E_p);
			marker3->DrawMarker(t_q*0.04,E_q);
			TAxis *axis=original->GetYaxis();
			E_line->DrawLine(recon.t_dip*0.04,axis->GetXmin(),recon.t_dip*0.04,axis->GetXmax());
			t_line->DrawLine(recon.t_half*0.04,axis->GetXmin(),recon.t_half*0.04,axis->GetXmax());
		}
	}
}

void ThreadedAnalyzer::analyze(const RawData &rdata,Recon &recon,Int_t print){
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic) num_threads(14)
#endif
	for(Int_t n=0;n<14;n++){
		analyzers[n]->preanalyze(rdata,recon,n,print==n);
	}
	for(Int_t n=0;n<14;n++){
		if(DEBUG){
			cerr<<n<<endl;
		}
		analyzers[n]->analyze(rdata,recon,n,print==n);
	}
	analyzeEP(rdata,recon,print==15);
}

Int_t ThreadedAnalyzer::run(){
	try{
		if(!openned){
			open(task);
		}
		while(openned && reader.readNext(entry,rdata)){
			analyze(rdata,recon);
			fileLock->Lock();
			tree->Fill();
			fileLock->UnLock();
		}
	}catch(...){
		TThread::Printf("Uncaught exception running task ss%i/%c%ir%i",
			task.series,task.type,task.series,task.run);
	}
	return 1;
}

void ThreadedAnalyzer::clean(){
	try{
		if(openned){
			TThread::Printf("Saving file ss%i/%c%ir%i.root",
				task.series,task.type,task.series,task.run);
		}
		close();
	}catch(...){
		TThread::Printf("Uncaught exception cleaning task ss%i/%c%ir%i",
			task.series,task.type,task.series,task.run);
	}
}

Bool_t ThreadedAnalyzer::DEBUG=kFALSE;
Bool_t ThreadedAnalyzer::EPFIT=kTRUE;
Bool_t ThreadedAnalyzer::INITIALIZED=kFALSE;
TGraph *ThreadedAnalyzer::original=new TGraph(2048);
TGraph *ThreadedAnalyzer::smooth1=new TGraph(2048);
TGraph *ThreadedAnalyzer::smooth2=new TGraph(2048);
TF1 *ThreadedAnalyzer::function=new TF1("ta1","pol1",0,82);
TF1 *ThreadedAnalyzer::sigfunc=new TF1("epsig",drawFitFunc,&EpFitFunction::evaluate,0,82,8,"EpFitFunction","evaluate");
TMarker *ThreadedAnalyzer::marker1=new TMarker();
TMarker *ThreadedAnalyzer::marker2=new TMarker();
TMarker *ThreadedAnalyzer::marker3=new TMarker();
TLatex *ThreadedAnalyzer::text1=new TLatex();
TLatex *ThreadedAnalyzer::text2=new TLatex();
TLatex *ThreadedAnalyzer::text3=new TLatex();
TLine *ThreadedAnalyzer::t_line=new TLine();
TLine *ThreadedAnalyzer::E_line=new TLine();
EpFitFunction *ThreadedAnalyzer::drawFitFunc=new EpFitFunction(2048);