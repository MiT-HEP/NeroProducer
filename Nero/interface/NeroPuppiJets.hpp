#ifndef NERO_PUPPIJETS_H
#define NERO_PUPPIJETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BarePuppiJets.hpp"


//
#include "NeroProducer/Nero/interface/NeroPF.hpp"


class NeroPuppiJets : virtual public NeroCollection, virtual public BarePuppiJets
{
    public:
        NeroPuppiJets();
        ~NeroPuppiJets();
        int analyze(const edm::Event& iEvent, const edm::EventSetup&iSetup);
        int analyze(const edm::Event& iEvent){return 2;} // never called
        virtual inline string name(){return "NeroPuppiJets";};

        // --- specific fuctions
        static bool JetId(const pat::Jet &, string id);

        // --- Handle
        edm::Handle<pat::JetCollection> handle;	

        // --- Token
        edm::EDGetTokenT<pat::JetCollection> token;

        // --- configuration
        float mMinPt;
        int   mMinNjets;
        float mMinEta;
        string mMinId;

        // extra info
        NeroPF *pf;

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
