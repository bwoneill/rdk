//ThreadTest.cpp

#include "ThreadTest.h"
#include "TString.h"
#include "TRandom1.h"
#include "TStopwatch.h"

#include <iostream>
using namespace std;

typedef TThread::VoidRtnFunc_t VoidRtnFunc_t;

ThreadTest::ThreadTest(Int_t n):N(n),s1(N),s2(0){
}

ThreadTest::~ThreadTest(){
}

void* ThreadTest::exec(void *arg){
	TRandom1 rand;
	Double_t list[1000];
	list[rand.Integer(1000)]=1;
	ThreadTest *test=(ThreadTest*)arg;
	TThread *self=TThread::Self();
	test->s1.Wait();
	test->mutex.Lock();
	Int_t n=test->idx++;
	test->mutex.UnLock();
	if(test->readNext(n)){
		TThread *next=new TThread((VoidRtnFunc_t)exec,arg,TThread::kLowPriority);
		test->q.AddLast(next);
		next->Run();
		test->analyze(n);
		TThread *prev=(TThread*)test->q.Before(self);
		if(prev){
			prev->Join();
		}
		test->write(n);
	}
	test->mutex.Lock();
	test->q.Remove(self);
	if(!test->q.First()){
		test->s2.Post();
		TThread::Printf("posting");
	}
	test->mutex.UnLock();
	test->s1.Post();
	self->Delete();
	return NULL;
}

Bool_t ThreadTest::readNext(Int_t n){
	if(n<10){
		TThread::Printf("reading %i in thread 0x%x",n,TThread::SelfId());
	}else{
		TThread::Printf("no more to read in thread 0x%x",TThread::SelfId());
	}
	return n<10;
}

void ThreadTest::analyze(Int_t n){
	TRandom1 rand;
	TThread::Sleep(0,100*rand.Integer(100));
	TThread::Printf("analyzing %i in thread 0x%x",n,TThread::SelfId());
}

void ThreadTest::write(Int_t n){
	TThread::Printf("writing %i in thread 0x%x",n,TThread::SelfId());
}

void ThreadTest::run(Bool_t wait){
	idx=0;
	TThread *first=new TThread((VoidRtnFunc_t)exec,this,TThread::kLowPriority);
	q.Clear();
	q.AddLast(first);
	TThread *last=(TThread*)q.Last();
	first->Run();
	while(last && wait){
		last->Join();
		last=(TThread*)q.Last();
	}
	//s2.Wait();
	TThread::Printf("done");
}