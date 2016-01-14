//merge.cpp

#include <fstream>
using namespace std;

void parse(Double_t *data,TString buffer){
	TStringToken token(buffer,",");
	for(Int_t i=0;i<6;i++){
		token.NextToken();
		data[i]=token.Atof();
	}
}

Int_t compare(Double_t *data1,Double_t *data2){
	if(data1[0]<data2[0]){
		return -1;
	}else if(data1[0]>data2[0]){
		return 1;
	}else if(data1[1]<data2[1]){
		return -1;
	}else if(data1[1]>data2[1]){
		return 1;
	}else if(data1[2]<data2[2]){
		return -1;
	}else if(data1[2]>data2[2]){
		return 1;
	}else{
		return 0;
	}
}

void formatMine(Double_t *data,ofstream &output){
	for(Int_t i=0;i<6;i++){
		output<<data[i]<<",";
	}
	output<<",,"<<endl;
}

void formatJeff(Double_t *data,ofstream &output){
	for(Int_t i=0;i<3;i++){
		output<<data[i]<<",";
	}
	output<<",,";
	for(Int_t i=3;i<6;i++){
		output<<","<<data[i];
	}
	output<<endl;
}

void formatBoth(Double_t *data1,Double_t *data2,ofstream &output){
	for(Int_t i=0;i<6;i++){
		output<<data1[i]<<",";
	}
	for(Int_t i=3;i<6;i++){
		output<<data2[i];
		if(i<5){
			output<<",";
		}
	}
	output<<endl;
}

void merge(){
	TString temp,buff1,buff2;
	Double_t myEvent[6],jeffEvent[6];
	temp.Form("%s/eventLists/list.csv",RDKANAL);
	ifstream myInput(temp);
	temp.Form("%s/eventLists/jeff.csv",RDKANAL);
	ifstream jeffInput(temp);
	temp.Form("%s/eventLists/merge.csv",RDKANAL);
	ofstream output(temp);
	myInput>>buff1;
	jeffInput>>buff2;
	output<<"Series,Run,EventNum,PeakHeight,Energy,DeltaTime,";
	output<<"PeakHeight_JSN,Energy_JSN,DeltaTime_JSN"<<endl;
	Bool_t more=kTRUE,mine=kFALSE,jeff=kFALSE;
	do{
		if(!mine ){
			myInput>>buff1;
			parse(myEvent,buff1);
			mine=!myInput.eof();
		}
		if(!jeff){
			jeffInput>>buff2;
			parse(jeffEvent,buff2);
			jeff=!jeffInput.eof();
		}
		if(mine && jeff){
			Int_t next=compare(myEvent,jeffEvent);
			if(next==-1){
				formatMine(myEvent,output);
				mine=kFALSE;
			}else if(next==1){
				formatJeff(jeffEvent,output);
				jeff=kFALSE;
			}else{
				formatBoth(myEvent,jeffEvent,output);
				mine=kFALSE;
				jeff=kFALSE;
			}
		}else if(mine){
			formatMine(myEvent,output);
			mine=kFALSE;
		}else if(jeff){
			formatJeff(jeffEvent,output);
			jeff=kFALSE;
		}
		more= !myInput.eof() || !jeffInput.eof();
	}while(more);
	myInput.close();
	jeffInput.close();
	output.close();
}