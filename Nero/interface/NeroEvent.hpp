#ifndef NERO_EVENT_H
#define NERO_EVENT_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Nero/interface/BareEvent.hpp"


class NeroEvent : virtual public NeroCollection, virtual public BareEvent
{
public:
	NeroEvent();
	~NeroEvent();
	int analyze(const edm::Event& iEvent);
	virtual inline string name(){return "NeroEvent";};
	
	// --- specific fuctions
	// --- Handle
	edm::Handle<double> rho_handle;

	// --- Token
	edm::EDGetTokenT<double> rho_token;


};


#endif
