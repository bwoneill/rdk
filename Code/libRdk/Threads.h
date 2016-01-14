//Threads.h

#ifndef THREADRUNNER_H
#define THREADRUNNER_H

#include "Runnable.h"

#include <queue>
#include <vector>
using std::queue;
using std::vector;

#include "TThread.h"
#include "TMutex.h"
#include "TSemaphore.h"

extern TMutex *fileLock;

class ThreadRunner{
	private:
		Runnable *app;
		TThread *thread;
	public:
		ThreadRunner(TString,Runnable* =NULL);
		~ThreadRunner();
		static void* exec(void*);
		static void* clean(void*);
		Int_t run(Runnable* =NULL);
		inline TThread* getThread() const {return thread;}
		inline TThread::EState getState() const {return thread->GetState();}
};

class ThreadQueue{
	private:
		queue<Runnable*> tasks,cleanup;
		TThread *thread;
	public:
		ThreadQueue(TString);
		static void* exec(void*);
		static void* clean(void*);
		inline void add(Runnable *task){tasks.push(task);cleanup.push(task);}
		Int_t run();
};

class MultiThread;

class SyncThread{
	private:
		TMutex *mutex;
		MultiThread *multi;
		Runnable *app;
		TThread *thread;
	public:
		SyncThread(TString,MultiThread*);
		~SyncThread();
		static void* exec(void*);
		static void* clean(void*);
		void run();
		inline void setApp(Runnable *app){this->app=app;}
		inline TThread* getThread() const {return thread;}
		inline TThread::EState getState() const {return thread->GetState();}
};

class MultiThread{
	private:
		TMutex *mutex;
		static queue<Runnable*> tasks;
		vector<SyncThread*> threads;
	public:
		MultiThread(Int_t);
		~MultiThread();
		void add(Runnable *task);
		void run();
		friend class SyncThread;
};

#endif