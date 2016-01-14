//noiseProfile.cpp

TVectorD noiseProfile(Long64_t N){
	TVectorD noise(NPOINTS);
	for(int i=0;i<NPOINTS;i++){
		noise[i]=0;
	}
	if(raw){
		double nevents=0;
		for(int i=0;i<raw->GetEntries() && nevents<N;i++){
			raw->GetEntry(i);
			for(int j=0;j<1;j++){
				if(recon.t_gamma[j]<1 && recon.E_gamma[j]<1){
					for(int k=0;k<NPOINTS;k++){
						noise[k]+=rdata.ch[j][k];
					}
					nevents++;
				}
			}
		}
		if(nevents>0){
			for(int i=0;i<NPOINTS;i++){
				noise[i]/=nevents;
			}
		}
	}
	return noise;
}