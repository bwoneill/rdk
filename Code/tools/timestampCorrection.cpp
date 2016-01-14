//timestampCorrection.cpp
Long64_t mask=0xFFFFFFFEFFFFFFFF;
TGraph *g=NULL,*g2=NULL;
TF1 *f=new TF1("f","pol1");
vector<double> sum,sum2,var;

void timestampCorrection(){
	LinearFitter2 lf(100);
	TreeReader reader;
	reader.openSeries(256,"S");
	Long64_t N=reader.tree->GetEntries();
	Long64_t n=N/100;
	Long64_t fvu=0,c2t=0,diff=0;
	if(g){
		delete g;
	}
	g=new TGraph(n);
	g->SetTitle("Timestamp fit correction;block number;#chi^{2}/Ndf");
	g->SetMarkerStyle(kFullDotSmall);
	if(g2){
		delete g2;
	}
	g2=new TGraph(n);
	g2->SetTitle("Timestamp fit correction;block number;FVU=1-R^{2}");
	g2->SetMarkerStyle(kFullDotSmall);
	for(Long64_t i=0;i<n;i++){
		for(Long64_t j=0;j<100;j++){
			reader.tree->GetEntry(100*i+j);
			Long64_t t0=150*reader.entry.timestamp[0]+0.5;
			Long64_t t1=150*reader.entry.timestamp[1]+0.5;
			Long64_t delta=(t0-t1)&mask;
			lf.setPoint(j,t1,delta);
		}
		lf.fit();
		g->SetPoint(i,i,lf.getChi2()/98);
		g2->SetPoint(i,i,1-lf.getR2());
		if(1-lf.getR2()>1e-4){
			fvu++;
		}
		if(lf.getChi2()>9.8e3){
			c2t++;
		}
		if(lf.getChi2()>9.8e3 && 1-lf.getR2()>1e-4){
			diff++;
		}
	}
	//g->Draw("ap");
	//g->GetXaxis()->SetRangeUser(0,n-1);
	g2->Draw("ap");
	gPad->SetLogy(true);
	g2->GetXaxis()->SetRangeUser(0,n-1);
	cout<<fvu<<"/"<<n<<endl;
	cout<<c2t<<"/"<<n<<endl;
	cout<<diff<<"/"<<n<<endl;
}

//void timestampCorrection(){
//	f->SetParameter(0,6513);
//	f->SetParameter(1,-3.1459e-6);
//	f->SetLineColor(kRed);
//	f->SetRange(0,1.1e12);
//	TreeReader reader;
//	reader.openSeries(256,"S");
//	Long64_t N=reader.tree->GetEntries("nrun==4");
//	Long64_t n=N/100;
//	sum.resize(n);
//	sum2.resize(n);
//	var.resize(n);
//	for(Long64_t j=0;j<n;j++){
//		sum[j]=0;
//		sum2[j]=0;
//	}
//	if(g){
//		delete g;
//	}
//	if(g2){
//		delete g2;
//	}
//	g=new TGraph(N);
//	g2=new TGraph(n);
//	Long64_t j=-1;
//	for(Long64_t i=0;i<N;i++){
//		reader.tree->GetEntry(i);
//		Long64_t t0=150*reader.entry.timestamp[0];
//		Long64_t t1=150*reader.entry.timestamp[1];
//		Long64_t delta=(t0-t1)&mask;
//		delta-=f->Eval(t1);
//		g->SetPoint(i,t1,delta);
//		if(i%100==0){
//			j++;
//		}
//		sum[j]+=(double)delta;
//		sum2[j]+=(double)delta*(double)delta;
//	}
//	for(Long64_t j=0;j<n;j++){
//		var[j]=sum2[j]*0.01-sum[j]*sum[j]*1e-4;
//		g2->SetPoint(j,j,var[j]);
//	}
//	g->SetTitle("Corrected timestamp;clock cycles;deviation");
//	g->SetMarkerStyle(kFullDotSmall);
//	g2->SetTitle("Corrected timestamp;block number;variance");
//	g2->SetMarkerStyle(kFullDotSmall);
//}