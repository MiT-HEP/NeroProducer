#ifndef NERO_MET_RECLUSTER_H
#define NERO_MET_RECLUSTER_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareMet.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"


class NeroMetRecluster : virtual public NeroCollection,
    virtual public BareMet
{
    /*
     * This class is supposed to substitute NeroMet, if we recompute it from Reclustered objects
     */
    public:
        NeroMetRecluster();
        ~NeroMetRecluster();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroMetRecluster";};

        // --- Handle
        edm::Handle<pat::METCollection> handle;	
        edm::Handle<reco::PFMETCollection> handle_puppi;
        edm::Handle<reco::PFMETCollection> handle_puppiUncorr;

        // --- Token
        edm::EDGetTokenT<pat::METCollection> token;
        edm::EDGetTokenT<reco::PFMETCollection> token_puppi;
        edm::EDGetTokenT<reco::PFMETCollection> token_puppiUncorr;
        //
        NeroPF * pf;

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
