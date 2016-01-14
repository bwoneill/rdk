//TreeFilter.cpp

#include "TreeFilter.h"
using namespace std;

TreeFilter::TreeFilter(Double_t hvt,Double_t sp,Double_t fl){
	HVT=hvt;
	spark=sp;
	flux=fl;
}

TreeFilter::~TreeFilter(){
}

Bool_t TreeFilter::valid(const NData &ndata,ostream &log) const{
	Double_t nn=ndata.nn,t=ndata.t,nep=ndata.nep;
	Bool_t result=kTRUE;
	if(HVT!=0 || spark!=0 || flux!=0){
		if(nn<=0 || t<=0){
			result=kFALSE;
			log<<"No neutrons or zero run time"<<endl;
		}
		if(nn/t<=flux){
			result=kFALSE;
			log<<"Low neutron flux: n/t="<<nn/t*1e3<<"Hz<="<<flux*1e3<<"Hz"<<endl;
		}
		if(nep/nn<=HVT){
			result=kFALSE;
			log<<"HVT: ep/n="<<nep/nn<<"<="<<HVT<<endl;
		}
		if(nep/nn>=spark){
			result=kFALSE;
			log<<"Unknown ep source, possible spark: ep/n="<<nep/nn<<">="<<spark<<endl;
		}
	}
	return result;
}

const TreeFilter *TreeFilter::all=new TreeFilter(0,0,0);
const TreeFilter *TreeFilter::std=new TreeFilter();