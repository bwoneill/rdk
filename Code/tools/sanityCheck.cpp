//sanityCheck.cpp

#include <iomanip>
#include <fstream>

void sanityCheck(){
	open1400();
	ofstream out("D:/Documents/Physics/Research/RDK/output.txt");
	out.setf(ios::left,ios::adjustfield);
	out<<setw(15)<<"T"<<setw(5)<<"S"<<setw(5)<<"R"<<setw(5)<<"E"<<endl;
	for(int i=0;i<tree->GetEntries();i++){
		tree->GetEntry(i);
		if(recon.t_gamma[2]>0 && recon.t_gamma[2]<NPOINTS && recon.E_gamma[2]>=315 && recon.E_gamma[2]<=505){
			out<<setw(15)<<entry.timestamp<<setw(5)<<entry.nseries<<setw(5)<<entry.nrun<<setw(5)<<entry.nevent<<endl;
		}
	}
}