#ifndef NERO_LEPTONS_H
#define NERO_LEPTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"

class NeroLeptons : public NeroCollection
{
public:
	NeroLeptons();
	~NeroLeptons();
	void clear();
	void defineBranches(TTree *t);
	virtual int analyze(const edm::Event &)  ;

	// ----
	TClonesArray  *p4;
	vector<float> *pdgId;	
	vector<float> *iso;	
	vector<int>   *tightId;	

	
	// to be setted by the vertex
	const reco::Vertex *pv_;
	void inline SetPV( const reco::Vertex *pv){ pv_ = pv ; }
	void inline SetPV( const reco::Vertex &pv){ pv_ = &pv ; }

	// Token
	edm::EDGetTokenT<pat::MuonCollection> mu_token;
	edm::EDGetTokenT<pat::ElectronCollection> el_token;
	edm::EDGetTokenT<edm::ValueMap<bool> > 	el_mediumid_token;
	edm::EDGetTokenT<edm::ValueMap<bool> > 	el_tightid_token;

	// Handle
	edm::Handle<pat::MuonCollection> mu_handle;
	edm::Handle<pat::ElectronCollection> el_handle;
	edm::Handle<edm::ValueMap<bool> > el_medium_id;
	edm::Handle<edm::ValueMap<bool> > el_tight_id;
	//edm::Handle<edm::ValueMap<float> > el_iso_ch;
	//edm::Handle<edm::ValueMap<float> > el_iso_nh;
	//edm::Handle<edm::ValueMap<float> > el_iso_pho;

};


#endif
