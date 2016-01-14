//checkCalibrationMode.cpp

void checkCalibrationMode(){
	RDKConfig config(Form("%s/detector_config.csv",RDKANAL));
	CalibrationList list(Form("%s/temp_calibration.csv",RDKANAL));
	for(Int_t i=0;i<list.GetEntries();i++){
		CalibrationData *data=(CalibrationData*)list.At(i);
		UInt_t c=config.getConfig(data->getSeries());
		for(Int_t j=0;j<14;j++){
			UInt_t mode=RDKConfig::getMode(c,j);
			if(mode==1U && data->getPeak(j)<=0){
				cout<<"Uncalibrated detector: S"<<data->getSeries()<<"r"<<data->getRun();
				cout<<"#"<<j/7<<j%7<<endl;
			}
		}
	}
}