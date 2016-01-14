//chechHash.cpp

void checkHash(){
	double **table=new double*[BoreMap::max];
	for(int I=0;I<BoreMap::max;I++){
		table[I]=new double[4];
		for(int J=0;J<4;J++){
			table[I][J]=0;
		}
	}
	TFile magfile("C:/Opera/tables/magnet.root");
	TString angle[2]={"180","9.5"};
	for(int i=0;i<2;i++){
		FieldMap *map=new BoreMap(angle[i].Atof());
		TTree *bfield=(TTree*)magfile.Get(Form("bfield-%s",angle[i].Data()));
		double buff[3];
		bfield->SetBranchAddress("point",buff);
		bfield->SetBranchStatus("value",0);
		for(int n=0;n<bfield->GetEntries();n++){
			if((n%1000)==0){
				cout<<"Vectors checked: "<<n<<endl;
			}
			bfield->GetEntry(n);
			TVector3 temp(buff);
			Long64_t hash=map->hash(temp);
			if(hash>=BoreMap::max || hash<0){
				cout<<"Invalid hash="<<hash<<" for vector number "<<n<<endl;
				temp.Print();
				BoreMap::DEBUG=true;
				map->hash(temp);
				BoreMap::DEBUG=false;
				return;
			}
			if(table[hash][0]!=0){
				TVector3 v1(buff);
				bfield->GetEntry(table[hash][0]);
				TVector3 v2(buff);
				TVector3 diff=v1-v2;
				if(diff.Mag()>1e-2){
					cout<<"Collision at hash="<<hash<<endl;
					cout<<"Vectors "<<n<<" and "<<table[hash][0]<<endl;
					Long64_t h1=map->hash(v1),h2=map->hash(v2);
					cout<<"Vector "<<n<<" hash="<<h1<<endl;
					v1.Print();
					cout<<"Vector "<<table[hash][0]<<" hash="<<h2<<endl;
					v2.Print();
					return;
				}
			}
			table[hash][0]=n;
			for(int j=0;j<3;j++){
				table[hash][j+1]=buff[j];
			}
		}
		delete map;
	}
	delete[] table;
}