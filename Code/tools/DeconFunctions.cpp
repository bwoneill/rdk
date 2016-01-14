//DeconFunctions.cpp

double *getImpulse(){
	double *impulse=new double[NPOINTS];
	for(int i=0;i<NPOINTS;i++){
		impulse[i]=exp(-1e-3*i);
	}
	return impulse;
}

double *getSmooth(){
	double *smooth=new double[NPOINTS];
	for(int i=0;i<NPOINTS;i++){
		smooth[i]=gaussian(i,4)+gaussian(i-NPOINTS,4);
	}
	return smooth;
}

double *getNoise(){
	double *noise=new double[NPOINTS];
	for(int i=0;i<NPOINTS;i++){
		noise[i]=exp(8.73953+3.15517e-42*pow(i-1024,2));
	}
	return noise;
}

double gaussian(double x,double sigma){
	return exp(-pow(x/sigma,2)/2)/sqrt(TMath::TwoPi())/sigma;
}