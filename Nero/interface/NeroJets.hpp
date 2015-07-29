#ifndef NERO_JETS_H
#define NERO_JETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareJets.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"


class NeroJets : virtual public NeroCollection, virtual public BareJets
{
    public:
        NeroJets();
        ~NeroJets();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroJets";};

        // --- specific fuctions
        static bool JetId(const pat::Jet &, string id);
        // --- Handle
        edm::Handle<pat::JetCollection> handle;	
        edm::Handle<edm::ValueMap<float>> qg_handle;

        // --- Token
        edm::EDGetTokenT<pat::JetCollection> token;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token;

        // --- configuration
        float mMinPt;
        int   mMinNjets;
        float mMinEta;
        string mMinId;

        // 
        NeroPF *pf;
        NeroVertex *vtx;
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
