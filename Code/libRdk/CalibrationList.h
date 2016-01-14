//CalibrationList.h

#ifndef CALIBRATIONLIST_H
#define CALIBRATIONLIST_H

#include "RDKTypes.h"
#include "TSortedList.h"
#include <ostream>

class CalibrationData;

class CalibrationList: public TSortedList{
private:
	TObjLink *current;	//!
public:
	CalibrationList();
	CalibrationList(TString);
	~CalibrationList();
	CalibrationData *getCal(Int_t,Int_t);
	CalibrationData *getCal(const Entry&);
	CalibrationData *getCal(const std::pair<Int_t,Int_t>&);
	Recon calibrate(const Entry&,const Recon&);
	ClassDef(CalibrationList,1)
};

std::ostream& operator<<(std::ostream&,const CalibrationList&);

#endif