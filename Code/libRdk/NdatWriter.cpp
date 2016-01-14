//NdatWriter.cpp

#include "NdatWriter.h"
#include "Threads.h"

#include "TSystem.h"
#include "TThread.h"
#include "TFile.h"
#include "TTree.h"

NdatWriter::NdatWriter(Char_t c,Int_t s,Int_t r,Double_t t, TFile *f):
		type(c),nseries(s),nrun(r),start(t),file(f),closeAtEnd(file==NULL){
	reader=new NdatReader(c,s,r);
	ndat=NULL;
	Ndat=NULL;
}

NdatWriter::~NdatWriter(){
	delete ndat;
	delete Ndat;
	if(reader){
		delete reader;
	}
}

void NdatWriter::write(){
	try{
		if(nseries<=0 || nrun<0){
			throw RDKException("Invalid series or run");
		}
		if(!file){
			TString temp;
			temp.Form("%s/ss%i/%c%ir%i.root",RDKANAL,nseries,type,nseries,nrun);
			file=TFile::Open(temp,"update");
		}
		TString temp;
		temp.Form("rawndat-%c%ir%i",type,nseries,nrun);
		ndat=new TTree(temp,"RDK rawndat");
		ndat->Branch("ndata",&ndata,"start/D:nn00/L:nn01:nn10:nn11:nn20:nn21:nn30:nn31:nn:ne:np:nep:t:series/I:run");
		temp.Form("ndat-%c%ir%i",type,nseries,nrun);
		Ndat=new TTree(temp,"RDK ndat");
		Ndat->Branch("ndata",&total,"start/D:nn00/L:nn01:nn10:nn11:nn20:nn21:nn30:nn31:nn:ne:np:nep:t:series/I:run");
		ndata.series=nseries;
		ndata.run=nrun;
		while(reader->readNext(ndata)){
			ndata.start=start;
			total+=ndata;
			ndat->Fill();
		}
		Ndat->Fill();
		file->WriteTObject(ndat,"rawndat","overwrite");
		file->WriteTObject(Ndat,"ndat","overwrite");
		delete ndat;
		delete Ndat;
		if(closeAtEnd){
			file->Close();
		}
		delete reader;
		reader=NULL;
		ndat=NULL;
		Ndat=NULL;
	}catch(RDKException e){
		TString temp;
		temp.Form("Fatal error in ss%i/%c%ir%ib1.ndat",nseries,type,nseries,nrun);
		TThread::Printf(temp);
		TThread::Printf(e.what());
		if(closeAtEnd && file){
			file->Close();
		}
		if(reader){
			delete reader;
			reader=NULL;
		}
		throw RDKException("NdatWriter error");
	}
}