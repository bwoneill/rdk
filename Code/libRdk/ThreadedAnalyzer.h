//ThreadedAnalyzer.h

#ifndef THREADEDANALYZER_H
#define THREADEDANALYZER_H

#include "RDKTypes.h"
#include "Smoother.h"
#include "RawFileReader.h"
#include "Threads.h"
#include "EpFitFunction.h"
//#include "EpgFitFunction.h"

#include "TVectorD.h"

#include <vector>

class TFile;
class TTree;
class TGraph;
class TF1;
class TMarker;
class TLine;
class TLatex;
class Analyzer;
class EpFitter;

class ThreadedAnalyzer:public Runnable{
private:
	Analyzer **analyzers;
	Bool_t openned;
	Smoother tSmoother,eSmoother;
	RawFileReader reader;
	Entry entry;
	Recon recon;
	RawData rdata;
	Task task;
	TFile *file;
	TTree *tree;
	EpFitFunction epfunc;
	EpFitter *fitter;
	Double_t param[7];
	static Bool_t INITIALIZED;
	//static TFitterMinuit *fitter;
	//static EpgFitFunction *func;
	static void initialize();
	void analyzeEP(const RawData&,Recon&,Bool_t=kFALSE);
public:
	ThreadedAnalyzer();
	ThreadedAnalyzer(const Task&);
	~ThreadedAnalyzer();
	void analyze(const RawData&,Recon&,Int_t=-1);
	void open(Char_t,Int_t,Int_t,UInt_t=DEFAULT_CONFIG);
	void open(const Task&);
	void close();
	Int_t run();
	void clean();
	static Bool_t DEBUG,EPFIT;
	static TGraph *original,*smooth1,*smooth2;
	static TF1 *function,*sigfunc;
	static TMarker *marker1,*marker2,*marker3;
	static TLatex *text1,*text2,*text3;
	static TLine *t_line,*E_line;
	static EpFitFunction *drawFitFunc;
	friend class Analyzer;
};

#endif