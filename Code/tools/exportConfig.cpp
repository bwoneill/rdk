//exportConfig.cpp

void exportConfig(){
	ofstream output("config.csv");
	output<<"Series,ch00,ch01,ch02,ch03,ch04,ch05,ch06,ch07,ch10,ch11,ch12,ch13,ch14,ch15,ch16,ch17"<<endl;
	vector<Int_t> series=RawTreeReader::getSeries(RDKANAL);
	for(Int_t i=0;i<series.size();i++){
		UInt_t config=rdk2config->getConfig(series[i]);
		output<<series[i];
		for(Int_t j=0;j<16;j++){
			UInt_t temp=(config>>(2*j))&3U;
			output<<","<<temp;
		}
		output<<endl;
	}
	output.close();
}