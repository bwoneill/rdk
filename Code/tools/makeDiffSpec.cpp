//makeDiffSpec.cpp

#include <fstream>
using namespace std;

TGraphErrors graph(77);

void makeDiffSpec(){
	TString temp;
	temp.Form("%s/diffspec.csv",RDKHOME);
	ifstream input(temp);
	Char_t buff[255];
	for(Int_t i=0;i<77;i++){
		input.getline(buff,255);
		temp=buff;
		TStringToken token(temp,",");
		token.NextToken();
		Double_t x=token.Atof();
		token.NextToken();
		Double_t y=token.Atof();
		token.NextToken();
		Double_t sigma=token.Atof();
		graph.SetPoint(i,x,y);
		graph.SetPointError(i,0,sigma);
	}
	graph.Draw();
}