#ifndef BARE_ALL_H
#define BARE_ALL_H

#include "NeroProducer/Nero/interface/BareCollection.hpp"

class BareAll : virtual public BareCollection 
{
/* container for the inforamations that will be saved for all events */
public:
	BareAll();
	~BareAll();
	void clear();
	void defineBranches(TTree*);
	virtual inline string name(){ return "BareAll";}

	// -- variables
	int isRealData;
	int runNum;
	int lumiNum;
	ULong64_t eventNum;

	int puTrueInt;
	float mcWeight;
	
};

#endif
