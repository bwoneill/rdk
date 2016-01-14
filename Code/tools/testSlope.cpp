//testSlope.cpp

void testSlope(Int_t mode){
	TChain t1("rdk");
	Recon r1;
	if(mode==0){
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/C270r0.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/C270r1.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/C270r2.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/D270r3.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r4.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r5.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r6.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r7.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r8.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r9.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/C270r10.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/C270r11.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/C270r12.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/D270r13.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r14.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r15.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r16.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/noslope/ss270/S270r17.root");
	}else{
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/C270r0.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/C270r1.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/C270r2.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/D270r3.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r4.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r5.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r6.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r7.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r8.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r9.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/C270r10.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/C270r11.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/C270r12.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/D270r13.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r14.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r15.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r16.root");
		t1.Add("/mnt/data/rdk2/analysis/bwoneill/ss270/S270r17.root");
	}
	t1.SetBranchAddress("recon",(Double_t*)&r1);
	CalibrationData::initCalibration();
	for(Int_t i=0;i<t1.GetEntries();i++){
		t1.GetEntry(i);
		for(Int_t j=0;j<14;j++){
			if(r1.E_gamma[j]>300 && r1.t_gamma[j]>100 && r1.t_gamma[j]<900){
				CalibrationData::hist[j]->Fill(r1.E_gamma[j]);
			}
		}
	}
	CalibrationData *data=CalibrationData::calibrate('C',270,0,0);
	for(Int_t i=0;i<14;i++){
		cout<<"Ch "<<i<<": "<<data->getPeak(i)<<endl;
	}
	delete data;
}
