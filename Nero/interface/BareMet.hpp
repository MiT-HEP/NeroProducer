#ifndef BARE_MET_H
#define BARE_MET_H

#include "NeroProducer/Nero/interface/BareCollection.hpp"


class BareMet : virtual public BareCollection
{
public:
	BareMet();
	~BareMet();
	void clear();
	void defineBranches(TTree *t);
	virtual void setBranchAddresses(TTree*) ;
	virtual inline string name(){return "BareMet";};

	// -- variables
	TClonesArray *p4;
	vector<float> *ptJESUP;
	vector<float> *ptJESDOWN;

	// Should not stay here, but in MC -> we will esculed it if run on onlyMc
	TClonesArray *genP4;
};
#endif
