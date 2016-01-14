//generateComparison.cpp

TLegend *legend=NULL;
TGraph *graph_mc=NULL;
TGraphErrors *graph_j=NULL;
Double_t *graph_y=NULL;

void compareEe(TH1F *hist,TGraph *g,TGraphErrors *j=NULL){
	Int_t n=g->GetN();
	Double_t *x=g->GetX(),*y=g->GetY();
	if(graph_y!=NULL){
		delete[] y;
	}
	Double_t sum=0;
	for(Int_t i=0;i<n;i++){
		sum+=y[i];
	}
	if(graph_mc!=NULL){
		delete graph_mc;
	}
	graph_mc=new TGraph(n);
	for(Int_t i=0;i<n;i++){
		graph_mc->SetPoint(i,x[i],y[i]/sum);
	}
	graph_mc->SetLineColor(kRed);
	if(legend!=NULL){
		delete legend;
	}
	legend=new TLegend(0.7,0.8,0.9,0.9);
	if(j!=NULL){
		legend->AddEntry(hist,"Data - O\'Neill","lp");
		if(graph_j!=NULL){
			delete graph_j;
		}
		sum=0;
		n=j->GetN();
		x=j->GetX();
		y=j->GetY();
		Double_t *ey=j->GetEY();
		for(Int_t i=0;i<n;i++){
			sum+=y[i];
		}
		graph_j=new TGraphErrors(n);
		for(Int_t i=0;i<n;i++){
			graph_j->SetPoint(i,x[i],y[i]/sum);
			graph_j->SetPointError(i,0,ey[i]/sum);
		}
		graph_j->SetLineColor(kGreen+2);
		legend->AddEntry(graph_j,"Data - Nico","lp");
	}else{
		legend->AddEntry(hist,"Data","lp");
	}
	legend->AddEntry(graph_mc,"Monte Carlo","lp");
}

void compareTof(TH1F *hist,TGraph *g1,TGraphErrors *g2=NULL){
	Int_t n=g1->GetN();
	Double_t *x=g1->GetX(),*y=g1->GetY();
	Double_t sum=0;
	for(Int_t i=0;i<n;i++){
		sum+=y[i];
	}
	if(graph_mc!=NULL){
		delete graph_mc;
	}
	graph_mc=new TGraph(n);
	for(Int_t i=0;i<n;i++){
		graph_mc->SetPoint(i,x[i]*1e6,y[i]/sum);
	}
	graph_mc->SetLineColor(kRed);
	if(legend!=NULL){
		delete legend;
	}
	legend=new TLegend(0.7,0.8,0.9,0.9);
	if(g2!=NULL){
		if(graph_j!=NULL){
			delete graph_j;
		}
		n=g2->GetN();
		x=g2->GetX();
		y=g2->GetY();
		Double_t *ey=g2->GetEY();
		graph_j=new TGraphErrors(n);
		sum=0;
		for(Int_t i=0;i<n;i++){
			sum+=y[i];
		}
		for(Int_t i=0;i<n;i++){
			graph_j->SetPoint(i,x[i]*1e6,y[i]/sum);
			graph_j->SetPointError(i,0,ey[i]/sum);
		}
		graph_j->SetLineColor(kGreen+2);
		legend->AddEntry(hist,"Data - O\'Neill","lp");
		legend->AddEntry(graph_j,"Data - Nico","lp");
	}else{
		legend->AddEntry(hist,"Data","lp");
	}
	legend->AddEntry(graph_mc,"Monte Carlo","lp");
}

void compareGamma(TH1F *hist,TGraph *g,TGraphErrors *j=NULL){
	if(graph_mc!=NULL){
		delete graph_mc;
	}
	Double_t *x=g->GetX(),*y=g->GetY();
	Int_t n=g->GetN();
	graph_mc=new TGraph(n);
	for(Int_t i=0;i<n;i++){
		graph_mc->SetPoint(i,x[i],y[i]);
	}
	graph_mc->SetLineColor(kRed);
	if(legend!=NULL){
		delete legend;
	}
	legend=new TLegend(0.7,0.8,0.9,0.9);
	if(j!=NULL){
		legend->AddEntry(hist,"Data - O\'Neill","lp");
		if(graph_j!=NULL){
			delete graph_j;
		}
		n=j->GetN();
		x=j->GetX();
		y=j->GetY();
		Double_t *ey=j->GetEY();
		graph_j=new TGraphErrors(n);
		for(Int_t i=0;i<n;i++){
			graph_j->SetPoint(i,x[i],y[i]);
			graph_j->SetPointError(i,0,ey[i]);
		}
		graph_j->SetLineColor(kGreen+2);
		legend->AddEntry(graph_j,"Data - Nico","lp");
	}else{
		legend->AddEntry(hist,"Data","lp");
	}
	legend->AddEntry(graph_mc,"Monte Carlo","lp");
}