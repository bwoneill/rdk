//compare_events.cpp

CompareData* compare_events(){
	TFile f1("../Plots/event_comparison/JSN_S223.root");
	TTree *jeff=(TTree*)f1.Get("rdk_jsn");
	TFile f2("I:/analysis/bwoneill/second_pass/ss223/S223.root");
	TTree *mine=(TTree*)f2.Get("rdk_second_pass");
	CompareData *comp=new CompareData("../Plots/event_comparison/S223_comparison.root");
	comp->compare(jeff,mine);
	return comp;
}