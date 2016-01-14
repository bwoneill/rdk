//calibrate.cpp

#include <vector>
#include <fstream>
#include <string>
using namespace std;

void calibrate(){
	CalibrationData::mode.SetBitNumber(2);
	CalibrationData::mode.SetBitNumber(6);
	CalibrationData::mode.SetBitNumber(13);
	//TreeReader reader(TreeFilter::all);
	CalibrationList calibrations;
	float peak[14],sigma[14],chidof[14],sum[14];
	int series;
	TFile file(Form("%s/cal.root",RDKANAL),"recreate");
	TTree *tree=new TTree("cal","cal");
	tree->Branch("series",&series,"series/I");
	tree->Branch("peak",&peak,"peak[14]/F");
	tree->Branch("sigma",&sigma,"sigma[14]/F");
	tree->Branch("chidof",&chidof,"chidof[14]/F");
	tree->Branch("sum",&sum,"sum[14]/F");
	for(Int_t i=0;i<TreeReader::n1400;i++){
		series=TreeReader::all1400[i];
		vector<Int_t> list=TreeReader::getList(series,"CD",RDKANAL);
		for(Int_t in=0;in<list.size();in++){
			Int_t run=list[in];
			TreeReader reader(TreeFilter::all);
			//reader.reset();
			NData ndata=reader.openFile(series,run);
			while(in<list.size()-1 && list[in+1]-list[in]<=1){
				reader.openFile(series,list[++in]);
			}
			CalibrationData::fill(&reader);
			Char_t type=TreeReader::getAnalyzedType(series,run);
			CalibrationData *data=CalibrationData::calibrate(type,series,run,ndata.start);
			calibrations.Add(data);
			if(data){
				for(int j=0;j<14;j++){
					peak[j]=data->getPeak(j);
					sigma[j]=data->getSigma(j);
					chidof[j]=data->getChiDof(j);
					sum[j]=data->getSum(j);
				}
				tree->Fill();
			}
		}
	}
	ofstream log(Form("%s/cal.csv",RDKANAL));
	log<<calibrations<<flush;
	log.close();
	file.WriteTObject(tree);
	file.WriteTObject(&calibrations);
	file.Close();
	//log.open(Form("%s/comp.csv",RDKANAL));
	//log<<"File,Ratio_11,Ratio_12,Ratio_13,Ratio_14,Ratio_15,Ratio_16,";
	//log<<"Ratio_21,Ratio_22,Ratio_23,Ratio_24,Ratio_25,Ratio_26"<<endl;
	//CalibrationList herbsCal(Form("%s/CAL_TEMPERA_TEMPORA.csv",RDKANAL));
	//for(Int_t i=0;i<calibrations.GetEntries();i++){
	//	CalibrationData *data=(CalibrationData*)calibrations.At(i);
	//	Int_t series=data->getSeries(),run=data->getRun();
	//	CalibrationData *data2=herbsCal.getCal(series,run);
	//	log<<data->getFile().c_str();
	//	for(Int_t j=0;j<12;j++){
	//		if(data->getPeak(j)>0 && data2->getPeak(j)>0){
	//			log<<","<<data->getPeak(j)/data2->getPeak(j);
	//		}else{
	//			log<<",";
	//		}
	//	}
	//	log<<endl;
	//}
	//log.close();
	gSystem->ChangeDirectory(WORKDIR);
}