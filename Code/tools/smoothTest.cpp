//smoothTest.cpp

TGraph *g1,*g2,*g3;
TCanvas c1;

void smoothTest(){
	openRawFile(506,0);
	getNextGamma();
	double xaxis[NPOINTS],cdata[NPOINTS],fdata[NPOINTS],delta[NPOINTS];
	for(int i=0;i<NPOINTS;i++){
		xaxis[i]=i;
		cdata[i]=rdata.ch[ch][i];
		fdata[i]=rdata.ch[ch][i];
	}
	cufftSmooth csmooth(NPOINTS,64);
	csmooth.smooth(cdata);
	fftwSmooth fsmooth(NPOINTS,64);
	fsmooth.smooth(fdata);
	for(int i=0;i<NPOINTS;i++){
		delta[i]=cdata[i]-fdata[i];
	}
	c1.Divide(1,2);
	c1.cd(1);
	drawGamma();
	g1=new TGraph(NPOINTS,xaxis,cdata);
	g1->SetLineColor(kBlue);
	g1->Draw("same");
	g2=new TGraph(NPOINTS,xaxis,fdata);
	g2->SetLineColor(kRed);
	g2->Draw("same");
	c1.cd(2);
	g3=new TGraph(NPOINTS,xaxis,delta);
	g3->SetTitle("Residuals");
	g3->GetXaxis()->SetLimits(0,NPOINTS-1);
	g3->Draw("al");
	c1.cd(0);
}