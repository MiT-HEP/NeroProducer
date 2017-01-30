#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"
#include "NeroProducer/Nero/interface/MiniIsolation.hpp"
#include "NeroProducer/Nero/interface/NeroFunctions.hpp"
#include <time.h>

// -- Electron Isolation
NeroLeptons::NeroLeptons(): 
        NeroCollection(),
        BareLeptons()
{
    vtx_   = NULL;
    evt_   = NULL;

    mMinPt_mu = 10;
    mMinEta_mu = 2.4;
    mMaxIso_mu = 0.2;

    mMinPt_el = 10;
    mMinEta_el = 2.5;
    mMaxIso_el = -1.;

    mMinNleptons = 0;    

    rnd_ = new TRandom3( ) ;
}

NeroLeptons::~NeroLeptons(){
    delete EleCorr; 
    delete rnd_; 
}

unsigned NeroLeptons::idStringToEnum(std::string idString)
{
    unsigned idEnum = 0;
    if (idString == "veto")          { idEnum = LepVeto;     }
    else if (idString == "loose")    { idEnum = LepLoose;    }
    else if (idString == "medium")   { idEnum = LepMedium;   }
    else if (idString == "tight")    { idEnum = LepTight;    }
    else if (idString == "none")     { idEnum = LepBaseline; }
    // ask for any other id except Lep Baseline
    else if (idString == "any" )     { idEnum = ~LepBaseline; } 
    return idEnum;
}
int NeroLeptons::analyze(const edm::Event & iEvent)
{
    if ( mOnlyMc  ) return 0;

    kMinId = idStringToEnum(mMinId);
    
    if ( vtx_ == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Vertex Class not set."<<endl;
    if ( vtx_ -> GetPV() == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Primary Vertex not set."<<endl;
    if ( evt_ == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Event Class not set."<<endl;

    iEvent.getByToken(mu_token,mu_handle);	
    iEvent.getByToken(el_token,el_handle);	

    iEvent.getByToken(el_mediumid_token,el_medium_id);
    iEvent.getByToken(el_tightid_token,el_tight_id);
    iEvent.getByToken(el_vetoid_token,el_veto_id);
    iEvent.getByToken(el_looseid_token,el_loose_id);
    iEvent.getByToken(el_mva_token,el_mva);
    iEvent.getByToken(el_hltid_token,el_hlt_id);
    iEvent.getByToken(rho_token,rho_handle);

    edm::Handle<EcalRecHitCollection> ebRecHits;
    //edm::Handle<EcalRecHitCollection> eeRecHits;
    iEvent.getByToken(ebRecHits_token,ebRecHits);
    //iEvent.getByToken(eeRecHits_token,eeRecHits);

    if ( not mu_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] mu_handle is not valid"<<endl;
    if ( not el_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_handle is not valid"<<endl;
    if ( not el_medium_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_medium_id is not valid"<<endl;
    if ( not el_tight_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_tight_id is not valid"<<endl;
    if ( not el_veto_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_veto_id is not valid"<<endl;
    if ( not el_loose_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_loose_id is not valid"<<endl;
    if ( not el_hlt_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_hlt_id is not valid"<<endl;
    if ( not el_mva.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_mva is not valid"<<endl;
    if ( not rho_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] rho_handle is not valid"<<endl;

    vector<myLepton> leptons;

    for (const pat::Muon &mu : *mu_handle) {
        // selection
        if (mu.pt() < 5. ) continue;
        if (mu.pt() < mMinPt_mu || fabs(mu.eta()) > mMinEta_mu ) continue; 
        float chiso  = mu.pfIsolationR04().sumChargedHadronPt;
        float niso   = mu.pfIsolationR04().sumNeutralHadronEt;
        float phoiso = mu.pfIsolationR04().sumPhotonEt;
        float puiso = mu.pfIsolationR04().sumPUPt;
        float totiso = chiso + TMath::Max( niso + phoiso - .5*puiso, 0. ) ;
        bool isFake = 0;

        if ( mMaxIso_mu > 0 and totiso/mu.pt() > mMaxIso_mu ) continue;
        if ( totiso/mu.pt() < 0.4 && mu.isolationR03().sumPt/mu.pt() < 0.4 ){ isFake = 1; }

        myLepton l;
        l.pdgId = -mu.charge()*13;
        l.iso = totiso;
        l.p4.SetPxPyPzE( mu.px(),mu.py(),mu.pz(),mu.energy());
        l.selBits =  0 ;
            l.selBits |= LepBaseline;  
            l.selBits |= unsigned(mu.isLooseMuon()) * LepVeto;  // fill veto bit with loose info
            l.selBits |= unsigned(mu.isLooseMuon()) * LepLoose;
            l.selBits |= unsigned(mu.isTightMuon( * vtx_->GetPV() ))*LepTight ;
            l.selBits |= unsigned(mu.isMediumMuon() * LepMedium);

            if ( fabs((mu.muonBestTrack()->dz((*vtx_->GetPV()).position())))<0.1 and (mu.dB()< 0.02) ){
                l.selBits |= unsigned(mu.isMediumMuon() * LepMediumIP);
                l.selBits |= unsigned(mu.isTightMuon( * vtx_->GetPV() ))*LepTightIP ;
                l.selBits |= unsigned(mu.isSoftMuon(* vtx_->GetPV())) * LepSoftIP;
                if(isFake){ l.selBits |= unsigned(mu.isTightMuon(* vtx_->GetPV()) * LepFake); }
                }
            l.selBits |= unsigned(mu.isStandAloneMuon() * MuStandalone);
            l.selBits |= unsigned(mu.isTrackerMuon() * MuTracker);
            l.selBits |= unsigned(mu.isGlobalMuon() * MuGlobal);
            
        l.pfPt = mu.pfP4().pt();

        l.chiso  = chiso;
        l.nhiso  = niso;
        l.phoiso = phoiso;
        l.puiso  = puiso;
        //l.miniiso = getPFMiniIsolation_DeltaBeta(pf_->handle, dynamic_cast<const reco::Candidate *>(&mu), 0.05, 0.2, 10., false);
        l.miniiso = getPFMiniIsolation_EffectiveArea(pf_->handle, dynamic_cast<const reco::Candidate *>(&mu), 0.05, 0.2, 10., false, false, *rho_handle );
        
        if ( not (l.selBits & kMinId) ) continue;
        leptons.push_back(l);
    }

    // Electron LOOP
    int iEle = -1;
    for (const pat::Electron &el : *el_handle)
    {
        iEle ++;

        if ( el.pt() < 10 ) continue;
        if ( el.pt() < mMinPt_el || fabs(el.eta()) > mMinEta_el ) continue;

        edm::RefToBase<pat::Electron> ref ( edm::Ref< pat::ElectronCollection >(el_handle, iEle) ) ;

        bool isPassVeto = (*el_veto_id)[ref] and el.passConversionVeto();
        bool isPassTight = (*el_tight_id)[ref] and el.passConversionVeto();
        bool isPassMedium = (*el_medium_id)[ref] and el.passConversionVeto();
        bool isPassLoose = (*el_loose_id)[ref] and el.passConversionVeto();
        bool isPassHLT = (*el_hlt_id)[ref] and el.passConversionVeto();

        myLepton l;
        l.pdgId = -el.charge()*11;

        l.mva = (*el_mva)[ref];
        l.etasc = el.superCluster()->eta();
        l.sieie = el.full5x5_sigmaIetaIeta();
        l.sipip = el.full5x5_sigmaIphiIphi();
        l.sieip = el.full5x5_sigmaIetaIphi();
        l.r9 = el.r9();
        
        // float chIso = el.chargedHadronIso();
        // float nhIso = el.neutralHadronIso();
        // float phoIso = el.photonIso();
        float chIso = el.pfIsolationVariables().sumChargedHadronPt;
        float nhIso = el.pfIsolationVariables().sumNeutralHadronEt;
        float phoIso = el.pfIsolationVariables().sumPhotonEt;

        float puChIso= el.puChargedHadronIso();

        bool isEB = el.isEB();
        bool isEE = el.isEE();
        bool isEBEEGap  = el.isEBEEGap(); // for EB EE
        bool isEBEtaGap = el.isEBEtaGap(); //for EB
        bool isEBPhiGap = el.isEBPhiGap (); //for EB
        bool isEERingGap = el.isEERingGap(); //for EE
        bool isEEDeeGap = el.isEEDeeGap ();//for EE

        //l.iso = chIso + nhIso + phoIso; 
        // effective area from
        // https://indico.cern.ch/event/369239/contribution/4/attachments/1134761/1623262/talk_effective_areas_25ns.pdf
        double ea = 0.;
        ea = ea_->getEffectiveArea(el.eta() ) ;

        l.iso = chIso + TMath::Max( nhIso + phoIso - evt_->rho * ea , 0. ) ; 

        double Ecorr=NeroFunctions::getEGSeedCorrections(el,ebRecHits); 

        float smear = 0.0, scale = 1.0;
        float aeta = std::abs(el.eta());
        float et = el.energy()/cosh(aeta);

        if (iEvent.isRealData() )
        {
                
                scale = EleCorr->ScaleCorrection(iEvent.id().run(), el.isEB(), el.r9(), aeta, et);
                Ecorr *= scale;
        }
        else
        {
                 // the kNone refers to systematcis changes
                 // arbitrary func of run,lumi, event
                 rnd_->SetSeed(iEvent.id().run()*1000000+iEvent.luminosityBlock()*100 + iEvent.id().event()) ;
                 smear = EleCorr->getSmearingSigma((int) iEvent.id().run(), el.isEB(), el.r9(), aeta, et, 0.,0.);
                 float corr = 1.0  + smear * rnd_->Gaus(0,1);
                 Ecorr *= corr;
        
        }
        
        l.ecorr = Ecorr;
        l.p4.SetPxPyPzE( el.px(),el.py(),el.pz(),el.energy());

        l.selBits = 0 ;

        l.selBits |= unsigned(LepBaseline);
        l.selBits |= unsigned(isPassTight) * LepTight;
        l.selBits |= unsigned(isPassMedium) * LepMedium;
        l.selBits |= unsigned(isPassVeto) * LepVeto;
        l.selBits |= unsigned(isPassLoose) * LepLoose;

        l.selBits |= unsigned(isEB and (not isEBEEGap and not isEBEtaGap and not isEBPhiGap)  ) * LepEBEE;
        l.selBits |= unsigned(isEE and (not isEBEEGap and not isEERingGap and not isEEDeeGap)  ) * LepEBEE;

        const double dz = ( vtx_->size() ? 
                            el.gsfTrack()->dz((*vtx_->GetPV()).position()) : 
                            el.gsfTrack()->dz());
        
        const double dxy = ( vtx_->size() ? 
                             el.gsfTrack()->dxy((*vtx_->GetPV()).position()) : 
                             el.gsfTrack()->dxy());

        bool dz_cut  = isEB ? 0.10 : 0.20;
        bool dxy_cut = isEB ? 0.05 : 0.10;

        if ( dz<dz_cut && dxy<dxy_cut ){
            l.selBits |= unsigned(isPassMedium) * LepMediumIP;
            l.selBits |= unsigned(isPassTight)  * LepTightIP;
        }
        
        l.selBits |= unsigned(isPassHLT) * LepFake;

        if (el.chargeInfo().isGsfCtfConsistent and el.chargeInfo().isGsfCtfScPixConsistent and el.chargeInfo().isGsfScPixConsistent) l.selBits |= EleTripleCharge;
        constexpr reco::HitPattern::HitCategory missingHitType = reco::HitPattern::MISSING_INNER_HITS;
        const unsigned mHits = el.gsfTrack()->hitPattern().numberOfHits(missingHitType);

        if (mHits == 0 ) l.selBits |=EleNoMissingHits;

        l.pfPt = el.pt();
    
        l.chiso  = chIso;
        l.nhiso  = nhIso;
        l.phoiso = phoIso;
        l.puiso  = puChIso;
        //l.miniiso = getPFMiniIsolation_DeltaBeta(pf_->handle, dynamic_cast<const reco::Candidate *>(&el), 0.05, 0.2, 10., false) ;
        l.miniiso = getPFMiniIsolation_EffectiveArea(pf_->handle, dynamic_cast<const reco::Candidate *>(&el), 0.05, 0.2, 10., false, false, *rho_handle );

        if ( not (l.selBits & kMinId) ) continue;
        leptons.push_back(l);

    }

    if ( int(leptons.size()) < mMinNleptons ) return 1;

    // sort leptons
    sort( leptons.begin(),leptons.end() , [](const myLepton &x, const myLepton&y){ return x.p4.Pt() > y.p4.Pt();} );

    // fill storage
    for(const auto &l : leptons)
    {
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(l.p4);
        iso     -> push_back(l.iso);
        corr    -> push_back(l.ecorr);
        selBits -> push_back(l.selBits);
        pdgId   -> push_back(l.pdgId);
        mva     -> push_back(l.mva);
        lepPfPt -> push_back(l.pfPt);

        chIso	-> push_back(l.chiso);
        nhIso	-> push_back(l.nhiso);
        phoIso	-> push_back(l.phoiso);
        puIso	-> push_back(l.puiso);
        miniIso	-> push_back(l.miniiso);

        etaSC	-> push_back(l.etasc);
        sieie	-> push_back(l.sieie);
        sipip	-> push_back(l.sipip);
        sieip	-> push_back(l.sieip);
        r9  	-> push_back(l.r9);
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
