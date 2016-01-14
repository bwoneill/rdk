//createMagnet.cpp

void createMagnet(){
	TString angle[2]={"180","9.5"};
	for(int i=1;i<2;i++){
		VectorField *field=new VectorField(new BoreMap(angle[i].Atof()));
		FieldReader *reader=new FieldReader(field);
		for(float r=0;r<=7;r+=0.25){
			TString file=Form("C:/Opera/tables/Magnet Rr=%1.2f th=%s.table",r,angle[i].Data());
			cout<<"Reading from file "<<file<<endl;
			reader->read(file);
		}
		cout<<"Saving magnetic field"<<endl;
		field->save("C:/Opera/tables/magnet.root",Form("bfield-%s",angle[i].Data()));
		cout<<"Calculating potential"<<endl;
		BoreInt integrator(field,angle[i].Atof());
		integrator.integrate();
		ScalarField *pot=integrator.getPotential();
		cout<<"Saving scalar potential"<<endl;
		pot->save("C:/Opera/tables/magnet.root",Form("potential-%s",angle[i].Data()));
		delete reader;
		delete field;
		delete pot;
	}
}