//plotall.cpp

TreeReader reader;
Plotter *plotter=Plotter::getPlotter();

void plotall(){
	reader.reset();
	reader.openList(Form("%s/runlist.csv",RDKANAL));
	TFile file(Form("%s/cal.root",RDKANAL));
	CalibrationList *bapd_list=(CalibrationList*)file.Get("CalibrationList");
	CalibrationList *bgo_list=new CalibrationList(
		Form("%s/calibration/TCfit_511keV_calibration_21Feb11.dat",RDKANAL));
	plotter->reset();
	plotter->fill(&reader,bgo_list,bapd_list);
	plotter->draw(Form("%s/Temp/analysis/",RDKHOME));
	plotter->save(Form("%s/Temp/analysis/",RDKHOME));
	delete bgo_list;
}

void replot(){
	plotter->open(Form("%s/Temp/analysis/",RDKHOME));
	plotter->draw(Form("%s/Temp/analysis/",RDKHOME));
}