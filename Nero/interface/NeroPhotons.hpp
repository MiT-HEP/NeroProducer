#ifndef NERO_PHOTONS_H
#define NERO_PHOTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BarePhotons.hpp"


class NeroPhotons : virtual public NeroCollection,
		virtual public BarePhotons
{
public:
	NeroPhotons();
	~NeroPhotons();
	int analyze(const edm::Event& iEvent);
	virtual inline string name(){return "NeroPhotons";};
	
	// --- specific fuctions
	// --- Handle
	edm::Handle<pat::PhotonCollection> handle;	

	edm::Handle<edm::ValueMap<bool> > medium_id;  

	edm::Handle<edm::ValueMap<float> > iso_ch;
	edm::Handle<edm::ValueMap<float> > iso_nh;
	edm::Handle<edm::ValueMap<float> > iso_pho;

	// --- Token
	edm::EDGetTokenT<pat::PhotonCollection> token;
	edm::EDGetTokenT<edm::ValueMap<bool> > medium_id_token;
	edm::EDGetTokenT<edm::ValueMap<float> > iso_ch_token;
	edm::EDGetTokenT<edm::ValueMap<float> > iso_nh_token;
	edm::EDGetTokenT<edm::ValueMap<float> > iso_pho_token;
	
	// --- configuration
	float mMinPt ;
	float mMaxIso ;

};


#endif
