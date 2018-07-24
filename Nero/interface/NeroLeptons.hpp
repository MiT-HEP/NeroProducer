#ifndef NERO_LEPTONS_H
#define NERO_LEPTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareLeptons.hpp"

#include "RecoEgamma/EgammaTools/interface/EffectiveAreas.h"
#include <memory>

// Electron corrector
#include "EgammaAnalysis/ElectronTools/interface/EnergyScaleCorrection_class.h"

#include "TRandom3.h"

class NeroLeptons : virtual public NeroCollection,
    virtual public BareLeptons
{
    public:
        NeroLeptons():NeroCollection(){};
        NeroLeptons(edm::ConsumesCollector & cc,edm::ParameterSet iConfig );

        ~NeroLeptons();

        virtual int analyze(const edm::Event &)  ;
        virtual inline string name(){return "NeroLeptons";};
        //
        class myLepton{
            public:
                myLepton(){ chiso=-999; nhiso=-999; phoiso=-999; puiso=-999;mva = -999; miniiso=-999;
                            etasc=0; sieie=0; sipip=0; sieip=0; r9=0; ecorr=1; 
                            resolution =-999; nlayers=-999;
                }
                float iso;
                TLorentzVector p4;
                unsigned selBits;
                int pdgId;
                float pfPt;

                float chiso;
                float nhiso;
                float phoiso;
                float puiso;
                float miniiso;

                float mva;

                float etasc;
                float sieie;
                float sipip;
                float sieip;
                float r9;
                float ecorr;

                float resolution;
                int nlayers;
            
        };

        enum ELEID{Veto,Loose,Medium,Tight};
        bool passEleId(const pat::Electron & , ELEID id, float iso);

        std::unique_ptr<EffectiveAreas> ea_;
    
        // --- Token
        edm::EDGetTokenT<reco::VertexCollection> vtx_token;
        edm::EDGetTokenT<pat::PackedCandidateCollection> token_pf;

        // Token
        edm::EDGetTokenT<pat::MuonCollection> mu_token;
        edm::EDGetTokenT<pat::ElectronCollection> el_token;
        //edm::EDGetTokenT<edm::ValueMap<bool> > 	el_vetoid_token;
        //edm::EDGetTokenT<edm::ValueMap<bool> > 	el_looseid_token;
        //edm::EDGetTokenT<edm::ValueMap<bool> > 	el_mediumid_token;
        //edm::EDGetTokenT<edm::ValueMap<bool> > 	el_tightid_token;
        //edm::EDGetTokenT<edm::ValueMap<bool> > 	el_hltid_token;
        //edm::EDGetTokenT<edm::ValueMap<float> > el_mva_token;

        edm::EDGetTokenT<pat::ElectronCollection> el_uncalib_token;

        edm::EDGetTokenT<EcalRecHitCollection> ebRecHits_token;

        //for miniiso
        edm::EDGetTokenT<double> rho_token;

        // Handle
        // --- Handle
        edm::Handle<pat::MuonCollection> mu_handle;
        edm::Handle<pat::ElectronCollection> el_handle;

        //edm::Handle<edm::ValueMap<bool> > el_veto_id;
        //edm::Handle<edm::ValueMap<bool> > el_loose_id;
        //edm::Handle<edm::ValueMap<bool> > el_medium_id;
        //edm::Handle<edm::ValueMap<bool> > el_tight_id;
        //edm::Handle<edm::ValueMap<bool> > el_hlt_id;
        //edm::Handle<edm::ValueMap<float> > el_mva;

        edm::Handle<pat::ElectronCollection> el_uncalib_handle;

        edm::Handle<reco::VertexCollection> vtx_handle;
        edm::Handle<pat::PackedCandidateCollection> handle_pf;

        // for miniiso
        edm::Handle<double> rho_handle;


        //configuration
        float mMinPt_el;
        float mMinEta_el;
        float mMaxIso_el;

        float mMinPt_mu;
        float mMinEta_mu;
        float mMaxIso_mu;

        int mMinNleptons;
        string mMinId;
        unsigned kMinId;

        unsigned idStringToEnum(std::string idString);

        // --- EGTools
        EnergyScaleCorrection_class *EleCorr{0};
        TRandom3 *rnd_{0};
        
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
