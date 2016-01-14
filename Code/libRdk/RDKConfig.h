//RDKConfig.h

#ifndef RDKCONFIG_H
#define RDKCONFIG_H

#define BGO_MODE 1U
#define BAPD_MODE 2U
#define BAPD_PERP 3U

#include <map>

#include "TString.h"

class RDKConfig{
private:
	std::map<UInt_t,UInt_t> config_map;
public:
	RDKConfig(TString);
	~RDKConfig();
	UInt_t getConfig(UInt_t);
	static UInt_t getMode(UInt_t,UInt_t);
};

#endif