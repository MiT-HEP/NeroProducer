#ifndef NERO_EVENT_H
#define NERO_EVENT_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"


class NeroEvent : public NeroCollection
{
public:
	NeroEvent();
	~NeroEvent();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event& iEvent);
	
	// --- specific fuctions
	// --- Handle

	// --- Token

	// -- variables
	int isRealData;
	int runNum;
	int lumiNum;
	ULong64_t eventNum;
	

};


#endif
