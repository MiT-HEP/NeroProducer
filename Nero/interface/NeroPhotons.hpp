#ifndef NERO_PHOTONS_H
#define NERO_PHOTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BarePhotons.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/SuperClusterFootprintRemovalMiniAOD.h"
#include "RecoEcal/EgammaCoreTools/interface/EcalClusterLazyTools.h"

// Electron corrector
//#include "EgammaAnalysis/ElectronTools/interface/EnergyScaleCorrection_class.hh"
#include "TRandom3.h"


class NeroPhotons : virtual public NeroCollection,
    virtual public BarePhotons
{
    public:
        NeroPhotons();
        ~NeroPhotons();
        int analyze(const edm::Event& iEvent) {return 2;}; // this function should never be called
        int analyze(const edm::Event& iEvent,const edm::EventSetup &iSetup);
        virtual inline string name(){return "NeroPhotons";};

        // --- specific fuctions
        // --- Handle
        edm::Handle<pat::PhotonCollection> handle;	

        edm::Handle<edm::ValueMap<bool> > loose_id;  
        edm::Handle<edm::ValueMap<bool> > medium_id;  
        edm::Handle<edm::ValueMap<bool> > tight_id;  

        edm::Handle<edm::ValueMap<float> > iso_ch;
        edm::Handle<edm::ValueMap<float> > iso_nh;
        edm::Handle<edm::ValueMap<float> > iso_pho;
        edm::Handle<edm::ValueMap<float> > iso_wch;

        // --- Token
        edm::EDGetTokenT<pat::PhotonCollection> token;
        edm::EDGetTokenT<edm::ValueMap<bool> > loose_id_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > medium_id_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > tight_id_token;
        edm::EDGetTokenT<edm::ValueMap<float> > iso_ch_token;
        edm::EDGetTokenT<edm::ValueMap<float> > iso_nh_token;
        edm::EDGetTokenT<edm::ValueMap<float> > iso_pho_token;
        edm::EDGetTokenT<edm::ValueMap<float> > iso_wch_token;
        
        edm::EDGetTokenT<EcalRecHitCollection> ebRecHits_token;
        edm::EDGetTokenT<EcalRecHitCollection> eeRecHits_token;
    

        EcalClusterLazyTools *clusterTools = 0;

        // --- RC

        // --- configuration
        float mMinPt;
        int   mMinNpho;
        float mMaxEta;
        float mMaxIso;
        string mMinId;
        unsigned kMinId;

        // -- PF
        NeroPF *pf;
        NeroJets *jets;
        // -- FPR
        NeroVertex *vtx;
        NeroLeptons*leps;
        // -- rho
        NeroEvent *evt;

        // needed to be constructed during the plugin construction
        //SuperClusterFootprintRemovalMiniAOD  *fpr;

        unsigned idStringToEnum(std::string idString);

        bool cutBasedPhotonId( const pat::Photon& pho, string type="loose_50ns", bool withIso = true , bool withSieie=true);

        float cutBasedPhotonIdEffArea( const pat::Photon&pho, string type="ch_50ns");

        // --- EGTools
        //EnergyScaleCorrection_class *PhoCorr{0};
        //TRandom3 *rnd_{0};

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
