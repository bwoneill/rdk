//Runnable.h

#ifndef RUNNABLE_H
#define RUNNABLE_H

#include "Rtypes.h"

class Runnable{
	public:
		virtual Int_t run()=0;
		virtual void clean()=0;
};

#endif