//dwtTest.cpp

void dwtTest(){
	HaarWavelet haar(3);
	vector<double> in(8),out(8);
	for(int i=0;i<8;i++){
		in[i]=i*i-i+1;
		cout<<i<<": "<<in[i]<<endl;
	}
	haar.full_dwt(in,out);
	for(int i=0;i<8;i++){
		cout<<i<<": "<<out[i]<<endl;
	}
	haar.full_idwt(out,in);
	for(int i=0;i<8;i++){
		cout<<i<<": "<<in[i]<<endl;
	}
}