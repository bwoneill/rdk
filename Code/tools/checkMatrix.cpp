//checkMatrix.cpp

TFile *matFile;
TMatrixD *mat;

void checkMatrix(){
	matFile=new TFile("C:/Opera/tables/magnet.root");
	mat=(TMatrixD*)matFile->Get("bfield");
	for(float r=0;r<=7;r+=0.25){
		for(int theta=0;theta<=360;theta+=2){
			if(r==0 && theta!=0){
				continue;
			}
			for(float z=0;z<=45;z+=0.25){
				int A=0;
				if(r==0){
					A=4*z;
				}else{
					A=131044*r-4*z+181*theta/2-32400;
				}
				double R=(*mat)[A][0],Z=(*mat)[A][2];
				int T=(*mat)[A][1];
				bool test=(r==R)&&((theta-T)%360==0)&&(z==Z);
				if(!test){
					cerr<<Form("i=%i (%0.2f,%i,%0.2f)!=(%0.2f,%i,%0.2f)",A,r,theta,z,R,T,Z)<<endl;
					return;
				}
			}
		}
	}
}