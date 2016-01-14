//openMagnet.cpp

VectorField *field1,*field2;
ScalarField *pot1,*pot2;
TGraph2D *g=NULL;

void openMagnet(){
	TStopwatch stopwatch;
	stopwatch.Start();
	TFile magfile("C:/Opera/tables/magnet.root");
	FieldMap *map1=new BoreMap(180),*map2=new BoreMap(9.5);
	field1=new VectorField(map1,&magfile,"bfield-180");
	pot1=new ScalarField(map1,&magfile,"potential-180");
	field2=new VectorField(map2,&magfile,"bfield-9.5");
	pot2=new ScalarField(map2,&magfile,"potential-9.5");
	magfile.Close();
	stopwatch.Stop();
	cout<<"Time to open magnet file: "<<stopwatch.RealTime()<<"s"<<endl;
	cout<<"Cpu time: "<<stopwatch.CpuTime()<<"s"<<endl;
}

void polarPlotPotential(ScalarField *pot,double Z){
	const BoreMap *map=(BoreMap*)pot->getMap();
	double psi=map->getPsi();
	if(g!=NULL){
		g->Delete();
	}
	const int n=180*28+1;
	double x[n],y[n],z[n];
	TVector3 temp(0,0,Z);
	temp.RotateY(psi);
	x[0]=0;
	y[0]=0;
	z[0]=1e-6*pot->get(temp);
	for(float r=0.25;r<=7;r+=0.25){
		for(int phi=0;phi<360;phi+=2){
			TVector3 v(r,0,Z);
			v.RotateZ(TMath::DegToRad()*phi);
			int index=TMath::Nint(180*4*r)+phi/2-179;
			x[index]=v.x();
			y[index]=v.y();
			v.RotateY(psi);
			z[index]=1e-6*pot->get(v);
		}
	}
	g=new TGraph2D("potgraph","Magnetic Scalar Potential;x(cm);y(cm);#phi(V*s/m)",n,x,y,z);
	g->Draw("tri");
}

void checkTime(int n){
	double *array=new double[n];
	TRandom rand;
	rand.RndmArray(n,array);
	TStopwatch stopwatch;
	stopwatch.Start();
	for(int i=0;i<n;i++){
		pot1->get(TVector3(array[n],2*array[n],3*array[n]));
	}
	stopwatch.Stop();
	cout<<"Time to get "<<n<<" scalar points: "<<stopwatch.RealTime()<<"s"<<endl;
	cout<<"Cpu time: "<<stopwatch.CpuTime()<<"s"<<endl;
	cout<<"Time per get: "<<stopwatch.CpuTime()*1e9/n<<"ns"<<endl;
}