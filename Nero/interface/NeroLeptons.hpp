#ifndef NERO_LEPTONS_H
#define NERO_LEPTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareLeptons.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"


class NeroLeptons : virtual public NeroCollection,
	virtual public BareLeptons
{
public:
	NeroLeptons();
	~NeroLeptons();

	virtual int analyze(const edm::Event &)  ;
	virtual inline string name(){return "NeroLeptons";};
	//
	class myLepton{
		public:
		float iso;
		TLorentzVector p4;
		int tightId;
		int pdgId;
		// ---
	};
	
	// to be setted by the vertex
	//const reco::Vertex *pv_;
	//void inline SetPV( const reco::Vertex *pv){ pv_ = pv ; }
	//void inline SetPV( const reco::Vertex &pv){ pv_ = &pv ; }
	NeroVertex *vtx_ ;

	// Token
	edm::EDGetTokenT<pat::MuonCollection> mu_token;
	edm::EDGetTokenT<pat::ElectronCollection> el_token;
	edm::EDGetTokenT<edm::ValueMap<bool> > 	el_mediumid_token;
	edm::EDGetTokenT<edm::ValueMap<bool> > 	el_tightid_token;

	//edm::EDGetTokenT<edm::ValueMap<float> > 	el_iso_ch_token;
	//edm::EDGetTokenT<edm::ValueMap<float> > 	el_iso_nh_token;
	//edm::EDGetTokenT<edm::ValueMap<float> > 	el_iso_pho_token;

	// Handle
	edm::Handle<pat::MuonCollection> mu_handle;
	edm::Handle<pat::ElectronCollection> el_handle;
	edm::Handle<edm::ValueMap<bool> > el_medium_id;
	edm::Handle<edm::ValueMap<bool> > el_tight_id;

	//edm::Handle<edm::ValueMap<float> > el_iso_ch;
	//edm::Handle<edm::ValueMap<float> > el_iso_nh;
	//edm::Handle<edm::ValueMap<float> > el_iso_pho;

	//configuration
	float mMinPt;
	int   mMinNleptons;

};


#endif
