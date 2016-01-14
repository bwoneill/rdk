//RDKTypes.h

#ifndef RDKTYPES_H
#define RDKTYPES_H

#include "RDKConfig.h"
#include "Rtypes.h"
#include <exception>
#include <ostream>

enum AnalyzerMode {kFit,kSmooth,kDecon};

const Int_t NGAMMA=14,NCHANNELS=16,NPARAM=6;
const Int_t NPOINTS=2048;
const Int_t NFLAT=128;
const Int_t MAXHEIGHT=30000;
extern const UInt_t DEFAULT_CONFIG;
extern const Char_t *RDKANAL,*RDKRAW,*RDKHOME;
extern Char_t *WORKDIR;
extern RDKConfig *rdk2config;

class RDKException: public std::exception{
private:
	Char_t description[256];
public:
	RDKException(const Char_t *);
	const Char_t *what() const throw(){return description;}
};

struct RawData{
public:
	Short_t ch[NCHANNELS][NPOINTS];
};

struct Recon{
public:
	Double_t t_e,E_e,t_p,E_p,t_q,E_q;
	Double_t t_gamma[14],E_gamma[14],chi2_gamma[14],R2_gamma[14];
	Double_t fitparam[14][6];
	Double_t nfit[14],noise[14];
	Double_t t_rise[14];
	Double_t t_half,t_dip;
	Double_t sigma_e,sigma_p,ep_offset,ep_chi2,ep_edm,ep_errdef;
	// Int_t ep_nvpar,ep_nparx; // not used?
	Int_t sat[14],tail[14],peak_pos[14],pre_E[14];
	Float_t disc[14],flat[14],adj_E[14];
	~Recon(){}
	static const Char_t *BRANCH;
};

struct Entry{
public:
	Double_t timestamp[2];
	Int_t nevent,nrun,nseries;
	static const Char_t *BRANCH;
};

struct BoardEvent{
public:
	BoardEvent();
	BoardEvent(Char_t,const Entry&,const Recon&,Int_t);
	Long64_t timestamp;
	Char_t type;
	Int_t nseries,nrun,nevent;
	Double_t t_e,E_e,t_half,t_dip,t_p,E_p,chi2_ep,sigma_e,sigma_p;
	Double_t t_q,E_q;
	Double_t t_gamma[7],E_gamma[7],noise[7],t_rise[7],chi2_gamma[7];
	Double_t g_param[7][6];
};

Bool_t operator<(const BoardEvent&,const BoardEvent&);

struct epEvent{
public:
	epEvent();
	epEvent(const epEvent&);
	epEvent(Char_t,const Entry&,const Recon&);
	epEvent(const BoardEvent&,const BoardEvent&,Int_t=-1);
	Double_t t_e,E_e,t_half,t_dip,t_p,E_p,chi2_ep,sigma_e,sigma_p;
	Double_t t_q,E_q;
	Double_t t_gamma[14],E_gamma[14],chi2_gamma[14],t_rise[14];
	Double_t g_param[14][6];
	Int_t M[14],grp[14];
	Long64_t timestamp[2];
	Int_t nseries,nrun,nevent,nevent0,nevent1;
	Char_t type;
	epEvent& operator=(const epEvent&);
	Bool_t operator==(const epEvent&) const;
	Bool_t operator!=(const epEvent&) const;
	Bool_t operator<(const epEvent&) const;
	static const Char_t *BRANCH;
};

std::ostream& operator<<(std::ostream&,const epEvent&);

struct NData{
public:
	Double_t start;
	Long64_t nn00,nn01,nn10,nn11,nn20,nn21,nn30,nn31;
	Long64_t nn,ne,np,nep,t;
	Int_t series,run;
	NData();
	NData& operator=(const NData &a);
	NData& operator=(const Int_t a[18]);
	NData& operator+=(const NData &a);
};

struct Task{
public:
	Task(const Char_t* ="",Int_t=0,Int_t=0,Char_t='/0',UInt_t=DEFAULT_CONFIG);
	const Char_t *path;
	Int_t series,run;
	Char_t type;
	UInt_t config;
};

Bool_t operator<(Task a,Task b);

std::ostream& operator<<(std::ostream&,const Task&);

#endif