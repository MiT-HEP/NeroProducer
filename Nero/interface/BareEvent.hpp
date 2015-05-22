#ifndef BARE_EVENT_H
#define BARE_EVENT_H

#include "NeroProducer/Nero/interface/BareCollection.hpp"

class BareEvent : virtual public BareCollection 
{
public:
	BareEvent();
	~BareEvent();
	void clear();
	void defineBranches(TTree*);
	virtual inline string name(){ return "BareEvent";}

	// -- variables
	int isRealData;
	int runNum;
	int lumiNum;
	ULong64_t eventNum;
	
	double rho;	
};

#endif
