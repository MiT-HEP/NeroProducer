#ifndef NERO_TRIGGER_H
#define NERO_TRIGGER_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareTrigger.hpp"

class NeroTrigger : virtual public NeroCollection, virtual public BareTrigger
{
public:
	NeroTrigger();
	~NeroTrigger();
	int analyze(const edm::Event& iEvent);
	virtual inline string name(){return "NeroTrigger";};
	
	// --- Handle
	edm::Handle< edm::TriggerResults  > handle;	
	edm::Handle<pat::PackedTriggerPrescales> prescale_handle;

	// --- Token
	edm::EDGetTokenT< edm::TriggerResults > token;
	edm::EDGetTokenT< pat::PackedTriggerPrescales > prescale_token;

	// --- configuration
	// vector<string> *triggerNames

};
#endif
