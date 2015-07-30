#ifndef NERO_PHOTONS_H
#define NERO_PHOTONS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BarePhotons.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/NeroEvent.hpp"


class NeroPhotons : virtual public NeroCollection,
    virtual public BarePhotons
{
    public:
        NeroPhotons();
        ~NeroPhotons();
        int analyze(const edm::Event& iEvent);
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

        // --- Token
        edm::EDGetTokenT<pat::PhotonCollection> token;
        edm::EDGetTokenT<edm::ValueMap<bool> > loose_id_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > medium_id_token;
        edm::EDGetTokenT<edm::ValueMap<bool> > tight_id_token;
        edm::EDGetTokenT<edm::ValueMap<float> > iso_ch_token;
        edm::EDGetTokenT<edm::ValueMap<float> > iso_nh_token;
        edm::EDGetTokenT<edm::ValueMap<float> > iso_pho_token;

        // --- RC

        // --- configuration
        float mMinPt;
        int   mMinNpho;
        float mMinEta;
        float mMaxIso;

        // -- PF
        NeroPF *pf;
        NeroJets *jets;
        // -- rho
        NeroEvent *evt;
        
        bool cutBasedPhotonId( const pat::Photon& pho, string type="loose_50ns", bool withIso = true , bool withSieie=true);

        float cutBasedPhotonIdEffArea( const pat::Photon&pho, string type="ch_50ns");
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
