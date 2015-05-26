#ifndef BARE_JETS_H
#define BARE_JETS_H

#include "NeroProducer/Nero/interface/BareCollection.hpp"

class BareJets : virtual public BareCollection
{
public:
	BareJets();
	~BareJets();
	void clear();
	void defineBranches(TTree*);
	virtual void setBranchAddresses(TTree*);
	virtual inline string name(){ return "BareJets";}

	// -- variables
	// -- variables
	TClonesArray *p4;
	vector<float> *rawPt;
	vector<float> *bDiscr;
	vector<float> *puId;
	vector<float> *unc;
	vector<float> *qgl;
	vector<int>   *flavour;

};

#endif


