#ifndef NERO_TAUS_H
#define NERO_TAUS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareTaus.hpp"

class NeroTaus : virtual public NeroCollection,
    virtual public BareTaus
{
    public:
        NeroTaus():NeroCollection(){};
        NeroTaus(edm::ConsumesCollector & cc,edm::ParameterSet iConfig );
        ~NeroTaus();
        int analyze(const edm::Event &)  ;
        virtual inline string name(){return "NeroTaus";};


        // Token
        edm::EDGetTokenT<pat::TauCollection> token ;	
        // Handle
        edm::Handle<pat::TauCollection> handle;

        // --- configuration
        float mMinPt;
        int   mMinNtaus;
        float mMinEta;
        string mMinId;
        float mMaxIso;
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
