#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroPhotons::NeroPhotons() : BarePhotons(){

    mMinPt = 15;
    mMaxIso = -1;
    mMinNpho = 0;
    mMinEta = 2.5;

    pf = NULL;
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
        float _chIso_ =  (*iso_ch) [ref];
        float _nhIso_ =  (*iso_nh) [ref];
        float _phIso_ =  (*iso_pho)[ref];	
        float _puIso_ =  -1 ;  // TODO
        float totIso = _chIso_ + _nhIso_ + _phIso_;

        bool isPassLoose  = (*loose_id)[ref];	
        bool isPassMedium = (*medium_id)[ref];	
        bool isPassTight  = (*tight_id)[ref];	

        if (not isPassLoose) continue;
        if (mMaxIso >=0 and totIso > mMaxIso) continue;

        // RC -- no FPR for the moment TODO
        //
        float _chIsoRC_ = 0;
        float _nhIsoRC_ = 0;
        float _phIsoRC_ = 0;
        float _puIsoRC_ = 0;

        // allowed dphi
        float dphis[] = { 0.5 * 3.14159 , -.5 *3.14159, 0.25*3.14150 , -0.25*3.14159, 0.75*3.14159,-0.75*3.14159} ;

        float DR=0.3; // close obj
        float DRCone=0.3; // default value for iso https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPFBasedIsolationRun2

        float dphi = -100;

        //select dphi
        for(unsigned i =0 ;i< sizeof(dphis)/sizeof(float) ;++i)
        {
            float dphi_cand = dphis[i];
            bool close_obj = false;
            for(auto &j : *jets->handle) 
            {
                if (j.pt() <20 or j.eta() >2.5) continue; // it's own set of jets
                TLorentzVector v1,v2;
                v1.SetPtEtaPhiM( pho.pt(),pho.eta(),pho.phi() + dphi_cand,0 ) ;
                v2.SetPtEtaPhiM( j.pt(),j.eta(),j.phi(),j.mass() );
                if (v1.DeltaR(v2) <DR ) close_obj = true; 
            } 
            if ( not close_obj ) 
            {
            dphi = dphi_cand;
            break;
            }
        }

        if (dphi <-99)
        {
        _chIsoRC_ = -1;
        _nhIsoRC_ = -1;
        _phIsoRC_ = -1;
        _puIsoRC_ = -1;
        }
        else
        {
            for( auto &cand : *pf->handle )
            {
                TLorentzVector v1,v2;
                v1.SetPtEtaPhiM( pho.pt(),pho.eta(),pho.phi() + dphi,0 ) ;
                v2.SetPtEtaPhiM( cand.pt(),cand.eta(),cand.phi(),cand.mass() );
                if (v1.DeltaR(v2) < DRCone )
                {
                     // TO BE CHECKED !!! TODO FIXME
                if (cand.charge() != 0  and abs(cand.pdgId())>20 and  fabs( cand.dz() ) <=0.1 and cand.fromPV()>1 and cand.trackHighPurity() ) _chIsoRC_ += cand.pt();
                if (cand.charge() == 0 and cand.pdgId() == 22 ) _phIsoRC_ += cand.pt();
                if (cand.charge() == 0 and cand.pdgId() != 22 ) _nhIsoRC_ += cand.pt();
                if (cand.charge() != 0 and abs(cand.pdgId() )>20 and ( 
                            fabs( cand.dz() ) >0.1  or cand.fromPV()<=1 or not cand.trackHighPurity() 
                            ) ) _puIsoRC_ += cand.pt(); 
                }

            }
        }
    
        //FILL
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(pho.px(),pho.py(),pho.pz(),pho.energy());
        iso->push_back(totIso);	
        sieie -> push_back( pho. full5x5_sigmaIetaIeta() );


        tightid->push_back(isPassTight);
        mediumid->push_back(isPassMedium);

        chIso -> push_back( _chIso_);
        phoIso -> push_back( _phIso_ ) ;
        nhIso -> push_back ( _nhIso_ ) ;
        puIso -> push_back ( _puIso_ ) ;

        chIsoRC -> push_back( _chIsoRC_);
        phoIsoRC -> push_back( _phIsoRC_ ) ;
        nhIsoRC -> push_back ( _nhIsoRC_ ) ;
        puIsoRC -> push_back ( _puIsoRC_ ) ;
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
