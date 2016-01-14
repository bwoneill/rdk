//threadTreeTest.cpp
#include "TFile.h"
#include "TTree.h"
#include "TThread.h"

void testFunction(){
	TThread::Printf("Making root file");
	TFile *f=TFile::Open("test.root","recreate");
	TThread::Printf("Making tree");
	TTree *t=new TTree("test","test");
	Int_t value=1;
	TThread::Printf("Making branch");
	t->Branch("value",&value,"value/I");
	TThread::Printf("Filling tree");
	t->Fill();
	TThread::Printf("Writing tree");
	f->WriteTObject(t);
	TThread::Printf("Closing file");
	f->Close();
	TThread::Printf("Done");
}

void threadTreeTest(){
	TThread *th=new TThread("testThread",(void(*)(void *))&testFunction);
	th->Run();
}