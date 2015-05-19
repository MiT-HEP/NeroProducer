#ifndef NERO_TAUS_H
#define NERO_TAUS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"

class NeroTaus : public NeroCollection
{
public:
	NeroTaus();
	~NeroTaus();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event &)  ;

	// --  members
	TClonesArray *p4;
	vector<float>           *id ;// byMediumCombinedIsolationDeltaBetaCorr3Hit ?
	vector<int>       	*Q ;// charge
	vector<float>           *M ; // mass
	vector<float>           *iso ;

	// Token
	edm::EDGetTokenT<pat::TauCollection> token ;	
	// Handle
	edm::Handle<pat::TauCollection> handle;

};


#endif
