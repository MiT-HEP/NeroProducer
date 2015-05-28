#ifndef NERO_JETS_H
#define NERO_JETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareJets.hpp"


class NeroJets : virtual public NeroCollection, virtual public BareJets
{
public:
	NeroJets();
	~NeroJets();
	int analyze(const edm::Event& iEvent);
	virtual inline string name(){return "NeroJets";};
	
	// --- specific fuctions
	static bool JetId(const pat::Jet &);
	// --- Handle
	edm::Handle<pat::JetCollection> handle;	
	edm::Handle<edm::ValueMap<float>> qg_handle;

	// --- Token
	edm::EDGetTokenT<pat::JetCollection> token;
	edm::EDGetTokenT<edm::ValueMap<float> > qg_token;

	// --- configuration
	float mMinPt;
	int   mMinNjets;

};


#endif
