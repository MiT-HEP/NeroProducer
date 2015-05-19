#ifndef NERO_PHOTONS_H
#define NERO_PHOTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"


class NeroPhotons : public NeroCollection
{
public:
	NeroPhotons();
	~NeroPhotons();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event& iEvent);
	
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

	// -- variables
	TClonesArray *p4;
	vector<float> *sieie;
	vector<float> *iso;

};


#endif
