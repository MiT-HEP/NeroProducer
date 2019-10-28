#ifndef NERO_JETS_H
#define NERO_JETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareJets.hpp"


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
        NeroJets():NeroCollection(){};
        NeroJets(edm::ConsumesCollector & cc,edm::ParameterSet iConfig );
        ~NeroJets();
        int analyze(const edm::Event& iEvent, const edm::EventSetup&iSetup);
        int analyze(const edm::Event& iEvent){return 2;} // never called
        virtual inline string name(){return "NeroJets";};

        // --- specific fuctions
        static bool JetId(const pat::Jet &, string id);
        // --- Handle
        edm::Handle<pat::JetCollection> handle;	
        edm::Handle<edm::ValueMap<float>> qg_handle;
        edm::Handle<edm::ValueMap<float>> puid_handle;

        edm::Handle<edm::ValueMap<int>> qg_handle_Mult;
        edm::Handle<edm::ValueMap<float>> qg_handle_PtD;
        edm::Handle<edm::ValueMap<float>> qg_handle_Axis2;
        //
        edm::Handle<edm::ValueMap<float>> qg_handle_Axis1;
        edm::Handle<edm::ValueMap<int>> qg_handle_cmult;
        edm::Handle<edm::ValueMap<int>> qg_handle_nmult;
        edm::Handle<edm::ValueMap<float>> qg_handle_pt_dr_log;

        edm::Handle<edm::ValueMap<float>> bcorr_handle;
        edm::Handle<edm::ValueMap<float>> bcorrunc_handle;

        edm::Handle<edm::View<reco::GenParticle> > pruned_handle;
        edm::Handle<double> rho_handle;
        edm::EDGetTokenT<double> rho_token;

        // --- Token
        edm::EDGetTokenT<pat::JetCollection> token;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token;
        edm::EDGetTokenT<edm::ValueMap<float> > puid_token;
        bool redoPU{false};

        edm::EDGetTokenT<edm::ValueMap<int> >   qg_token_Mult;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token_PtD;
        edm::EDGetTokenT<edm::ValueMap<float> > qg_token_Axis2;

        edm::EDGetTokenT<edm::ValueMap<float>> qg_token_Axis1;
        edm::EDGetTokenT<edm::ValueMap<int>> qg_token_cmult;
        edm::EDGetTokenT<edm::ValueMap<int>> qg_token_nmult;
        edm::EDGetTokenT<edm::ValueMap<float>> qg_token_pt_dr_log;

        edm::EDGetTokenT<edm::ValueMap<float>> bcorr_token;
        edm::EDGetTokenT<edm::ValueMap<float>> bcorrunc_token;

        edm::EDGetTokenT<reco::GenJetCollection> gen_token;
        map<string, edm::EDGetTokenT<edm::ValueMap<float> > > qg_dR_tokens_f;
        map<string, edm::EDGetTokenT<edm::ValueMap<int> > > qg_dR_tokens_i;
        edm::EDGetTokenT<edm::View<reco::GenParticle> > pruned_token;
        // --- configuration
        float mMinPt;
        int   mMinNjets;
        float mMinEta;
        string mMinId;

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
