#ifndef BARE_PHOTONS_H
#define BARE_PHOTONS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"


class BarePhotons : virtual public BareCollection
{
public:
	BarePhotons();
	~BarePhotons();
	void clear();
	void defineBranches(TTree *t);
	void setBranchAddresses(TTree*) ;
	virtual inline string name(){return "BarePhotons";};

	// -- variables
	TClonesArray *p4;
	vector<float> *sieie;
	vector<float> *iso;
    vector<int> *tightid;
    vector<int> *mediumid;
};

#endif
