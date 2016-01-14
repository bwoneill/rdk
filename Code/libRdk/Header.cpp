//Header.cpp

#include "Header.h"
#include "TObjString.h"

#include <iostream>

using namespace std;

Header::Header(){
	isOwner=kTRUE;
	map=new TMap();
	initialize();
}

Header::Header(TMap *m){
	isOwner=kFALSE;
	map=m;
	TMap *temp=(TMap*)map->GetValue("Header");
	initialize();
}

Header::~Header(){
	if(isOwner){
		delete map;
	}
}

void Header::initialize(){
	addSection("Header");
	addSection("Footer");
}

void Header::parseString(TString input,TString &section,TString &key){
	if(input.Contains(".")){
		Int_t i=input.Index(".");
		section=input(0,i);
		key=input(i+1,input.Length());
	}else{
		section=input;
		key="";
	}
}

void Header::addSection(TString name){
	TString section,key;
	parseString(name,section,key);
	TMap *m=getSection(section);
	if(m==NULL){
		m=new TMap();
		m->SetOwnerValue();
		map->Add(new TObjString(section),m);
	}
}

TMap* Header::getSection(TString input) const{
	TString section,key;
	parseString(input,section,key);
	return (TMap*)map->GetValue(section);
}

TString& Header::getValue(TString input) const{
	TString section,key;
	parseString(input,section,key);
	TMap *s=(TMap*)map->GetValue(section);
	if(s!=NULL){
		TObjString *v=(TObjString*)s->GetValue(key);
		if(v!=NULL){
			return v->String();
		}
	}
	//static TString value="";
	return *(new TString());
}

Int_t Header::setValue(TString name,TString value){
	TString section,key;
	parseString(name,section,key);
	TMap *s=getSection(section);
	if(s==NULL){
		return -1;
	}
	TObjString *obj=(TObjString*)s->GetValue(key);
	if(obj!=NULL){
		obj->SetString(value);
		return 1;
	}else{
		s->Add(new TObjString(key),new TObjString(value));
		return 0;
	}
}

Int_t Header::appendValue(TString name,TString value){
	TString section,key;
	parseString(name,section,key);
	TMap *s=getSection(section);
	if(s==NULL){
		return -1;
	}
	TObjString *obj=(TObjString*)s->GetValue(key);
	if(obj!=NULL){
		obj->SetString(obj->String()+value);
		return 1;
	}else{
		s->Add(new TObjString(key),new TObjString(value));
		return 0;
	}
}

vector<TString> Header::getListOfSections() const{
	vector<TString> list;
	TMapIter iter(map);
	TObjString *o;
	while(o=(TObjString*)iter.Next()){
		list.push_back(o->String());
	}
	return list;
}

vector<TString> Header::getListOfKeys(TString s) const{
	vector<TString> list;
	TMap *section=(TMap*)map->GetValue(s);
	if(section!=NULL){
		TMapIter iter(section);
		TObjString *o;
		while(o=(TObjString*)iter.Next()){
			list.push_back(o->String());
		}
	}
	return list;
}

void Header::printTree() const{
	TMapIter iter1(map);
	TObjString *o;
	while(o=(TObjString*)iter1.Next()){
		cout<<o->String()<<endl;
		TMap *temp=(TMap*)map->GetValue(o);
		TMapIter iter2(temp);
		TObjString *tempo;
		while(tempo=(TObjString*)iter2.Next()){
			TObjString *value=(TObjString*)temp->GetValue(tempo);
			cout<<"-"<<tempo->String()<<" = "<<value->String()<<endl;
		}
	}
}