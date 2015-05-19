#ifndef NERO_MONTECARLO_H
#define NERO_MONTECARLO_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"


class NeroMonteCarlo : public NeroCollection
{
public:
	NeroMonteCarlo();
	~NeroMonteCarlo();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event& iEvent);
	
	// --- specific fuctions
	int crossSection(edm::Run const & iRun, TH1F* h);
	// --- Handle
	edm::Handle<edm::View<pat::PackedGenParticle>> packed_handle;	
	edm::Handle<edm::View<reco::GenParticle> > pruned_handle;
	edm::Handle<GenEventInfoProduct> info_handle;
	edm::Handle< std::vector<PileupSummaryInfo> > pu_handle;
	edm::Handle<reco::GenJetCollection> jet_handle;
	edm::Handle<GenRunInfoProduct> runinfo_handle; 

	// --- Token
	edm::EDGetTokenT<edm::View<pat::PackedGenParticle> > packed_token;
	edm::EDGetTokenT<edm::View<reco::GenParticle> > pruned_token;
	edm::EDGetTokenT<GenEventInfoProduct> info_token;
	edm::EDGetTokenT< std::vector<PileupSummaryInfo> > pu_token;
	edm::EDGetTokenT<reco::GenJetCollection> jet_token;
	edm::EDGetTokenT<GenRunInfoProduct> runinfo_token;
	

	// -- variables
	TClonesArray *p4; // gen particles
	vector<int>  *pdgId;

	// genjets
	TClonesArray *jetP4;

	//
	int puTrueInt;
	//
	float mcWeight ;
        float qScale   ;
        float alphaQED ;
        float alphaQCD ;
        float x1       ;
        float x2       ;
        int   pdf1Id   ;
        int   pdf2Id   ;
        float scalePdf ;
	

};


#endif
