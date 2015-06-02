#ifndef BARE_EVENT_H
#define BARE_EVENT_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareEvent : virtual public BareCollection
{
public:
	BareEvent();
	~BareEvent();
	void clear();
	void defineBranches(TTree*);
	void setBranchAddresses(TTree*);
	virtual inline string name(){ return "BareEvent";}

	// -- variables
	int isRealData;
	int runNum;
	int lumiNum;
	ULong64_t eventNum;
	
	float rho;	
	//
	// --- not good for CMSSW, 
	#ifdef TREE_STREAMER
	ClassDef(BareEvent,1);
	#endif
};

#endif
