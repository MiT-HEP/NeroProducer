#ifndef BARE_FATJETS_H
#define BARE_FATJETS_H

#include "NeroProducer/Nero/interface/BareCollection.hpp"


class BareFatJets : virtual public BareCollection
{
public:
	BareFatJets();
	~BareFatJets();
	void clear();
	void defineBranches(TTree *t);
	virtual inline string name(){return "BareFatJets";};

	// -- variables
	TClonesArray *p4;
	vector<float> *rawPt;
	vector<int>   *flavour;
	vector<float> *tau1;
	vector<float> *tau2;
	vector<float> *tau3;

	vector<float> *trimmedMass;
	vector<float> *prunedMass;
	vector<float> *filteredMass;
	vector<float> *softdropMass;

};

#endif
