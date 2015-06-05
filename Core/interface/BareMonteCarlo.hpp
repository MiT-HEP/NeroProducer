#ifndef BARE_MONTECARLO_H
#define BARE_MONTECARLO_H

#include "NeroProducer/Core/interface/BareCollection.hpp"


class BareMonteCarlo : virtual public BareCollection
{
public:
	BareMonteCarlo();
	~BareMonteCarlo();
	void clear();
	void defineBranches(TTree *t);
	void setBranchAddresses(TTree*) ;
	virtual inline string name(){return "BareMonteCarlo";};

	// -- variables
	TClonesArray *p4; // gen particles
	vector<int>  *pdgId;

	// genjets
	TClonesArray *jetP4;

	//
	int puTrueInt;
	//
	float mcWeight ;
        float qScale   ;
        float alphaQED ;
        float alphaQCD ;
        float x1       ;
        float x2       ;
        int   pdf1Id   ;
        int   pdf2Id   ;
        float scalePdf ;

};
#endif
