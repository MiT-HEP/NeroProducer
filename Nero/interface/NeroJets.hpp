#ifndef NERO_JETS_H
#define NERO_JETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareJets.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"


// --- JEC UNCERTAINTY ---
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "JetMETCorrections/Objects/interface/JetCorrectionsRecord.h"
#include "JetMETCorrections/Objects/interface/JetCorrector.h"
//  ----
#include "TRandom3.h"

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
        edm::Handle<edm::ValueMap<float>> qg_handle;

        edm::Handle<edm::ValueMap<int>> qg_handle_Mult;
        edm::Handle<edm::ValueMap<float>> qg_handle_PtD;
        edm::Handle<edm::ValueMap<float>> qg_handle_Axis2;
        //
        edm::Handle<edm::ValueMap<float>> qg_handle_Axis1;
        edm::Handle<edm::ValueMap<int>> qg_handle_cmult;
        edm::Handle<edm::ValueMap<int>> qg_handle_nmult;
        edm::Handle<edm::ValueMap<float>> qg_handle_pt_dr_log;

        // --- Token
        edm::EDGetTokenT<pat::JetCollection> token;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token;

        edm::EDGetTokenT<edm::ValueMap<int> >   qg_token_Mult;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token_PtD;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token_Axis2;

        edm::EDGetTokenT<edm::ValueMap<float>> qg_token_Axis1;
        edm::EDGetTokenT<edm::ValueMap<int>> qg_token_cmult;
        edm::EDGetTokenT<edm::ValueMap<int>> qg_token_nmult;
        edm::EDGetTokenT<edm::ValueMap<float>> qg_token_pt_dr_log;

        edm::EDGetTokenT<reco::GenJetCollection> gen_token;
        map<string, edm::EDGetTokenT<edm::ValueMap<float> > > qg_dR_tokens_f;
        map<string, edm::EDGetTokenT<edm::ValueMap<int> > > qg_dR_tokens_i;
        // --- configuration
        float mMinPt;
        int   mMinNjets;
        float mMinEta;
        string mMinId;

        // 
        NeroPF *pf;
        NeroVertex *vtx;
        NeroEvent *evt;
        NeroMonteCarlo *mc;
        
        // JES
        void InitJes(const edm::EventSetup& iSetup);

    private:
        // JES
        JetCorrectionUncertainty *jecUnc_;
        bool isJecUncSet_;

        //JER 
        TRandom3*rnd_{0};
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
