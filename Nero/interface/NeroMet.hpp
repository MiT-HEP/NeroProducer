#ifndef NERO_MET_H
#define NERO_MET_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareMet.hpp"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETCollection.h"

class NeroMet : virtual public NeroCollection,
    virtual public BareMet
{
    public:
        NeroMet():NeroCollection(){};
        NeroMet(edm::ConsumesCollector & cc,edm::ParameterSet iConfig );
        ~NeroMet();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroMet";};



        // --- Token
        edm::EDGetTokenT<pat::PackedCandidateCollection> token_pf;
        edm::EDGetTokenT<pat::METCollection> token;
        edm::EDGetTokenT<pat::METCollection> token_puppi;
        //edm::EDGetTokenT<pat::METCollection> token_cleanmu;
        //edm::EDGetTokenT<pat::METCollection> token_cleaneg;
        //edm::EDGetTokenT<pat::METCollection> token_unclean;
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
