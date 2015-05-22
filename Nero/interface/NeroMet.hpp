#ifndef NERO_MET_H
#define NERO_MET_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Nero/interface/BareMet.hpp"


class NeroMet : virtual public NeroCollection,
		virtual public BareMet
{
public:
	NeroMet();
	~NeroMet();
	int analyze(const edm::Event& iEvent);
	virtual inline string name(){return "NeroMet";};
	
	// --- Handle
	edm::Handle<pat::METCollection> handle;	

	// --- Token
	edm::EDGetTokenT<pat::METCollection> token;


};


#endif
