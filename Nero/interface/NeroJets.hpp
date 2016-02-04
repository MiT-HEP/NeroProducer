#ifndef NERO_JETS_H
#define NERO_JETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareJets.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"

#include "DataFormats/PatCandidates/interface/JetCorrFactors.h"

// --- JEC UNCERTAINTY ---
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
//  ----


class NeroJets : virtual public NeroCollection, virtual public BareJets
{
    public:
        NeroJets();
        ~NeroJets();
        int analyze(const edm::Event& iEvent, const edm::EventSetup&iSetup);
        int analyze(const edm::Event& iEvent){return 2;} // never called
        virtual inline string name(){return "NeroJets";};

        // --- specific fuctions
        static bool JetId(const pat::Jet &, string id);
        // --- Handle
        edm::Handle<pat::JetCollection> handle;	
        edm::Handle<edm::ValueMap<float> > qg_handle;
        edm::Handle<edm::ValueMap<pat::JetCorrFactors> > jec_handle;
    
        // --- Token
        edm::EDGetTokenT<pat::JetCollection> token;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token;
        edm::EDGetTokenT<edm::ValueMap<pat::JetCorrFactors> > jec_token;

        // --- configuration
        float mMinPt;
        int   mMinNjets;
        float mMinEta;
        string mMinId;
        bool mApplyJEC;

        // 
        NeroPF *pf;
        NeroVertex *vtx;
        
        // JES
        void InitJes(const edm::EventSetup& iSetup);

// JES
    private:
        JetCorrectionUncertainty *jecUnc_;
        bool isJecUncSet_;
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
