//RDKTools.h

#ifndef RDKTOOLS_H
#define RDKTOOLS_H

#include "RDKTypes.h"

#include <queue>

class ThreadedAnalyzer;
class RawFileReader;
class TTree;
class TGraph;
class TGraphErrors;

UInt_t getConfig(UInt_t);
Int_t setNThreads(Int_t);
void fitTest(const RawData&,Int_t,Int_t);
void testRun(Char_t,Int_t,Int_t);
void logNdat(const NData&);
void crunchDisk(AnalyzerMode,Int_t=0);
void crunchSeries(AnalyzerMode,Int_t,Int_t=0);
std::queue<Task> buildQueue(Int_t=0,const Char_t* ="SCD");
std::queue<Task> buildQueue(const Char_t*);
std::queue<Task> queueFromFile(const Char_t*,const Char_t* ="SCD");
void crunch();
void crunch(std::queue<Task>,Int_t=0);
void crunch(std::queue<Task>,AnalyzerMode,Int_t=0);
template <class T> void crunchTemplate(std::queue<Task>,AnalyzerMode,Int_t=0);
Double_t fitFunction(Double_t*,Double_t*);
void cooperAnalysis(const RawData&,Int_t*,Double_t&,Double_t);
void cooperTest(RawFileReader*,TTree*,Double_t);
void calibrate();
void updateNdat();
void exportTimestamps(Char_t,Int_t,Int_t);
void crunchFromFile(const Char_t*,const Char_t* ="SCD");
void second_pass(TString="",Bool_t=kTRUE);
void importConfig(const Char_t*);
void createEpCal();
TGraph *graphMatt(TString);
TGraphErrors *egraphMatt(TString);
TTree *openAllTrees();
TTree *openSet1();
TTree *openSet2();
TTree *openSet2a();
TTree *openSet2b();
TTree *openSet3();
TTree *openSet4();
void exportTree(TTree*,TString);
void importJeffData(TString,TString);
void parseJeffLine(TString,epEvent&);

#endif