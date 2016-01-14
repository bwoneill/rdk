//ThreadTest.h

#ifndef THREADTEST_H
#define THREADTEST_H

#include "TThread.h"
#include "TMutex.h"
#include "TSemaphore.h"
#include "TList.h"

#include <vector>
#include <queue>

class ThreadTest{
private:
	const Int_t N;
	Int_t idx;
	TSemaphore s1,s2;
	TMutex mutex;
	TList q;
	static void* exec(void*);
	Bool_t readNext(Int_t);
	void analyze(Int_t);
	void write(Int_t);
public:
	ThreadTest(Int_t);
	~ThreadTest();
	void run(Bool_t=kFALSE);
};

#endif