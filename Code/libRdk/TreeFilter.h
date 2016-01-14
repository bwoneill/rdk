//TreeFilter.h

#ifndef TREEFILTER_H
#define TREEFILTER_H

#include "RDKTypes.h"
#include <iostream>

class TreeFilter{
private:
	Double_t HVT,spark,flux;
public:
	TreeFilter(Double_t=4.2e-3,Double_t=1,Double_t=0.7);
	~TreeFilter();
	Bool_t valid(const NData&,std::ostream& =std::clog) const;
	static const TreeFilter *all,*std;
};

#endif