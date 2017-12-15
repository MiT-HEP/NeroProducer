#ifndef NERO_TRACK_JETS_H
#define NERO_TRACK_JETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareTrackJets.hpp"



class NeroTrackJets : virtual public NeroCollection, virtual public BareTrackJets
{
    public:
        NeroTrackJets():NeroCollection(){};
        NeroTrackJets(edm::ConsumesCollector & cc,edm::ParameterSet iConfig );
        ~NeroTrackJets();
        int analyze(const edm::Event& iEvent){return 2;} // never called
        int analyze(const edm::Event& iEvent, const edm::EventSetup&iSetup)override;
        virtual inline string name(){return "NeroTrackJets";};

        // --- Handle
        edm::Handle< std::vector<reco::PFJet> > handle;	

        // --- Token
        edm::EDGetTokenT< std::vector<reco::PFJet> > token;
        // --- configuration
        float mMinPt;

    private:
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
