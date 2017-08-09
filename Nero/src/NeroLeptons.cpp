#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"
#include "NeroProducer/Nero/interface/MiniIsolation.hpp"
#include "NeroProducer/Nero/interface/NeroFunctions.hpp"
#include <time.h>

namespace myid{
bool isMediumMuon(const reco::Muon & recoMu) 
    { // medium id for B->F
      // https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonIdRun2#MediumID2016_to_be_used_with_Run
          bool goodGlob = recoMu.isGlobalMuon() and
                          recoMu.globalTrack()->normalizedChi2() < 3 and
                          recoMu.combinedQuality().chi2LocalPosition < 12 and
                          recoMu.combinedQuality().trkKink < 20; 
          bool isMedium = muon::isLooseMuon(recoMu) and 
                          recoMu.innerTrack()->validFraction() > 0.49 and
                          muon::segmentCompatibility(recoMu) > (goodGlob ? 0.303 : 0.451); 
          return isMedium; 
    }
};

// -- Electron Isolation
NeroLeptons::NeroLeptons(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
    BareLeptons()
{

    vtx_token = cc.consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
    token_pf = cc.consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCands"));
    ea_ . reset (new EffectiveAreas( edm::FileInPath(iConfig.getParameter<std::string>("eleEA")).fullPath () ) );
    mu_token = cc.consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
    el_token = cc.consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
    el_hltid_token = cc.consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleHLTIdMap"));
    el_mva_token = cc.consumes<edm::ValueMap<float> > (iConfig.getParameter<edm::InputTag>("eleMvaMap"));
    rho_token = cc.consumes<double> (edm::InputTag("fixedGridRhoFastjetCentralNeutral")); // for miniIso

    el_uncalib_token = cc.consumes<pat::ElectronCollection>(edm::InputTag("slimmedElectrons"));

    mMinPt_mu = iConfig.getParameter<double>("minMuPt");
    mMinEta_mu = iConfig.getParameter<double>("minMuEta");
    mMaxIso_mu = iConfig.getParameter<double>("maxMuIso");

    mMinPt_el = iConfig.getParameter<double>("minElePt");
    mMinEta_el = iConfig.getParameter<double>("minEleEta");
    mMaxIso_el = iConfig.getParameter<double>("maxEleIso");

    mMinId = iConfig.getParameter<string>("minLepId");

    mMinNleptons = iConfig.getParameter<int>("minLepN");

    ebRecHits_token = cc.mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ebRecHits"));
    //leps -> eeRecHits_token = cc.mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("eeRecHits"));

    // eventually configure
    //leps -> EleCorr = new EnergyScaleCorrection_class("EgammaAnalysis/ElectronTools/data/ScalesSmearings/Winter_2016_reReco_v1_ele");
    // leps->EleCorr -> doSmearings= true;
    // leps->EleCorr -> doScale= true;
    

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
    kMinId = idStringToEnum(mMinId);
    
    iEvent.getByToken(vtx_token, vtx_handle);
    const reco::Vertex *pv  = &vtx_handle->front();
    iEvent.getByToken(token_pf, handle_pf);

    iEvent.getByToken(mu_token,mu_handle);	
    iEvent.getByToken(el_token,el_handle);	
    iEvent.getByToken(el_uncalib_token,el_uncalib_handle);	

    //iEvent.getByToken(el_mediumid_token,el_medium_id);
    //iEvent.getByToken(el_tightid_token,el_tight_id);
    //iEvent.getByToken(el_vetoid_token,el_veto_id);
    //iEvent.getByToken(el_looseid_token,el_loose_id);
    iEvent.getByToken(el_mva_token,el_mva);
    iEvent.getByToken(el_hltid_token,el_hlt_id);
    iEvent.getByToken(rho_token,rho_handle);

    edm::Handle<EcalRecHitCollection> ebRecHits;
    //edm::Handle<EcalRecHitCollection> eeRecHits;
    iEvent.getByToken(ebRecHits_token,ebRecHits);
    //iEvent.getByToken(eeRecHits_token,eeRecHits);
    //
    iEvent.getByToken(el_uncalib_token,el_uncalib_handle);

    /*
    for(unsigned i=0;i<el_uncalib_handle->size();++i)
    {
        cout<<"I="<<i<<"PT="<<(*el_uncalib_handle)[i].pt()
            <<(*el_handle)[i].pt()
            <<"ETA = "<<(*el_uncalib_handle)[i].eta()<<":"<<(*el_handle)[i].eta()
            <<"ID = "<<edm::Ref<pat::ElectronCollection>(el_uncalib_handle,i).id().id()
            <<": "   <<edm::Ref<pat::ElectronCollection>(el_handle,i).id().id()
            <<endl;
    }
    */

    if ( not mu_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] mu_handle is not valid"<<endl;
    if ( not el_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_handle is not valid"<<endl;
    if ( not el_uncalib_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_uncalib_handle is not valid"<<endl;
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
            l.selBits |= unsigned(mu.isTightMuon( * pv ))*LepTight ;
            l.selBits |= unsigned(mu.isMediumMuon() * LepMedium);
            l.selBits |= unsigned(myid::isMediumMuon(mu) * MuMediumB2F);

            if ( fabs((mu.muonBestTrack()->dz(pv->position())))<0.1 and (mu.dB()< 0.02) ){
                l.selBits |= LepIP;
                l.selBits |= unsigned(mu.isMediumMuon() * LepMediumIP);
                l.selBits |= unsigned(mu.isTightMuon( * pv ))*LepTightIP ;
                l.selBits |= unsigned(mu.isSoftMuon(* pv)) * LepSoftIP;
                if(isFake){ l.selBits |= unsigned(mu.isTightMuon(* pv) * LepFake); }
                }
            l.selBits |= unsigned(mu.isStandAloneMuon() * MuStandalone);
            l.selBits |= unsigned(mu.isTrackerMuon() * MuTracker);
            l.selBits |= unsigned(mu.isGlobalMuon() * MuGlobal);
            
        l.pfPt = mu.pfP4().pt();

        l.chiso  = chiso;
        l.nhiso  = niso;
        l.phoiso = phoiso;
        l.puiso  = puiso;
        l.miniiso = getPFMiniIsolation_EffectiveArea(handle_pf, dynamic_cast<const reco::Candidate *>(&mu), 0.05, 0.2, 10., false, false, *rho_handle );

        if (not mu.innerTrack().isNull()){
            l.nlayers = mu.innerTrack() -> hitPattern().trackerLayersWithMeasurement();
            l.resolution = mu.innerTrack() ->ptError() / mu.innerTrack()->pt();
        }
        
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

        l.iso = chIso + TMath::Max( nhIso + phoIso - (*rho_handle) * ea , 0. ) ; 

        bool isPassVeto = passEleId( el, Veto, l.iso/el.pt());   
        bool isPassTight = passEleId( el, Tight, l.iso/el.pt()); 
        bool isPassMedium = passEleId( el, Medium, l.iso/el.pt());
        bool isPassLoose = passEleId( el, Loose, l.iso/el.pt()) ; 
        bool isPassHLT = (*el_hlt_id)[ref] and el.passConversionVeto();

        
        double Ecorr=NeroFunctions::getEGSeedCorrections(el,ebRecHits); 

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

        const double dz = fabs( vtx_handle->size() ? 
                            el.gsfTrack()->dz(pv->position()) : 
                            el.gsfTrack()->dz());
        
        const double dxy = fabs( vtx_handle->size() ? 
                             el.gsfTrack()->dxy(pv->position()) : 
                             el.gsfTrack()->dxy());

        bool dz_cut  = isEB ? 0.10 : 0.20;
        bool dxy_cut = isEB ? 0.05 : 0.10;

        if ( dz<dz_cut && dxy<dxy_cut ){
            l.selBits |= LepIP;
            l.selBits |= unsigned(isPassMedium) * LepMediumIP;
            l.selBits |= unsigned(isPassTight)  * LepTightIP;
        }
        
        l.selBits |= unsigned(isPassHLT) * LepFake;

        if (el.chargeInfo().isGsfCtfConsistent and el.chargeInfo().isGsfCtfScPixConsistent and el.chargeInfo().isGsfScPixConsistent) l.selBits |= EleTripleCharge;
        constexpr reco::HitPattern::HitCategory missingHitType = reco::HitPattern::MISSING_INNER_HITS;
        const unsigned mHits = el.gsfTrack()->hitPattern().numberOfHits(missingHitType);

        if (mHits == 0 ) l.selBits |=EleNoMissingHits;

        l.pfPt = (*el_uncalib_handle)[iEle].pt();
    
        l.chiso  = chIso;
        l.nhiso  = nhIso;
        l.phoiso = phoIso;
        l.puiso  = puChIso;
        //l.miniiso = getPFMiniIsolation_DeltaBeta(handle_pf, dynamic_cast<const reco::Candidate *>(&el), 0.05, 0.2, 10., false) ;
        l.miniiso = getPFMiniIsolation_EffectiveArea(handle_pf, dynamic_cast<const reco::Candidate *>(&el), 0.05, 0.2, 10., false, false, *rho_handle );

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

        resolution -> push_back(l.resolution);
        nLayers -> push_back(l.nlayers);
    }


    return 0;
}

namespace NeroHelper {
    // function from EGamma 
    float dEtaInSeed(const pat::Electron& ele){
        return ele.superCluster().isNonnull() && ele.superCluster()->seed().isNonnull() ? 
            ele.deltaEtaSuperClusterTrackAtVtx() - ele.superCluster()->eta() + ele.superCluster()->seed()->eta() : std::numeric_limits<float>::max();
    }
    int missingHits(const pat::Electron& el) { 
        constexpr reco::HitPattern::HitCategory missingHitType = reco::HitPattern::MISSING_INNER_HITS;
        const unsigned mHits = el.gsfTrack()->hitPattern().numberOfHits(missingHitType);
        return mHits ;
    }

}; // namespace

bool NeroLeptons::passEleId(const pat::Electron &el , ELEID id, float iso)
{
   using std::abs;
   bool passId = true;
   if ( abs(el.superCluster()->eta()) <= 1.479 )
   {
       switch (id)
       {
           case Veto: 
               {
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.0115; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) < 0.00749  ; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) <0.228  ; 
               passId = passId and  el.hadronicOverEm() < 0.356;
               passId = passId and  iso < 0.175 ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.299 ;
               passId = passId and  NeroHelper::missingHits(el) <=2;
               passId = passId and  el.passConversionVeto();
               break;
               }
           case Loose:{
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.011; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) < 0.00477; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) <0.222 ; 
               passId = passId and  el.hadronicOverEm() < 0.298;
               passId = passId and  iso <  0.0994  ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.241 ;
               passId = passId and  NeroHelper::missingHits(el) <=1;
               passId = passId and  el.passConversionVeto();
               break;
               }
           case Medium:{
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.00998; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) < 0.00311; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) <0.103; 
               passId = passId and  el.hadronicOverEm() < 0.253;
               passId = passId and  iso < 0.0695 ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.134 ;
               passId = passId and  NeroHelper::missingHits(el) <=1;
               passId = passId and  el.passConversionVeto();
               break;
               }
           case Tight: { 
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.00998; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) <   0.00308 ; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) <0.0816; 
               passId = passId and  el.hadronicOverEm() < 0.0414;
               passId = passId and  iso < 0.0588 ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.0129 ;
               passId = passId and  NeroHelper::missingHits(el) <=1;
               passId = passId and  el.passConversionVeto();
               break;
               }
            default: passId=false;
       }// end switch id
   }// end barrel
   else
   {
       switch (id)
       {
           case Veto: 
               {
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.037; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) < 0.00895  ; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) < 0.213  ; 
               passId = passId and  el.hadronicOverEm() < 0.211;
               passId = passId and  iso < 0.159 ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.15  ;
               passId = passId and  NeroHelper::missingHits(el) <=3;
               passId = passId and  el.passConversionVeto();
               break;
               }
           case Loose:{
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.0314; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) < 0.00868; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) < 0.213  ; 
               passId = passId and  el.hadronicOverEm() < 0.101 ;
               passId = passId and  iso <  0.107 ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.14  ;
               passId = passId and  NeroHelper::missingHits(el) <=1;
               passId = passId and  el.passConversionVeto();
               break;
               }
           case Medium:{
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.0298; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) < 0.00609; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) < 0.045; 
               passId = passId and  el.hadronicOverEm() < 0.0878;
               passId = passId and  iso < 0.0821 ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.13  ;
               passId = passId and  NeroHelper::missingHits(el) <=1;
               passId = passId and  el.passConversionVeto();
               break;
               }
           case Tight: { 
               passId = passId and  el.full5x5_sigmaIetaIeta()  < 0.0292; 
               passId = passId and  abs(NeroHelper::dEtaInSeed(el)) < 0.00605  ; 
               passId = passId and  abs(el.deltaPhiSuperClusterTrackAtVtx()) < 0.0394; 
               passId = passId and  el.hadronicOverEm() < 0.0641 ;
               passId = passId and  iso < 0.0571  ;
               passId = passId and  std::abs(1.0 - el.eSuperClusterOverP())*1.0/el.ecalEnergy() < 0.0129  ;
               passId = passId and  NeroHelper::missingHits(el) <=1;
               passId = passId and  el.passConversionVeto();
               break;
               }
            default: passId=false;
       }// end switch id
   } //end endcap
   return passId;
}

NEROREGISTER(NeroLeptons);



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
