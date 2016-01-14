//Smoother.cpp

#include "Smoother.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TThread.h"
#include "RDK_OMP.h"

using namespace TMath;

Smoother::Smoother(UInt_t r,Double_t p,UInt_t o){
	vectors=NULL;
	init(r,p,o);
}

Smoother::~Smoother(){
	clean();
}

void Smoother::init(UInt_t r,Double_t p,UInt_t o){
	radius=r;
	order=o;
	power=p;
	size=2*radius+1;
	clean();
	vectors=new TVectorD[size];
	for(Int_t i=0;i<size;i++){
		Int_t offset=Abs((Int_t)(radius-i)),dim=size-offset;
		vectors[i].ResizeTo(dim);
		TMatrixD x(dim,order+1),unit(1,order+1);
		for(Int_t j=0;j<order+1;j++){
			unit[0][j]=Power(0,j);
		}
		TMatrixD w(dim,dim);
		for(Int_t j=0;j<dim;j++){
			for(Int_t k=0;k<order+1;k++){
				x[j][k]=Power(j-i,k);
			}
			if(i>radius){
				x[j][1]+=offset;
			}
			w[j][j]=1-Power(Abs(x[j][1]/(radius+1)),power);
		}
		TMatrixD xT(order+1,dim);
		xT.Transpose(x);
		TMatrixD xTwx(xT*w*x);
		xTwx.Invert();
		vectors[i]=(unit*xTwx*xT*w)[0];
	}
	//vectors[radius].Print();
}

void Smoother::smooth(const TMatrixTRow<Double_t> &in,TMatrixTRow<Double_t> &out,
											Int_t dim)const{
	const Int_t upr=2*radius-dim+1;
	Double_t *buffer=new Double_t[dim];
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,16) num_threads(16)
#endif
	for(Int_t i=0;i<dim;i++){
		Int_t lower,upper,index;
		Double_t sum=0;
		if(i<=radius){
			lower=0;
			upper=i+radius;
			index=i;
		}else if(i<dim-radius){
			lower=i-radius;
			upper=i+radius;
			index=radius;
		}else{
			lower=i-radius;
			upper=dim-1;
			index=i+upr;
		}
		for(Int_t j=lower;j<=upper;j++){
			sum+=in[j]*vectors[index][j-lower];
		}
		buffer[i]=sum;
		//out[i]=in.GetSub(lower,upper)*vectors[index];
	}
	for(Int_t i=0;i<dim;i++){
		out[i]=buffer[i];
	}
	delete[] buffer;
}

void Smoother::smooth(const TVectorD &in, TVectorD &out) const{
	Int_t dim=in.GetNrows(),upr=2*radius-dim+1;
	if(out.GetNrows()!=dim){
		out.ResizeTo(dim);
	}
	Double_t *buffer=new Double_t[dim];
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,16) num_threads(16)
#endif
	for(Int_t i=0;i<dim;i++){
		Int_t lower,upper,index;
		Double_t sum=0;
		if(i<=radius){
			lower=0;
			upper=i+radius;
			index=i;
		}else if(i<dim-radius){
			lower=i-radius;
			upper=i+radius;
			index=radius;
		}else{
			lower=i-radius;
			upper=dim-1;
			index=i+upr;
		}
		for(Int_t j=lower;j<=upper;j++){
			sum+=in[j]*vectors[index][j-lower];
		}
		buffer[i]=sum;
		//out[i]=in.GetSub(lower,upper)*vectors[index];
	}
	for(Int_t i=0;i<dim;i++){
		out[i]=buffer[i];
	}
	delete[] buffer;
}

TVectorD Smoother::smooth(const TVectorD &in) const{
	TVectorD out(in.GetNrows());
	smooth(in,out);
	return out;
}

void Smoother::clean(){
	if(vectors){
		delete[] vectors;
		vectors=NULL;
	}
}

Double_t Smoother::sigma_v() const{
	return Sqrt(vectors[radius]*vectors[radius]);
}

Double_t Smoother::sigma_h() const{
	Int_t dim=2*radius+1;
	Double_t sumW=0,sumX=0,sumX2=0;
#ifdef RDK_OMP
#pragma omp parallel for schedule(dynamic,16) num_threads(16)
#endif
	for(Int_t i=0;i<dim;i++){
		Double_t W=Abs(vectors[radius][i]),x=i-(Double_t)radius;
		sumW+=W;
		sumX+=x*W;
		sumX2+=x*x*W;
	}
	Double_t avgx=sumX/sumW,avgx2=sumX2/sumW;
	return Sqrt(avgx2-Power(avgx,2));
}