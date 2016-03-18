#ifndef NERO_LEPTONS_H
#define NERO_LEPTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareLeptons.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/NeroEvent.hpp"

// Electron corrector
#include "EgammaAnalysis/ElectronTools/interface/EnergyScaleCorrection_class.hh"

#include "TRandom3.h"

class NeroLeptons : virtual public NeroCollection,
    virtual public BareLeptons
{
    public:
        NeroLeptons();
        ~NeroLeptons();

        virtual int analyze(const edm::Event &)  ;
        virtual inline string name(){return "NeroLeptons";};
        //
        class myLepton{
            public:
                myLepton(){ chiso=-999; nhiso=-999; phoiso=-999; puiso=-999;mva = -999;}
                float iso;
                TLorentzVector p4;
                unsigned selBits;
                int pdgId;
                float pfPt;

                float chiso;
                float nhiso;
                float phoiso;
                float puiso;

                float mva;
        };

        // to be setted by the vertex
        //const reco::Vertex *pv_;
        //void inline SetPV( const reco::Vertex *pv){ pv_ = pv ; }
        //void inline SetPV( const reco::Vertex &pv){ pv_ = &pv ; }
        NeroVertex *vtx_ ;
        NeroEvent  *evt_ ; // rho 

        // Token
        edm::EDGetTokenT<pat::MuonCollection> mu_token;
        edm::EDGetTokenT<pat::ElectronCollection> el_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > 	el_vetoid_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > 	el_looseid_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > 	el_mediumid_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > 	el_tightid_token;
        edm::EDGetTokenT<edm::ValueMap<float> > el_mva_token;

        //edm::EDGetTokenT<edm::ValueMap<float> > 	el_iso_ch_token;
        //edm::EDGetTokenT<edm::ValueMap<float> > 	el_iso_nh_token;
        //edm::EDGetTokenT<edm::ValueMap<float> > 	el_iso_pho_token;

        // Handle
        edm::Handle<pat::MuonCollection> mu_handle;
        edm::Handle<pat::ElectronCollection> el_handle;
        edm::Handle<edm::ValueMap<bool> > el_veto_id;
        edm::Handle<edm::ValueMap<bool> > el_loose_id;
        edm::Handle<edm::ValueMap<bool> > el_medium_id;
        edm::Handle<edm::ValueMap<bool> > el_tight_id;
        edm::Handle<edm::ValueMap<float> > el_mva;

        //edm::Handle<edm::ValueMap<float> > el_iso_ch;
        //edm::Handle<edm::ValueMap<float> > el_iso_nh;
        //edm::Handle<edm::ValueMap<float> > el_iso_pho;

        //configuration
        float mMinPt_el;
        float mMinEta_el;
        float mMaxIso_el;

        float mMinPt_mu;
        float mMinEta_mu;
        float mMaxIso_mu;

        int mMinNleptons;

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
