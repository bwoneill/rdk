//RDKLinkDef.h

//analyzers
#pragma link C++ class Analyzer;
#pragma link C++ class ThreadedAnalyzer;
#pragma link C++ class SecondPass;
//file readers/writers
#pragma link C++ class DataReader;
#pragma link C++ class NdatReader;
#pragma link C++ class NdatWriter;
#pragma link C++ class RDKReader;
#pragma link C++ class RawTreeReader;
#pragma link C++ class TreeFilter;
#pragma link C++ class TreeReader;
#pragma link C++ class Plotter;
#pragma link C++ class Plotter2;
#pragma link C++ class RawFileReader;
#pragma link C++ class BufferedReader;
#pragma link C++ class EventDrawer;
//fit algorithms
#pragma link C++ class EpFitter;
#pragma link C++ class GFitter;
//#pragma link C++ class EpFitFunction;
#pragma link C++ class EpgFitFunction;
//#pragma link C++ class GFitFunction;
#pragma link C++ class EpFilter;
//smoothers
#pragma link C++ class Smoother;
//data structures
#pragma link C++ class epEvent;
#pragma link C++ class BoardEvent;
#pragma link C++ class Entry;
#pragma link C++ class RawData;
#pragma link C++ class Recon;
#pragma link C++ class NData;
#pragma link C++ class Header;
#pragma link C++ class CalibrationData+;
#pragma link C++ class CalibrationList+;
#pragma link C++ function operator<<(std::ostream&,const CalibrationData&);
#pragma link C++ function operator<<(std::ostream&,const CalibrationList&);
#pragma link C++ class Quantity;
#pragma link C++ function operator+(Double_t,const Quantity&);
#pragma link C++ function operator-(Double_t,const Quantity&);
#pragma link C++ function operator*(Double_t,const Quantity&);
#pragma link C++ function operator/(Double_t,const Quantity&);
#pragma link C++ class epComp;
//thread
#pragma link C++ class Runnable;
#pragma link C++ class ThreadRunner;
#pragma link C++ class ThreadQueue;
#pragma link C++ class MultiThread;
#pragma link C++ class ExeWrapper;
//miscellaneous
#pragma link C++ class RDKException;
//global
#pragma link C++ global NPOINTS;
#pragma link C++ global NFLAT;
#pragma link C++ global NGAMMA;
#pragma link C++ global RDKANAL;
#pragma link C++ global RDKRAW;
#pragma link C++ global RDKHOME;
#pragma link C++ global WORKDIR;
#pragma link C++ global rdk2config;
//tools
#pragma link C++ class RDKConfig;
#pragma link C++ class Task;
#pragma link C++ class vector<Task>;
#pragma link C++ class queue<Task>;
#pragma link C++ class CompareData;
#pragma link C++ function second_pass(TString,Bool_t);
#pragma link C++ function importConfig(const Char_t*);
#pragma link C++ function getConfig(UInt_t);
#pragma link C++ function operator<<(std::ostream&,Task);
#pragma link C++ function logNdat(const NData&);
#pragma link C++ function crunchDisk(Int_t);
#pragma link C++ function crunchSeries(Int_t,Int_t);
#pragma link C++ function buildQueue(Int_t);
#pragma link C++ function buildQueue(const Char_t*);
#pragma link C++ function crunch();
#pragma link C++ function crunch(queue<Task>,Int_t);
#pragma link C++ function crunch(queue<Task>,AnalyzerMode,Int_t);
#pragma link C++ function fitFunction(Double_t*,Double_t*);
#pragma link C++ function cooperTest(RawFileReader*,TTree*,Double_t);
#pragma link C++ function cooperAnalysis(const RawData&,Int_t*,Double_t&,Double_t);
#pragma link C++ function calibrate();
#pragma link C++ function updateNdat();
#pragma link C++ function exportTimestamps(Char_t,Int_t,Int_t);
#pragma link C++ function crunchFromFile(const Char_t*,const Char_t*);
#pragma link C++ function queueFromFile(const Char_t*,const Char_t*);
#pragma link C++ function createEpCal();
#pragma link C++ function graphMatt(TString);
#pragma link C++ function egraphMatt(TString);
#pragma link C++ function openAllTrees();
#pragma link C++ function openSet1();
#pragma link C++ function openSet2();
#pragma link C++ function openSet2a();
#pragma link C++ function openSet2b();
#pragma link C++ function openSet3();
#pragma link C++ function openSet4();
#pragma link C++ function exportTree(TTree*,TString);
#pragma link C++ function importJeffData(TString,TString);
#pragma link C++ function parseJeffLine(TString,epEvent&);
//test
#pragma link C++ function setNThreads(Int_t);
#pragma link C++ function fitTest(const RawData&,Int_t,Int_t);
#pragma link C++ function testRun(Char_t,Int_t,Int_t);