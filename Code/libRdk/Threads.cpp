//Threads.cpp

#include "Threads.h"
#include <iostream>
using namespace std;

TMutex *fileLock=new TMutex();

ThreadRunner::ThreadRunner(TString name,Runnable *app){
	this->app=app;
	thread=new TThread(name,(void(*)(void *))&exec,(void *)this);
	thread->SetPriority(TThread::kLowPriority);
}

ThreadRunner::~ThreadRunner(){
	TThread::Delete(thread);
	delete thread;
}

Int_t ThreadRunner::run(Runnable *task){
	if(task!=NULL){
		app=task;
	}
	if(app!=NULL){
		return thread->Run((void*)this);
	}else{
		return -1;
	}
}

void* ThreadRunner::exec(void *arg){
	TThread::CleanUpPush((void *)&clean,arg);
	TThread::SetCancelOn();
	TThread::SetCancelDeferred();
	if(arg!=NULL){
		ThreadRunner *runner=(ThreadRunner*)arg;
		runner->app->run();
	}
	return NULL;
}

void* ThreadRunner::clean(void *arg){
	if(arg!=NULL){
		ThreadRunner *runner=(ThreadRunner*)arg;
		runner->app->clean();
		runner->thread->Delete();
	}
	return NULL;
}

ThreadQueue::ThreadQueue(TString name){
	thread=new TThread(name,(void(*)(void *))&exec,(void *)this);
}

Int_t ThreadQueue::run(){
	return thread->Run();
}

void* ThreadQueue::exec(void *arg){
	TThread::CleanUpPush((void *)&clean,arg);
	TThread::SetCancelOn();
	TThread::SetCancelDeferred();
	ThreadQueue *thread=(ThreadQueue*)arg;
	while(!thread->tasks.empty()){
		Runnable *task=thread->tasks.front();
		thread->tasks.pop();
		task->run();
		if(task==thread->cleanup.front()){
			thread->cleanup.pop();
			task->clean();
			delete task;
		}
		//if(!thread->tasks.empty()){
		//	TThread::Sleep(0,1000);
		//}
	}
	return NULL;
}

void *ThreadQueue::clean(void *arg){
	if(arg!=NULL){
		ThreadQueue *runner=(ThreadQueue*)arg;
		while(!runner->cleanup.empty()){
			Runnable *task=runner->cleanup.front();
			runner->cleanup.pop();
			task->clean();
			delete task;
		}
		runner->thread->Delete();
	}
	return NULL;
}

SyncThread::SyncThread(TString name,MultiThread *multi){
	this->app=NULL;
	this->multi=multi;
	thread=new TThread(name,(void(*)(void *))&exec,(void *)this);
	thread->SetPriority(TThread::kLowPriority);
	mutex=new TMutex();
}

SyncThread::~SyncThread(){
	if(app!=NULL){
		app->clean();
		delete app;
	}
	thread->Delete();
	mutex->Delete();
}

void SyncThread::run(){
	//TThread::Printf("Start run");
	//cout<<multi->tasks.size()<<endl;
	thread->Run();
}

void* SyncThread::exec(void *arg){
	try{
		//TThread::Printf("Starting synchronized thread");
		SyncThread *runner=(SyncThread*)arg;
		//TThread::Printf("Getting runnable");
		Runnable *app=runner->app;
		//TThread::Printf("Getting parrent");
		MultiThread *multi=runner->multi;
		//TThread::Printf(runner->thread->GetName());
		//TThread::Printf("Start loop");
		while(true){
			runner->mutex->Lock();
			//TThread::Printf("Locked");
			if(!multi->tasks.empty()){
				//TThread::Printf("Getting task");
				app=runner->multi->tasks.front();
				multi->tasks.pop();
			}else{
				//TThread::Printf("No more tasks");
				app=NULL;
			}
			//TThread::Printf("Unlocking");
			runner->mutex->UnLock();
			if(app!=NULL){
				TThread::SetCancelOn();
				TThread::SetCancelDeferred();
				if(arg!=NULL){
					//TThread::Printf("Running...");
					app->run();
					//TThread::Printf("Cleaning...");
					app->clean();
					//TThread::Printf("Done");
					//delete app;
					//app=NULL;
				}
			}else{
				//TThread::Printf("Finishing");
				delete runner;
				return NULL;
			}
		}
	}catch(...){
		TThread::Printf("Uncaught exception in thread %i",TThread::SelfId());
	}
	return NULL;
}

void* SyncThread::clean(void *arg){
	if(arg!=NULL){
		delete arg;
	}
	return NULL;
}

MultiThread::MultiThread(Int_t n):threads(n){
	mutex=new TMutex();
	TString temp;
	for(Int_t i=0;i<n;i++){
		temp.Form("core%i",i);
		threads[i]=new SyncThread(temp,this);
	}
}

MultiThread::~MultiThread(){
	delete mutex;
}

void MultiThread::add(Runnable *task){
	mutex->Lock();
	tasks.push(task);
	mutex->UnLock();
}

void MultiThread::run(){
	for(Int_t i=0;i<threads.size();i++){
		threads[i]->run();
	}
}

queue<Runnable*> MultiThread::tasks;