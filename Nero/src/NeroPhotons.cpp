#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroPhotons::NeroPhotons() : BarePhotons(){

    mMinPt = 15;
    mMaxIso = -1;
    mMinNpho = 0;
    mMinEta = 2.5;
}

NeroPhotons::~NeroPhotons(){
}


int NeroPhotons::analyze(const edm::Event& iEvent){

    if ( mOnlyMc  ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);

    // ID and ISO
    iEvent.getByToken(loose_id_token,loose_id);
    iEvent.getByToken(medium_id_token,medium_id);
    iEvent.getByToken(tight_id_token,tight_id);
    iEvent.getByToken(iso_ch_token, iso_ch);
    iEvent.getByToken(iso_nh_token, iso_nh);
    iEvent.getByToken(iso_pho_token, iso_pho);

    int iPho = -1;	
    for (auto &pho : *handle)
    {
        ++iPho;

        if (pho.pt() <15 or pho.chargedHadronIso()/pho.pt() > 0.3) continue;		
        if (fabs(pho.eta()) > mMinEta ) continue;
        if (pho.pt() < mMinPt) continue;

        edm::RefToBase<pat::Photon> ref ( edm::Ref< pat::PhotonCollection >(handle, iPho) ) ;
        float chIso =  (*iso_ch) [ref];
        float nhIso =  (*iso_nh) [ref];
        float phIso =  (*iso_pho)[ref];	
        float totIso = chIso + nhIso + phIso;

        bool isPassLoose  = (*loose_id)[ref];	
        bool isPassMedium = (*medium_id)[ref];	
        bool isPassTight  = (*tight_id)[ref];	

        if (not isPassLoose) continue;
        if (mMaxIso >=0 and totIso > mMaxIso) continue;

        //FILL
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(pho.px(),pho.py(),pho.pz(),pho.energy());
        iso->push_back(totIso);	
        sieie -> push_back( pho. full5x5_sigmaIetaIeta() );


        tightid->push_back(isPassTight);
        mediumid->push_back(isPassMedium);
    }

    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
