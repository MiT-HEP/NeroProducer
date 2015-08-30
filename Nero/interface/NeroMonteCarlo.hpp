#ifndef NERO_MONTECARLO_H
#define NERO_MONTECARLO_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"
#include "NeroProducer/Nero/interface/NeroRunLumi.hpp"


class NeroMonteCarlo : virtual public NeroCollection,
    virtual public BareMonteCarlo,
    virtual public NeroRun
{
    public:
        NeroMonteCarlo();
        ~NeroMonteCarlo();
        int analyze(const edm::Event& iEvent);
        //void defineBranches(TTree *t);
        inline int analyzeRun(edm::Run const & iRun, TH1F* h)
        { return crossSection(iRun,h);}
        virtual inline string name(){return "NeroMonteCarlo";};

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

        // --- configuration
        float mMinGenParticlePt;
        float mMinGenJetPt;
        bool mParticleGun; 
        int isRealData;

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
