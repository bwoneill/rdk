//drawEvent.cpp

TCanvas c1;
TGraph g(2048);
TGraph h(2048);
TGaxis *axis=NULL;

void drawEvent(TString str){
	Int_t nseries,nrun,nevent,ch;
	Char_t type;
	TString temp=str(TPRegexp("[SCD*]\\d+"));
	type=temp(TPRegexp("[SCD]"));
	TPRegexp digits("\\d+");
	temp=temp(digits);
	nseries=temp.Atoi();
	temp=str(TPRegexp("r\\d+"));
	temp=temp(digits);
	nrun=temp.Atoi();
	temp=str(TPRegexp("e\\d+"));
	temp=temp(digits);
	nevent=temp.Atoi();
	temp=str(TPRegexp("c\\d+"));
	temp=temp(digits);
	ch=temp.Atoi();
	//cout<<"*"<<nseries<<"r"<<nrun<<"e"<<nevent<<"c"<<ch<<endl;
	RawFileReader reader;
	reader.open(type,nseries,nrun);
	reader.seek(nevent);
	Entry entry;
	RawData rdata;
	reader.readNext(entry,rdata);
	reader.close();
	g.SetLineColor(kBlue);
	Float_t g_min=rdata.ch[ch][0],g_max=g_min,h_min=rdata.ch[15][0],h_max=h_min;
	for(Int_t i=1;i<2048;i++){
		if(rdata.ch[ch][i]>g_max){
			g_max=rdata.ch[ch][i];
		}else if(rdata.ch[ch][i]<g_min){
			g_min=rdata.ch[ch][i];
		}
		if(rdata.ch[15][i]>h_max){
			h_max=rdata.ch[15][i];
		}else if(rdata.ch[15][i]<h_min){
			h_min=rdata.ch[15][i];
		}
	}
	Float_t scale=(h_max-h_min)/(g_max-g_min),shift=(g_max*h_min-g_min*h_max)/(g_max-g_min);
	for(Int_t i=0;i<2048;i++){
		g.SetPoint(i,0.04*i,scale*rdata.ch[ch][i]+shift);
		h.SetPoint(i,0.04*i,rdata.ch[15][i]);
	}
	g.GetXaxis()->SetRangeUser(0,2047*0.04);
	h.GetXaxis()->SetRangeUser(0,2047*0.04);
	h.Draw("al");
	g.Draw("same");
	if(axis){
		delete axis;
	}
	c1.Update();
	Float_t min=(gPad->GetUymin()-shift)/scale;
	Float_t max=(gPad->GetUymax()-shift)/scale;
	axis=new TGaxis(c1.GetUxmax(),c1.GetUymin(),c1.GetUxmax(),c1.GetUymax(),min,max,510,"+L");
	axis->SetLineColor(kBlue);
	axis->SetLabelColor(kBlue);
	axis->Draw();
	cout<<g_min<<endl;
	cout<<g_max<<endl;
}