//cudaFitTest.cpp
using namespace TMath;

TH1F *deltaE,*deltaT,*paramB;
cudaFit *fitter;
float **data;
int CH[11]={0,1,3,4,5,7,8,9,10,11,12};
float *debug=new float[512*2048*8+1];

TTree* cudaFitTest(){
	//find/record test signals
	TreeReader reader;
	reader.openFile(258,0);
	int index[512],channel[512];
	float energy[512],time[512];
	int i=0,j=0;
	while(i<512 && j<reader.tree->GetEntries()){
		reader.tree->GetEntry(j);
		for(int k=0;k<11;k++){
			int t=reader.recon.t_gamma[CH[k]];
			float E=reader.recon.E_gamma[CH[k]];
			if(t>0 && t<NPOINTS && E>3000 && E<6000 && i<512){
				index[i]=j;
				channel[i]=CH[k];
				time[i]=t;
				energy[i]=E;
				i++;
			}
		}
		j++;
	}
	reader.reset();
	//copy signals to data;
	RawFileReader rawReader(258,0);
	Entry entry;
	RawData rdata;
	data=new float[512];
	i=0;
	rawReader.initialize();
	for(i=0;i<512;i++){
		data[i]=new float[2048];
		j=index[i];
		int k=channel[i];
		rawReader.seek(j);
		rawReader.readNext(entry,rdata);
		for(int x=0;x<2048;x++){
			data[i][x]=rdata.ch[k][x];
		}
	}
	rawReader.close();
	//fit
	fitter=new cudaFit(512);
	fitter->setData(data);
	float *initParam=new float[8];
	initParam[0]=100;
	initParam[1]=0;
	initParam[2]=15000;
	initParam[3]=1.1e-3;
	initParam[4]=1.3e-3;
	initParam[5]=625;
	initParam[6]=0;
	initParam[7]=0;
	fitter->fit(initParam,0);
	float **param=new float[512];
	for(i=0;i<512;i++){
		param[i]=new float[8];
	}
	float fitE[512],fitT[512];
	fitter->getParameters(param);
	for(int evnt=0;evnt<512;evnt++){
		fitE[evnt]=peakHeight(param[evnt]);
		fitT[evnt]=param[evnt][5];
	}
	//compare
	deltaT=new TH1F("deltaT","deltaT",250,-250,0);
	deltaE=new TH1F("deltaE","deltaE",200,0,2);
	paramB=new TH1F("paramB","paramB",500,10000,30000);
	for(int evnt=0;evnt<512;evnt++){
		deltaT->Fill(time[evnt]-fitT[evnt]);
		deltaE->Fill(fitE[evnt]/energy[evnt]);
		paramB->Fill(param[evnt][2]);
	}
	delete[] param;
	delete[] initParam;
}

float peakHeight(const float *par){
	return par[0]+par[2]*par[4]/(par[3]+par[4])*Power((par[3]+par[4])/par[3],-par[3]/par[4]);
}