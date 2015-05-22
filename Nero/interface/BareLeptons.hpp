#ifndef BARE_LEPTONS_H
#define BARE_LEPTONS_H

#include "NeroProducer/Nero/interface/BareCollection.hpp"

class BareLeptons : virtual public BareCollection
{
public:
	BareLeptons();
	~BareLeptons();
	void clear();
	void defineBranches(TTree *t);
	virtual inline string name(){return "BareLeptons";};
	// ----
	TClonesArray  *p4;
	vector<float> *pdgId;	
	vector<float> *iso;	
	vector<int>   *tightId;	


};

#endif
