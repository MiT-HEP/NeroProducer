#ifndef NERO_MET_H
#define NERO_MET_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareMet.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"

class NeroMet : virtual public NeroCollection,
    virtual public BareMet
{
    public:
        NeroMet();
        ~NeroMet();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroMet";};

        bool rerunPuppi=false;

        // --- Handle
        edm::Handle<pat::METCollection> handle;	
        edm::Handle<pat::METCollection> handle_puppi;
        edm::Handle<reco::PFMETCollection> handle_puppiRerun;
        edm::Handle<reco::PFMETCollection> handle_puppiRerunUncorr;

        // --- Token
        edm::EDGetTokenT<pat::METCollection> token;
        edm::EDGetTokenT<pat::METCollection> token_puppi;
        edm::EDGetTokenT<reco::PFMETCollection> token_puppiRerun;
        edm::EDGetTokenT<reco::PFMETCollection> token_puppiRerunUncorr;
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
