//modwtTest.cpp

Modwt modwt(2048);
vector<double> wavelet;
TGraph graph(2048);

void modwtTest(){
	wavelet[0]=1;
	wavelet[1]=-1;
	modwt.setWavelet(wavelet);
	RawFileReader reader;
	reader.open(256,4);
	Entry entry;
	RawData rdata;
	reader.seek(12623);
	reader.readNext(entry,rdata);
	vector<double> input(2048);
	for(int i=0;i<2048;i++){
		input[i]=rdata.ch[2][i];
	}
	modwt.transform(input);
	for(int i=0;i<2048;i++){
		graph.SetPoint(i,i,modwt.getV()[20480+i]);
	}
	graph.Draw("al");
}