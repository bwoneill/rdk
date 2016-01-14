//CalibrationData.h

#ifndef CALIBRATIONDATA_H
#define CALIBRATIONDATA_H

#include "RDKTypes.h"
#include "TObject.h"
#include <utility>
#include <string>
#include <ostream>

class TH1;
class TF1;
class TBits;
class RawTreeReader;

class CalibrationData: public TObject{
private:
	std::string file;
	Int_t run,series;
	Long64_t start;
	Float_t fit[14][4];
	UInt_t config;
	void parseString(const Char_t*);
public:
	CalibrationData();
	CalibrationData(const Char_t*);
	CalibrationData(Char_t,Int_t,Int_t,Long64_t);
	CalibrationData(Char_t,Int_t,Int_t,Long64_t,const Float_t**);
	~CalibrationData();
	std::string getFile() const{return file;}
	Int_t getRun() const {return run;}
	Int_t getSeries() const {return series;}
	Long64_t getStartDate() const {return start;}
	Float_t getPeak(Int_t i) const {return fit[i][0];}
	Float_t getSigma(Int_t i) const {return fit[i][1];}
	Float_t getChiDof(Int_t i) const {return fit[i][2];}
	Float_t getSum(Int_t i) const {return fit[i][3];}
	std::string toString()const;
	void calibrate(Recon&)const;
	Int_t Compare(const TObject*) const;
	Bool_t IsSortable() const {return kTRUE;}
	Bool_t IsEqual(const TObject* obj) const {return Compare(obj)==0;}
	Bool_t operator>(const std::pair<Int_t,Int_t>&)const;
	Bool_t operator>=(const std::pair<Int_t,Int_t>&)const;
	Bool_t operator==(const std::pair<Int_t,Int_t>&)const;
	Bool_t operator<=(const std::pair<Int_t,Int_t>&)const;
	Bool_t operator<(const std::pair<Int_t,Int_t>&)const;
	static TH1 **hist;
	static TF1 *bg_gaus;
	static Int_t res;
	static TBits mode;
	static CalibrationData* calibrate(Char_t,Int_t,Int_t,Long64_t);
	static void initCalibration();
	static void fill(RawTreeReader*);
	friend std::ostream& operator<<(std::ostream&,const CalibrationData&);
	ClassDef(CalibrationData,1)
};

std::ostream& operator<<(std::ostream&,const CalibrationData&);

#endif