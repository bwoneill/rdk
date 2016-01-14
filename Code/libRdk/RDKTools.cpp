//RDKTools.cpp

#include "RDKTypes.h"
#include "RDKTools.h"
#include "RDKReader.h"
#include "Threads.h"
#include "RawTreeReader.h"
#include "RawFileReader.h"
#include "ThreadedAnalyzer.h"
#include "CalibrationData.h"
#include "CalibrationList.h"
#include "TreeFilter.h"
#include "NdatWriter.h"
#include "SecondPass.h"
#include "ExeWrapper.h"
#include "EpFilter.h"

#include "TAxis.h"
#include "TChain.h"
#include "TString.h"
#include "TSystem.h"
#include "TSystemDirectory.h"
#include "TSystemFile.h"
#include "TList.h"
#include "TPRegexp.h"
#include "TThread.h"
#include "TTree.h"
#include "TSemaphore.h"
#include "TBits.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStopwatch.h"
#include "TGraphErrors.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>
using namespace std;
#include "TMath.h"
using namespace TMath;
#include "bfstream.h"
#include "boost/iostreams/positioning.hpp"

#ifdef __GNUG__
#include <omp.h>
#endif

#include "TVirtualFitter.h"
#include "TFitterMinuit.h"
#include "EpFitFunction.h"

TFitterMinuit *fitter=NULL;
EpFitFunction *func=NULL;

Int_t setNThreads(Int_t n){
#ifdef __GNUG__
	omp_set_num_threads(n);
	return omp_get_max_threads();
#else
	return 1;
#endif
}

void createFitter(){
	TVirtualFitter::SetDefaultFitter("Minuit2");
	fitter=(TFitterMinuit*)TVirtualFitter::Fitter(0);
	func=new EpFitFunction(2048);
	fitter->SetMinuitFCN(func);
	fitter->CreateMinimizer();
	fitter->SetPrintLevel(-1);
}

void resetFitter(){
	fitter->Clear();
	fitter->CreateMinimizer();
	fitter->SetParameter(0,"p0",12000,1,0,30000);
	fitter->SetParameter(1,"p1",625,1,0,2048);
	fitter->SetParameter(2,"p2",5,1,0,100);
	fitter->SetParameter(3,"p3",500,1,0,30000);
	fitter->SetParameter(4,"p4",825,1,0,2048);
	fitter->SetParameter(5,"p5",5,1,0,100);
	fitter->SetParameter(6,"p6",0,1,-1000,1000);
}

void fitTest(const RawData &rdata,Int_t n,Int_t m){
	setNThreads(16);
	if(fitter==NULL || func==NULL){
		createFitter();
	}
	setNThreads(n);
	cout<<"Number of threads: "<<n<<endl;
	TStopwatch clock;
	clock.Start();
	for(Int_t j=0;j<m;j++){
		for(Int_t i=0;i<2048;i++){
			func->setPoint(i,rdata.ch[15][i]);
		}
		resetFitter();
		fitter->Minimize();
	}
	clock.Stop();
	cout<<"Real time: "<<clock.RealTime()<<"s"<<endl;
	cout<<"Cpu time: "<<clock.CpuTime()<<"s"<<endl;
}

map<UInt_t,UInt_t> config_map;

void importConfig(const Char_t* path){
	cout<<"Importing configuration table"<<endl;
	if(!config_map.empty()){
		config_map.clear();
	}
	ifstream in(path);
	TString line;
	line.ReadLine(in);
	while(!in.eof()){
		line.ReadLine(in);
		TStringToken token(line,",",kTRUE);
		token.NextToken();
		UInt_t series=token.Atoi();
		token.NextToken();
		token.NextToken();
		token.NextToken();
		token.NextToken();
		token.NextToken();
		TString temp(token);
		if(temp.BeginsWith("0x")){
			temp.Remove(0,2);
			temp=TString::BaseConvert(temp,16,10);
		}
		UInt_t config=temp.Atoi();
		config_map[series]=config;
	}
	cout<<config_map.size()<<endl;
}

UInt_t getConfig(UInt_t series){
	return config_map[series];
}

void testRun(Char_t t,Int_t s,Int_t r){
	ThreadedAnalyzer tanal;
	tanal.open(t,s,r);
	TStopwatch clock;
	clock.Start();
	tanal.run();
	tanal.clean();
	clock.Stop();
	cout<<"Run time: "<<clock.RealTime()<<endl;
	cout<<"Cpu time: "<<clock.CpuTime()<<endl;
}

void logNdat(const NData &ndata){
	TString temp;
	temp.Form("%s/log.txt",RDKANAL);
	ofstream log(temp,ios_base::out | ios_base::app);
	log<<"Ndat sums"<<endl;
	log<<"\tnn: "<<ndata.nn<<endl;
	log<<"\tnep: "<<ndata.nep<<endl;
	log<<"\tt: "<<ndata.t<<" ms="<<ndata.t/8.64e7<<" days"<<endl;
	log.close();
}

void crunchDisk(AnalyzerMode mode,Int_t N){
	if(config_map.empty()){
		TString path;
		path.Form("%s/detector_config.csv",RDKANAL);
		importConfig(path);
	}
	SysInfo_t sys;
	gSystem->GetSysInfo(&sys);
	const Int_t ncore=N>0?N:sys.fCpus+N;
	MultiThread multithread(ncore);
	TSystemDirectory dir(".",RDKRAW);
	TList *list=dir.GetListOfFiles();
	Int_t count=0;
	for(Int_t i=0;i<list->GetEntries();i++){
		TSystemFile *file=(TSystemFile*)list->At(i);
		TString path=file->GetName();
		if(file->IsDirectory() && path.BeginsWith("ss")){
			TList *files=((TSystemDirectory*)file)->GetListOfFiles();
			for(Int_t j=1;j<files->GetEntries();j++){
				file=(TSystemFile*)files->At(j);
				TString fileName=file->GetName();
				Char_t type=fileName[0];
				if((type=='S' || type=='C' || type=='D') && fileName.EndsWith("b1.dat")){
					fileName=fileName(1,fileName.Length());
					TString temp=fileName(0,fileName.Index('r'));
					Int_t series=temp.IsDigit()?temp.Atoi():-1;
					fileName=fileName(fileName.Index('r')+1,fileName.Length());
					temp=fileName(0,fileName.Index('b'));
					Int_t run=temp.IsDigit()?temp.Atoi():-1;
					temp.Form("%4i: %s/%c%ir%i",++count,path.Data(),type,series,run);
					cout<<temp<<endl;
					ThreadedAnalyzer *analyzer=new ThreadedAnalyzer(Task("",series,run,type,config_map[series]));
					multithread.add((Runnable*)analyzer);
				}
			}
			files->Delete();
		}
	}
	list->Delete();
	gSystem->ChangeDirectory(WORKDIR);
	multithread.run();
}

void crunchSeries(AnalyzerMode mode,Int_t series,Int_t N){
	if(config_map.empty()){
		TString path;
		path.Form("%s/detector_config.csv",RDKANAL);
		importConfig(path);
	}
	SysInfo_t sys;
	gSystem->GetSysInfo(&sys);
	const Int_t ncore=N>0?N:sys.fCpus+N;
	MultiThread multithread(ncore);
	TString dir;
	dir.Form("ss%i",series);
	if(!gSystem->IsAbsoluteFileName(dir)){
		dir.Form("%s/%s",RDKRAW,dir.Data());
	}
	TSystemDirectory d(".",dir);
	TList *list=d.GetListOfFiles();
	Int_t count=0;
	const TString suffix="b1.dat";
	for(Int_t i=0;i<list->GetEntries();i++){
		TSystemFile *file=(TSystemFile*)list->At(i);
		TString name=file->GetName();
		Char_t type=name[0];
		TString prefix;
		prefix.Form("%c%i",type,series);
		if((type=='S' || type=='C' || type=='D') && name.BeginsWith(prefix) && name.EndsWith(suffix)){
			name=name(0,name.Length()-suffix.Length());
			TString temp=name(1,name.Index("r")-1);
			Int_t series=temp.IsDigit()?temp.Atoi():-1;
			temp=name(name.Index("r")+1,name.Length());
			Int_t run=temp.IsDigit()?temp.Atoi():-1;
			ThreadedAnalyzer *reader=new ThreadedAnalyzer(Task("",series,run,type,config_map[series]));
			multithread.add((Runnable*)reader);
			count++;
			temp.Form("%4i: ss%i/%s",count,series,name.Data());
			cout<<temp<<endl;
		}
	}
	list->Delete();
	gSystem->ChangeDirectory(WORKDIR);
	multithread.run();
}

queue<Task> buildQueue(const Char_t* types){
	if(config_map.empty()){
		TString path;
		path.Form("%s/detector_config.csv",RDKANAL);
		importConfig(path);
	}
	queue<Task> tasks;
	try{
		vector<Int_t> series=RawTreeReader::getSeries(RDKRAW);
		for(Int_t i=0;i<series.size();i++){
			vector<Task> list=RawTreeReader::getRawList(series[i],types);
			for(Int_t j=0;j<list.size();j++){
				list[j].config=config_map[list[j].series];
				tasks.push(list[j]);
			}
		}
		gSystem->ChangeDirectory(WORKDIR);
	}catch(...){
		cerr<<"Unknown exception in buildQueue(const Char_t*)"<<endl;
	}
	return tasks;
}

queue<Task> buildQueue(Int_t nseries,const Char_t *types){
	if(config_map.empty()){
		TString path;
		path.Form("%s/detector_config.csv",RDKANAL);
		importConfig(path);
	}
	queue<Task> tasks;
	vector<Task> list=RawTreeReader::getRawList(nseries,types);
	for(Int_t j=0;j<list.size();j++){
		list[j].config=config_map[list[j].series];
		tasks.push(list[j]);
	}
	gSystem->ChangeDirectory(WORKDIR);
	return tasks;
}

void crunch(){
	if(config_map.empty()){
		TString path;
		path.Form("%s/detector_config.csv",RDKANAL);
		importConfig(path);
	}
	queue<Task> tasks;
	tasks.push(Task("ss256",256,19,'S',config_map[256]));
	crunch(tasks,kSmooth,1);
}

void crunch(queue<Task> list,Int_t N){
	try{
		cout<<"Checking list size"<<endl;
		if(list.empty()){
			cout<<"No files to crunch"<<endl;
			return;
		}else{
			cout<<"Number of files to crunch: "<<list.size()<<endl;
		}
		cout<<"Getting system info"<<endl;
		SysInfo_t sys;
		gSystem->GetSysInfo(&sys);
		cout<<"Setting number of cores: "<<flush;
		const Int_t ncore=N>0?N:sys.fCpus+N;
		cout<<ncore<<endl;
		if(ncore>1){
			MultiThread multithread(ncore);
			Int_t count=0;
			while(!list.empty()){
				Task task=list.front();
				cout<<setw(4)<<++count<<": "<<task<<endl;
				TString cmd;
				cmd.Form("nice -n 15 root -l \"tools/analyzerWrapper.cpp(\'%c\',%i,%i,%u)\" &> log/%c%ir%i.log",
					task.type,task.series,task.run,task.config,task.type,task.series,task.run);
				Runnable *runnable=new ExeWrapper(cmd);
				//Runnable *runnable=new ThreadedAnalyzer(task);
				multithread.add(runnable);
				list.pop();
			}
			gSystem->ChangeDirectory(WORKDIR);
			multithread.run();
		}else{
			Int_t count=0;
			while(!list.empty()){
				Task task=list.front();
				cout<<setw(4)<<++count<<": "<<task<<endl;
				ThreadedAnalyzer tanal(task);
				tanal.run();
				tanal.clean();
				list.pop();
			}
		}
	}catch(...){
		cerr<<"Unknown exception in crunch(queue<Task>,Int_t)"<<endl;
	}
}

void crunch(queue<Task> list,AnalyzerMode mode,Int_t N){
	crunchTemplate<ThreadedAnalyzer>(list,mode,N);
}

template <class T> void crunchTemplate(queue<Task> list,AnalyzerMode mode,Int_t N){
	cout<<"Checking list size"<<endl;
	if(list.empty()){
		cout<<"No files to crunch"<<endl;
		return;
	}else{
		cout<<"Number of files to crunch: "<<list.size()<<endl;
	}
	cout<<"Getting system info"<<endl;
	SysInfo_t sys;
	gSystem->GetSysInfo(&sys);
	cout<<"Setting number of cores: "<<flush;
	const Int_t ncore=N>0?N:sys.fCpus+N;
	cout<<ncore<<endl;
	MultiThread multithread(ncore);
	Int_t count=0;
	while(!list.empty()){
		Task task=list.front();
		cout<<setw(4)<<++count<<": "<<task<<endl;
		Runnable *runnable=new T(task);
		multithread.add(runnable);
		list.pop();
	}
	gSystem->ChangeDirectory(WORKDIR);
	multithread.run();
}

Double_t fitFunction(Double_t *x,Double_t *p){
	if(x[0]<p[4]){
		return p[0];
	}else{
		return p[0]+p[1]*Exp(-p[2]*(x[0]-p[4]))*(1-Exp(-p[3]*(x[0]-p[4])));
	}
}

void cooperAnalysis(const RawData &data,Int_t *t,Double_t &E,Double_t RATIO){
	Int_t T[5];
	for(Int_t i=0;i<5;i++){
		T[i]=0;
	}
	Double_t E_e=data.ch[15][0];
	for(Int_t i=256;i<768;i++){
		if(data.ch[15][i]>E_e){
			E_e=data.ch[15][i];
			T[2]=i;
		}
	}
	Double_t avg=0;
	Int_t n=0;
	for(Int_t i=0;i<2048;i++){
		if(fabs(fabs((Double_t)i-T[2])-512)<384){
			n++;
			avg+=data.ch[15][i];
		}
	}
	avg/=n;
	E=E_e-avg;
	Double_t threshold=avg+E*RATIO;
	Double_t half=avg+E/2;
	for(T[1]=T[2]-1;T[1]>0 && data.ch[15][T[1]]>half;T[1]--){}
	for(T[0]=T[1];T[0]>0 && data.ch[15][T[0]]>threshold;T[0]--){}
	for(T[3]=T[2]+1;T[3]<2048 && data.ch[15][T[3]]>half;T[3]++){}
	for(T[4]=T[3];T[4]<2048 && data.ch[15][T[4]]>threshold;T[4]++){}
	for(Int_t i=0;i<5;i++){
		t[i]=T[i];
	}
}

void cooperTest(RawFileReader *reader,TTree *tree,Double_t ratio){
	Int_t t[5];
	Double_t E;
	tree->SetBranchAddress("t_e",t);
	tree->SetBranchAddress("E_e",&E);
	Entry entry;
	RawData rdata;
	while(reader->readNext(entry,rdata)){
		cooperAnalysis(rdata,t,E,ratio);
		tree->Fill();
	}
	reader->close();
}

void calibrate(){
	cout<<"initializing"<<endl;
	RawTreeReader reader(TreeFilter::all);
	CalibrationList calibrations;
	float peak[14],sigma[14],chidof[14],sum[14];
	int series;
	cout<<"opening tfile"<<endl;
	TFile file(Form("%s/cal.root",RDKANAL),"recreate");
	cout<<"making ttree"<<endl;
	TTree *tree=new TTree("cal","cal");
	tree->Branch("series",&series,"series/I");
	tree->Branch("peak",&peak,"peak[14]/F");
	tree->Branch("sigma",&sigma,"sigma[14]/F");
	tree->Branch("chidof",&chidof,"chidof[14]/F");
	tree->Branch("sum",&sum,"sum[14]/F");
	cout<<"iterating over list of series"<<endl;
	for(Int_t i=0;i<RawTreeReader::n1400;i++){
		series=RawTreeReader::all1400[i];
		cout<<"getting list of runs for series "<<series<<endl;
		vector<Task> list=RawTreeReader::getList(series,"C",RDKANAL);
		for(Int_t in=0;in<list.size();in++){
			Int_t run=list[in].run;
			reader.reset();
			cout<<"opening file "<<series<<", "<<run<<endl;
			NData ndata=reader.openFile(list[in].type,series,run);
			cout<<"done"<<endl;
			while(in<list.size()-1 && list[in+1].run-list[in].run<=1){
				cout<<"opening file "<<series<<", "<<list[in+1].run<<endl;
				in++;
				reader.openFile(list[in].type,series,list[in].run);
				cout<<"done"<<endl;
			}
			cout<<"filling calibration histograms"<<endl;
			CalibrationData::fill(&reader);
			Char_t type=list[in].type;
			cout<<"calibrating"<<endl;
			CalibrationData *data=CalibrationData::calibrate(type,series,run,ndata.start);
			cout<<"adding to list"<<endl;
			calibrations.Add(data);
			cout<<"filling tree"<<endl;
			if(data){
				for(int j=0;j<14;j++){
					peak[j]=data->getPeak(j);
					sigma[j]=data->getSigma(j);
					chidof[j]=data->getChiDof(j);
					sum[j]=data->getSum(j);
				}
				tree->Fill();
			}
		}
	}
	cout<<"logging resulst"<<endl;
	ofstream log(Form("%s/cal.csv",RDKANAL));
	log<<calibrations<<flush;
	log.close();
	file.WriteTObject(tree);
	file.WriteTObject(&calibrations);
	file.Close();
	gSystem->ChangeDirectory(WORKDIR);
}

void updateNdat(){
	queue<Task> q=buildQueue("SCD");
	while(!q.empty()){
		Task task=q.front();
		TFile *f=new TFile(Form("%s/ss%i/%c%ir%i.root",RDKANAL,
			task.series,task.type,task.series,task.run),"update");
		NdatWriter writer(task.type,task.series,task.run,0,f);
		writer.write();
		f->Close();
		delete f;
		q.pop();
	}
	gSystem->ChangeDirectory(WORKDIR);
}

void exportTimestamps(Char_t t,Int_t s,Int_t r){
	TFile f(Form("%s/second_pass/ss%i/%c%i.root",RDKANAL,s,t,s));
	TTree *tree=(TTree*)f.Get("rdk_second_pass");
	epEvent event;
	tree->SetBranchAddress("epEvent",(Double_t*)&event);
	ofstream out(Form("%s/second_pass/ss%i/%c%i.csv",RDKANAL,s,t,s));
	out<<"#,nrun,nevent0,nevent1,timestamp0,timestamp1"<<endl;
	for(Int_t i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		if(event.nrun==r){
			out<<i<<','<<event.nrun<<','<<event.nevent0<<','<<event.nevent1;
			out<<','<<event.timestamp[0]<<','<<event.timestamp[1]<<endl;
		}
	}
	out.close();
}

template class vector<Task>;

void crunchFromFile(const Char_t *fileName,const Char_t *types){
	ifstream in(fileName);
	TPRegexp regex("ss\\d+"),digit("\\d+");
	TString line;
	vector<Task> list;
	while(in.good()){
		line.ReadLine(in);
		line=TString(TString(line(regex))(digit));
		Int_t series=line.Atoi();
		queue<Task> temp=buildQueue(series,types);
		while(!temp.empty()){
			Task t=temp.front();
			cout<<list.size()<<'\t'<<t<<endl;
			list.push_back(t);
			temp.pop();
		}
	}
	for(Int_t i=0;i<list.size();i++){
		ThreadedAnalyzer anal(list[i]);
		anal.run();
	}
}

queue<Task> queueFromFile(const Char_t *fileName,const Char_t *types){
	ifstream in(fileName);
	TPRegexp regex("ss\\d+"),digit("\\d+");
	TString line;
	queue<Task> list;
	while(in.good()){
		line.ReadLine(in);
		line=TString(TString(line(regex))(digit));
		Int_t series=line.Atoi();
		queue<Task> temp=buildQueue(series,types);
		while(!temp.empty()){
			list.push(temp.front());
			temp.pop();
		}
	}
	return list;
}

void second_pass(TString runlist,Bool_t filter){
	RDKConfig config(Form("%s/detector_config.csv",RDKANAL));
	vector<Int_t> series=RawTreeReader::getSeries(RDKANAL);
	if(runlist.EqualTo("")){
		runlist.Form("%s/runlist.csv",RDKANAL);
	}
	for(Int_t i=0;i<series.size();i++){
		cout<<"Series "<<series[i]<<endl;
		SecondPass sp(runlist,'S',series[i],config.getConfig(series[i]));
		sp.fill(filter);
	}
}

void createEpCal(){
	FileStat_t stats;
	TString path;
	path.Form("%s/epCals/",RDKANAL);
	TSystemDirectory dir(".",path);
	TList *list=dir.GetListOfFiles();
	ofstream output("epcal.csv");
	output<<"Series,eEndPt"<<endl;
	for(Int_t i=0;i<list->GetEntries();i++){
		TSystemFile *file=(TSystemFile*)list->At(i);
		if(!file->IsDirectory()){
			TPRegexp regex("\\d+[a-z]?"),number("\\d+.?\\d*");
			TString fileName=file->GetName();
			ifstream input(Form("%s/%s",path.Data(),fileName.Data()));
			TString buffer;
			buffer.ReadLine(input);
			buffer.ReadLine(input);
			buffer.ReadLine(input);
			buffer.ReadLine(input);
			//buffer=buffer(number);
			//Double_t peak=buffer.Atof();
			TString series=fileName(regex,8);
			output<<series<<","<<buffer<<endl;
		}
	}
}

TGraph *graphMatt(TString path){
	ifstream infile(path);
	TString buffer;
	buffer.ReadLine(infile);
	vector<Double_t> binCenter,binContent,binError;
	while(infile.good() && !infile.eof()){
		buffer.ReadLine(infile);
		TStringToken token(buffer,"\t");
		token.NextToken();
		binCenter.push_back(token.Atof());
		token.NextToken();
		binContent.push_back(token.Atof());
		token.NextToken();
		binError.push_back(token.Atof());
	}
	Int_t size=binCenter.size();
	TVectorD x(size),y(size);
	for(Int_t i=0;i<size;i++){
		x[i]=binCenter[i];
		y[i]=binContent[i];
	}
	return new TGraph(x,y);
}

TGraphErrors *egraphMatt(TString path){
	ifstream infile(path);
	TString buffer;
	buffer.ReadLine(infile);
	vector<Double_t> binCenter,binContent,binError;
	while(infile.good() && !infile.eof()){
		buffer.ReadLine(infile);
		TStringToken token(buffer,"\t");
		token.NextToken();
		binCenter.push_back(token.Atof());
		token.NextToken();
		binContent.push_back(token.Atof());
		token.NextToken();
		binError.push_back(token.Atof());
	}
	Int_t size=binCenter.size();
	TVectorD x(size),y(size),ex(size),ey(size);
	for(Int_t i=0;i<size;i++){
		x[i]=binCenter[i];
		y[i]=binContent[i];
		ex[i]=0;
		ey[i]=binError[i];
	}
	return new TGraphErrors(x,y,ex,ey);
}

TTree* openAllTrees(){
	TChain *chain=new TChain("rdk_second_pass");
	Int_t series[47]={76,77,78,79,87,89,91,93,94,95,104,105,106,119,120,126,127,151,153,155,156,160,161,162,165,166,175,176,185,187,215,223,227,228,253,257,270,289,290,291,292,304,305,311,313,314,317};
	for(Int_t i=0;i<47;i++){
		TString path;
		path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,series[i],series[i]);
		chain->Add(path);
	}
	return chain;
}

TTree* openSet1(){
	TChain *chain=new TChain("rdk_second_pass");
	Int_t series[11]={227,228,253,257,270,289,290,291,292,304,305};
	for(Int_t i=0;i<11;i++){
		TString path;
		path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,series[i],series[i]);
		chain->Add(path);
	}
	return chain;
}

TTree* openSet2(){
	TChain *chain=new TChain("rdk_second_pass");
	Int_t series[12]={76,77,78,79,87,89,91,93,94,95,215,223};
	for(Int_t i=0;i<12;i++){
		TString path;
		path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,series[i],series[i]);
		chain->Add(path);
	}
	return chain;
}

TTree* openSet2a(){
	TChain *chain=new TChain("rdk_second_pass");
	Int_t series[10]={76,77,78,79,87,89,91,93,94,95};
	for(Int_t i=0;i<10;i++){
		TString path;
		path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,series[i],series[i]);
		chain->Add(path);
	}
	return chain;
}

TTree* openSet2b(){
	TChain *chain=new TChain("rdk_second_pass");
	Int_t series[2]={215,223};
	for(Int_t i=0;i<2;i++){
		TString path;
		path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,series[i],series[i]);
		chain->Add(path);
	}
	return chain;
}

TTree* openSet3(){
	TChain *chain=new TChain("rdk_second_pass");
	Int_t series[20]={104,105,106,119,120,126,127,151,153,155,156,160,161,162,165,166,175,176,185,187};
	for(Int_t i=0;i<20;i++){
		TString path;
		path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,series[i],series[i]);
		chain->Add(path);
	}
	return chain;
}

TTree* openSet4(){
	TChain *chain=new TChain("rdk_second_pass");
	Int_t series[4]={311,313,314,317};
	for(Int_t i=0;i<4;i++){
		TString path;
		path.Form("%s/second_pass/ss%i/S%i.root",RDKANAL,series[i],series[i]);
		chain->Add(path);
	}
	return chain;
}

void exportTree(TTree *tree,TString path){
	std::ofstream out(path);
	epEvent epevent;
	tree->SetBranchAddress("epEvent",(Double_t*)&epevent);
	EpFilter filter;
	//preamble
	out<<"Run,event #,# on board 0,timestamp 0,# on board 1,timestamp 1,";
	//ep
	out<<"t_e,E_e,sigma_e,t_p,E_p,sigma_p,t_q,E_q,chi2_ep,t_half,t_dip";
	//photon
	for(Int_t i=0;i<14;i++){
		Int_t brd=i/7,ch=i%7;
		out<<",t_gamma "<<brd<<ch<<",E_gamma "<<brd<<ch<<",M "<<brd<<ch<<",Group "<<brd<<ch;
		out<<",chi2_gamma "<<brd<<ch<<",t_rise "<<brd<<ch;
		out<<",A "<<brd<<ch<<",B "<<brd<<ch<<",C "<<brd<<ch<<",D "<<brd<<ch;
	}
	//ep flags
	out<<",ep flags"<<endl;
	for(Int_t i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		epevent.nevent=i;
		out<<epevent<<",0x"<<hex<<filter.getStatus(epevent)<<dec<<endl;
	}
}

 void importJeffData(TString source,TString dest){
	 epEvent epevent;
	 TFile f(dest,"recreate");
	 TTree *tree=new TTree("rdk_jsn","rdk_jsn");
	 tree->Branch("epEvent",(Double_t*)&epevent,epEvent::BRANCH);
	 ifstream in(source);
	 string buff;
	 getline(in,buff,'\r');
	 while(!in.eof()){
		 getline(in,buff,'\r');
		 parseJeffLine(buff,epevent);
		 tree->Fill();
	 }
	 f.WriteTObject(tree,"rdk_jsn");
	 f.Close();
}

 void parseJeffLine(TString line,epEvent &epevent){
	 TStringToken token(line,",",kTRUE);
	 token.NextToken(); //series
	 epevent.nseries=token.Atoi();
	 token.NextToken(); //run
	 epevent.nrun=token.Atoi();
	 token.NextToken(); //board (seems to always be 1)
	 token.NextToken();	//event
	 epevent.nevent=token.Atoi();
	 token.NextToken(); //TimeStamp0
	 epevent.timestamp[0]=150*token.Atoll();
	 token.NextToken(); //TimeStamp1
	 epevent.timestamp[1]=150*token.Atoll();
	 token.NextToken(); //e_PkCh
	 epevent.t_e=0.04*token.Atof();
	 token.NextToken(); //e_PkHt
	 token.NextToken(); //e_En
	 epevent.E_e=token.Atof();
	 token.NextToken(); //e_width
	 epevent.sigma_e=token.Atof();
	 token.NextToken(); //e_chi
	 token.NextToken(); //p_PkCh
	 epevent.t_p=0.04*token.Atof();
	 token.NextToken(); //p_PkHt
	 token.NextToken(); //p_En
	 epevent.E_p=token.Atof();
	 token.NextToken(); //p_chi
	 epevent.sigma_p=token.Atof();
	 token.NextToken(); //ep_DeltaT
	 token.NextToken(); //p2_PkCh
	 epevent.t_q=0.04*token.Atof();
	 token.NextToken(); //p2_PkHt
	 token.NextToken(); //p2_En
	 epevent.E_q=token.Atof();
	 token.NextToken(); //p2_chi
	 token.NextToken(); //ep2_DeltaT
	 token.NextToken(); //ep_usercut
	 token.NextToken(); //PMult
	 token.NextToken(); //bMult
	 token.NextToken(); //Mult
	 token.NextToken(); //ETot
	 for(Int_t i=0;i<14;i++){
		 token.NextToken(); //PkCh##
		 epevent.t_gamma[i]=0.04*token.Atof();
		 token.NextToken(); //PkHt##
		 token.NextToken(); //gArea##
		 epevent.E_gamma[i]=token.Atof();
		 token.NextToken(); //DeltaT##
		 token.NextToken(); //Chi_DoF##
	 }
 }