#ifndef NERO_TAUS_H
#define NERO_TAUS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareTaus.hpp"

class NeroTaus : virtual public NeroCollection,
		virtual public BareTaus
{
public:
	NeroTaus();
	~NeroTaus();
	int analyze(const edm::Event &)  ;
	virtual inline string name(){return "NeroTaus";};


	// Token
	edm::EDGetTokenT<pat::TauCollection> token ;	
	// Handle
	edm::Handle<pat::TauCollection> handle;

};


#endif
