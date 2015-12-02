#ifndef NERO_PUPPIFATJETS_H
#define NERO_PUPPIFATJETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BarePuppiFatJets.hpp"


class NeroPuppiFatJets : virtual public NeroCollection, 
    virtual public BarePuppiFatJets
{
    public:
        NeroPuppiFatJets();
        ~NeroPuppiFatJets();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroPuppiFatJets";};

        // --- specific fuctions
        // --- Handle
        edm::Handle<pat::JetCollection> handle;	

        // --- Token
        edm::EDGetTokenT<pat::JetCollection> token;

        // --- configuration
        float mMinPt;
        int   mMinNjets;
        float mMaxEta;
        string mMinId;


};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
