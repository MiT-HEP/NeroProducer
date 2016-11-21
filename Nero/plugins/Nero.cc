// -*- C++ -*-
//
// Package:    NeroProducer/Nero
// Class:      Nero
// 
/**\class Nero Nero.cc NeroProducer/Nero/plugins/Nero.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Andrea Carlo Marini
//         Created:  Tue, 19 May 2015 13:38:17 GMT
//
//

#include "NeroProducer/Nero/interface/Nero.hpp"

#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/NeroFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroPuppiJets.hpp"
#include "NeroProducer/Nero/interface/NeroPuppiFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/NeroMetRecluster.hpp"
#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"
#include "NeroProducer/Nero/interface/NeroAll.hpp"
#include "NeroProducer/Nero/interface/NeroTrigger.hpp"
#include "NeroProducer/Nero/interface/NeroMatching.hpp"

//#define VERBOSE 2
//#define VERBOSE 1
//#define PROFILE 1

#ifdef PROFILE
    #include "TBenchmark.h"
    TBenchmark profile;
#endif

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
Nero::Nero(const edm::ParameterSet& iConfig) 

{

    tag_  = iConfig.getParameter<string>("tag");
    head_ = iConfig.getParameter<string>("head");
    info_ = iConfig.getParameter<string>("info");
    cmssw_ = iConfig.getParameter<string>("cmssw");

    bool onlyMc = iConfig.getParameter<bool>("onlyMc");

    // not push_back inline because he needs to know the class type for init
    NeroEvent *evt = new NeroEvent();
    evt -> mOnlyMc = onlyMc;
    evt -> rho_token = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
    evt -> SetExtend ( iConfig.getUntrackedParameter<bool>("extendEvent",false) );
    evt -> filter_token = consumes<edm::TriggerResults>(iConfig.getParameter < edm::InputTag > ("metFilterToken"));
    evt -> BadChCandFilter_token = consumes<bool>(iConfig.getParameter < edm::InputTag > ("BadChCandFilterToken"));
    evt -> BadPFMuon_token = consumes<bool>(iConfig.getParameter < edm::InputTag > ("BadPFMuonToken"));
    *(evt -> metfilterNames) = iConfig.getParameter < std::vector<std::string> > ("metfilterNames");

    obj.push_back(evt);

    // -- Before Leptons (mu uses Vtx)
    NeroVertex *vtx = new NeroVertex();
    vtx -> mOnlyMc = onlyMc;
    vtx -> token = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
    obj.push_back(vtx);

    // 
    NeroPF *pf = new NeroPF();
    pf -> token = consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCands"));
    obj.push_back (pf);


    //now do what ever initialization is needed
    NeroJets *jets = new NeroJets();
    jets -> mOnlyMc = onlyMc;
    jets -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"));
    jets -> qg_token = consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "qgLikelihood"));
    jets -> mMinPt = iConfig.getParameter<double>("minJetPt");
    jets -> mMinNjets = iConfig.getParameter<int>("minJetN");
    jets -> mMinEta = iConfig.getParameter<double>("minJetEta");
    jets -> mMinId = iConfig.getParameter<string>("minJetId");
    jets -> SetMatch( iConfig.getParameter<bool>("matchJet") );
    jets -> pf = pf;
    jets -> vtx = vtx;
    jets -> evt = evt;
    jets -> cachedPrefix = "";

    jets -> qg_token_Mult = consumes<edm::ValueMap<int>>(edm::InputTag("QGTagger", "mult"));
    jets -> qg_token_PtD = consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "ptD"));
    jets -> qg_token_Axis2 = consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "axis2"));

    obj.push_back(jets);

    NeroPuppiJets *puppijets = new NeroPuppiJets();
    puppijets -> mOnlyMc = onlyMc;
    puppijets -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppijets"));
    puppijets -> mMinPt = iConfig.getParameter<double>("minPuppiJetPt");
    puppijets -> mMinEta = iConfig.getParameter<double>("minPuppiJetEta");
    puppijets -> mMinNjets = iConfig.getParameter<int>("minPuppiJetN");
    puppijets -> mMinId = iConfig.getParameter<string>("minPuppiJetId");
    puppijets -> pf = pf;
    puppijets -> cachedPrefix = "";
    obj.push_back(puppijets);
    
    //--
    bool doReclustering= iConfig.getParameter<bool>("doReclustering");
    bool doAK8 = iConfig.getParameter<bool>("doAK8");
    bool doCA15 = iConfig.getParameter<bool>("doCA15");
    bool doPuppi = iConfig.getParameter<bool>("doPuppi");

    NeroFatJets *chsAK8 = new NeroFatJets();
    chsAK8 -> mRunJEC = false; // these jets are already corrected in MiniAOD
    chsAK8 -> mOnlyMc = onlyMc;
    chsAK8 -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("chsAK8"));
    chsAK8 -> rho_token = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
    chsAK8 -> vertex_token = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
    chsAK8 -> mMinPt = iConfig.getParameter<double>("minAK8CHSPt");
    chsAK8 -> mMaxEta = iConfig.getParameter<double>("minAK8CHSEta");
    chsAK8 -> mMinId = iConfig.getParameter<string>("minAK8CHSId");
    chsAK8 -> cachedPrefix = iConfig.getParameter<string>("AK8CHSName");
    chsAK8 -> jetRadius = 0.8;
    chsAK8 -> subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDrop"+chsAK8 -> cachedPrefix ,"SubJets"));
    chsAK8 -> btags_token = mayConsume<reco::JetTagCollection>(edm::InputTag(chsAK8->cachedPrefix + "PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
    chsAK8 -> jecBasePath= iConfig.getParameter<string>("chsAK8JEC");
    obj.push_back(chsAK8);


    //--
    if (doReclustering){
        if (doAK8) {
           if (doPuppi) {
               NeroPuppiFatJets *puppiAK8= new NeroPuppiFatJets();
               puppiAK8 -> mOnlyMc = onlyMc;
               puppiAK8 -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppiAK8"));
               puppiAK8 -> rho_token = evt->rho_token;
               puppiAK8 -> mMinPt = iConfig.getParameter<double>("minAK8PuppiPt");
               puppiAK8 -> mMaxEta = iConfig.getParameter<double>("minAK8PuppiEta");
               puppiAK8 -> mMinId = iConfig.getParameter<string>("minAK8PuppiId");
               puppiAK8 -> cachedPrefix = iConfig.getParameter<string>("AK8PuppiName");
               puppiAK8 -> jetRadius = 0.8;
               puppiAK8 -> subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDrop"+puppiAK8 -> cachedPrefix ,"SubJets"));
               puppiAK8 -> btags_token = mayConsume<reco::JetTagCollection>(edm::InputTag(puppiAK8->cachedPrefix + "PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
               obj.push_back(puppiAK8);
           }
        }

        if (doCA15) {

           if (doPuppi) {
               NeroPuppiFatJets *puppiCA15= new NeroPuppiFatJets();
               puppiCA15 -> mOnlyMc = onlyMc;
               puppiCA15 -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("puppiCA15"));
               puppiCA15 -> rho_token = evt->rho_token;
               puppiCA15 -> mMinPt = iConfig.getParameter<double>("minCA15PuppiPt");
               puppiCA15 -> mMaxEta = iConfig.getParameter<double>("minCA15PuppiEta");
               puppiCA15 -> mMinId = iConfig.getParameter<string>("minCA15PuppiId");
               puppiCA15 -> cachedPrefix = iConfig.getParameter<string>("CA15PuppiName");
               puppiCA15 -> jetRadius = 1.5;
               puppiCA15 -> subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDrop"+puppiCA15 -> cachedPrefix ,"SubJets"));
               puppiCA15 -> btags_token = mayConsume<reco::JetTagCollection>(edm::InputTag(puppiCA15->cachedPrefix + "PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
               obj.push_back(puppiCA15);
           } else {
               NeroFatJets *chsCA15 = new NeroFatJets();
               chsCA15 -> mRunJEC = true; 
               chsCA15 -> mOnlyMc = onlyMc;
               chsCA15 -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("chsCA15"));
               chsCA15 -> rho_token = evt->rho_token;
               chsCA15 -> mMinPt = iConfig.getParameter<double>("minCA15CHSPt");
               chsCA15 -> mMaxEta = iConfig.getParameter<double>("minCA15CHSEta");
               chsCA15 -> mMinId = iConfig.getParameter<string>("minCA15CHSId");
               chsCA15 -> cachedPrefix = iConfig.getParameter<string>("CA15CHSName");
               chsCA15 -> jetRadius = 1.5;
               chsCA15 -> subjets_token = mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDrop"+chsCA15 -> cachedPrefix ,"SubJets"));
               chsCA15 -> btags_token = mayConsume<reco::JetTagCollection>(edm::InputTag(chsCA15->cachedPrefix + "PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
               obj.push_back(chsCA15);

           }
        }
    }

    // --- 
    NeroTaus *taus = new NeroTaus();
    taus -> mOnlyMc = onlyMc;
    taus -> token = consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus"));
    taus -> mMinPt = iConfig.getParameter<double>("minTauPt");
    taus -> mMinNtaus = iConfig.getParameter<int>("minTauN");
    taus -> mMinEta = iConfig.getParameter<double>("minTauEta");
    taus -> mMinId = iConfig.getParameter<string>("minTauId");
    taus -> mMaxIso = iConfig.getParameter<double>("maxTauIso");
    taus -> SetExtend ( iConfig.getParameter<bool>("extendTau") );
    taus -> SetMatch( iConfig.getParameter<bool>("matchTau") );
    obj.push_back(taus);

    //--
    NeroLeptons *leps = new NeroLeptons();
    leps -> mOnlyMc = onlyMc;
    leps -> vtx_ = vtx; // Set the Vertex class
    leps -> evt_ = evt; // Set the Event class
    leps -> pf_ = pf;
    leps -> ea_ . reset (new EffectiveAreas( edm::FileInPath(iConfig.getParameter<std::string>("eleEA")).fullPath () ) );
    leps -> mu_token = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
    leps -> el_token = consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
    leps -> token_smear = consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("calibratedelectrons"));
    leps -> el_vetoid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleVetoIdMap"));
    leps -> el_looseid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleLooseIdMap"));
    leps -> el_mediumid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleMediumIdMap"));
    leps -> el_tightid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleTightIdMap"));
    leps -> el_hltid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleHLTIdMap"));
    //leps -> el_mva_token = consumes<edm::ValueMap<float> > (iConfig.getParameter<edm::InputTag>("eleMvaMap"));
    leps -> SetMatch( iConfig.getParameter<bool>("matchLep") );

    //leps -> el_iso_ch_token  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("eleChargedIsolation") );
    //leps -> el_iso_nh_token  = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("eleNeutralHadronIsolation") );
    //leps -> el_iso_pho_token = consumes<edm::ValueMap<float> >(iConfig.getParameter<edm::InputTag>("elePhotonIsolation") );
    //
    leps -> mMinPt_mu = iConfig.getParameter<double>("minMuPt");
    leps -> mMinEta_mu = iConfig.getParameter<double>("minMuEta");
    leps -> mMaxIso_mu = iConfig.getParameter<double>("maxMuIso");

    leps -> mMinPt_el = iConfig.getParameter<double>("minElePt");
    leps -> mMinEta_el = iConfig.getParameter<double>("minEleEta");
    leps -> mMaxIso_el = iConfig.getParameter<double>("maxEleIso");

    leps -> mMinNleptons = iConfig.getParameter<int>("minLepN");
    leps -> SetMatch( iConfig.getParameter<bool>("matchLep") );

    // eventually configure
    /* Not derived yet
    leps -> EleCorr = new EnergyScaleCorrection_class("EgammaAnalysis/ElectronTools/data/76X_16DecRereco_2015");
        leps->EleCorr -> doSmearings= true;
        leps->EleCorr -> doScale= true;
    */

    obj. push_back(leps);

    //--

    NeroMet *met = new NeroMet();
    met -> mOnlyMc = onlyMc;
    met -> token = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("mets"));
    met -> pf = pf;
    if (doPuppi) {
        met -> rerunPuppi = true;
        met -> token_puppiRerun = consumes<reco::PFMETCollection>(iConfig.getParameter<edm::InputTag>("metsPuppi"));
        met -> token_puppiRerunUncorr = consumes<reco::PFMETCollection>(iConfig.getParameter<edm::InputTag>("metsPuppiUncorrected"));
    } else {
        met -> rerunPuppi = false;
        met -> token_puppi = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metsPuppi"));
    }
    met -> SetExtend (iConfig.getParameter<bool>("extendMet"));
    obj.push_back(met);


    // --
    NeroPhotons *phos = new NeroPhotons();
    phos -> mOnlyMc = onlyMc;
    phos -> token = consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons"));
    phos -> token_smear = consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("calibratedphotons"));
    phos -> loose_id_token = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoLooseIdMap"));
    phos -> medium_id_token = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoMediumIdMap"));
    phos -> tight_id_token = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoTightIdMap"));
    phos -> iso_ch_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoChargedIsolation"));
    phos -> iso_nh_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoNeutralHadronIsolation"));
    phos -> iso_pho_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoPhotonIsolation"));
    phos -> iso_wch_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoWorstChargedIsolation"));
    phos -> ebRecHits_token = mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ebRecHits"));
    phos -> eeRecHits_token = mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("eeRecHits"));
    phos -> mMinPt = iConfig.getParameter<double>("minPhoPt");
    phos -> mMaxIso = iConfig.getParameter<double>("maxPhoIso");
    phos -> mMinNpho = iConfig.getParameter<int>("minPhoN");
    phos -> mMinId = iConfig.getParameter<string>("minPhoId");
    phos -> mMaxEta = iConfig.getParameter<double>("minPhoEta");
    phos -> SetMatch( iConfig.getParameter<bool>("matchPho") );
    phos -> pf = pf;
    phos -> jets = jets;
    phos -> evt = evt;
    phos -> vtx = vtx;
    phos -> leps = leps;
    phos -> SetExtend (iConfig.getParameter<bool>("extendPhotons"));
    /*
    phos -> fpr = new SuperClusterFootprintRemovalMiniAOD( consumesCollector() );
    phos -> PhoCorr = new EnergyScaleCorrection_class("EgammaAnalysis/ElectronTools/data/76X_16DecRereco_2015");
        phos->PhoCorr -> doSmearings= true;
        phos->PhoCorr -> doScale= true;
    */
    obj.push_back(phos);

    NeroMonteCarlo *mc = new NeroMonteCarlo();
    mc -> mOnlyMc = onlyMc;
    mc -> SetExtend ( iConfig.getUntrackedParameter<bool>("extendMonteCarlo",false) );
    mc -> packed_token = consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("packedgen"));
    mc -> pruned_token = consumes<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("prunedgen")) ;
    mc -> info_token   = consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generator"));
    mc -> lhe_token   = mayConsume<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lhe"));//LHEEventProduct_externalLHEProducer__LHE
    mc -> pu_token     = consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileup"));
    mc -> jet_token    = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjets"));
    mc -> runinfo_token = consumes<GenRunInfoProduct,edm::InRun>(iConfig.getParameter<edm::InputTag>("genruninfo") );
    mc -> mMinGenParticlePt = iConfig.getParameter<double>("minGenParticlePt");
    mc -> mMinGenJetPt = iConfig.getParameter<double>("minGenJetPt");
    mc -> mParticleGun = iConfig.getUntrackedParameter<bool>("particleGun",false);

    obj.push_back(mc);
    runObj.push_back(mc);

    NeroMatching *match = new NeroMatching();
        match -> jets_ = jets;
        match -> leps_ = leps;
        match -> phos_ = phos;
        match -> taus_ = taus;
        match -> mc_ = mc;
        match -> mTauDr = iConfig.getParameter<double>("matchTauDr");
        match -> mJetDr = iConfig.getParameter<double>("matchJetDr");
        match -> mLepDr = iConfig.getParameter<double>("matchLepDr");
        match -> mPhoDr = iConfig.getParameter<double>("matchPhoDr");
    obj.push_back(match);

    NeroTrigger *tr = new NeroTrigger();
    tr -> mOnlyMc = onlyMc;
    tr -> token = consumes< edm::TriggerResults >( iConfig.getParameter<edm::InputTag>("trigger"));
    tr -> prescale_token = consumes<pat::PackedTriggerPrescales>( iConfig.getParameter<edm::InputTag>("prescales") );
    tr -> object_token = consumes< pat::TriggerObjectStandAloneCollection > ( iConfig.getParameter<edm::InputTag> ("objects") );
    tr -> token_l1EtSum= consumes< BXVector< l1t::EtSum > > (edm::InputTag("caloStage2Digis:EtSum"));
    tr -> mNMatch = iConfig.getParameter<int>("triggerNMatch");
    // set the collection he needs to be awared of
    tr -> leps_ = leps;
    tr -> jets_ = jets;
    tr -> taus_ = taus;
    tr -> photons_ = phos;
    //
    *(tr -> triggerNames) =  iConfig.getParameter<std::vector<std::string> > ("triggerNames");
    obj.push_back(tr);

    // ----------------- Collection to be run at the Lumi Block ----
    NeroAll *info = new NeroAll();
    info -> mOnlyMc = onlyMc;
    info -> isSkim_ = 1;
    //info -> pu_token = consumes<std::vector<PileupSummaryInfo> >(edm::InputTag("addPileupInfo"));
    info -> info_token = consumes<GenEventInfoProduct>(edm::InputTag("generator"));
    info -> lhe_token   = mayConsume<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lhe"));//LHEEventProduct_externalLHEProducer__LHE
    info -> events_token = consumes<std::vector<long>,edm::InLumi>( edm::InputTag("InfoProducer","vecEvents") ) ;
    info -> weights_token = consumes<std::vector<float>,edm::InLumi>( edm::InputTag("InfoProducer","vecMcWeights") ) ;
    info -> putrue_token = consumes<std::vector<int>,edm::InLumi>( edm::InputTag("InfoProducer","vecPuTrueInt") ) ;
    obj.insert(obj.begin(),info);
    lumiObj.push_back(info);


}


Nero::~Nero()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
    // for(unsigned int i=0;i< obj.size();++i)
    // {
    //        delete obj[i];
    // }
    // obj.clear();

}


//
// member functions
//

// ------------ method called for each event  ------------
    void
Nero::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    #ifdef VERBOSE
        if (VERBOSE>1) cout<<"------- begin event --------"<<endl;
    #endif

    for(auto o : obj)
        o->clear();

    #ifdef VERBOSE
        if (VERBOSE>1) cout<<"------- analyze event --------"<<endl;
    #endif

    for(auto o : obj)
    {
        #ifdef VERBOSE
            if (VERBOSE > 1) { cout<<"[Nero]::[analyze] going to analyze "<<o->name() <<endl; }
            if (VERBOSE){sw_.Reset(); sw_.Start();}
        #endif
        #ifdef PROFILE
            if (PROFILE>0)profile.Start(o->name().c_str());
        #endif

        if (o->analyze(iEvent, iSetup) ) return; // analyze return 0 on success (VTX ..)

        #ifdef VERBOSE
            if (VERBOSE)
            {
                sw_.Stop(); 
                times_[o->name()] += sw_.CpuTime() ;
            }
            if(VERBOSE>1)
            {
                cout<< "[Nero]::[analyze] object "<<o->name()
                    <<" took:"<< sw_.CpuTime()<< "CPU Time and "
                    <<sw_.RealTime()<<"RealTime"<<endl;
            }
        #endif
        #ifdef PROFILE
            if (PROFILE>0)profile.Stop(o->name().c_str());
        #endif
    }

    // compress double precision to float precision
    for(auto o : obj)
    {
        o->compress();
    }

    #ifdef VERBOSE
        if (VERBOSE>1) cout<<"------- fill event --------"<<endl;
    #endif

    tree_->Fill();
    #ifdef VERBOSE
        if (VERBOSE>1) cout<<"------- end event (success) --------"<<endl;
        if (VERBOSE){
            times_[ "counter" ] +=1;
            if(times_[ "counter"] > 3000 )
            {
                cout<< " --- CPU TIMES ----" <<endl;
                for(auto &x : times_)
                {
                    cout << x.first <<": "<<x.second<<endl;
                    x.second = 0;
                }
                cout<< " ------------" <<endl;
                times_[ "counter"] = 0;
            }
        } // end VERBOSE
    #endif

}


// ------------ method called once each job just before starting event loop  ------------
    void 
Nero::beginJob()
{
    #ifdef VERBOSE
        if(VERBOSE) cout<<" >>>>>>> BEGIN JOB <<<<<<<<<"<<endl;
    #endif

    tree_    = fileService_ -> make<TTree>("events", "events");
    all_     = fileService_ -> make<TTree>("all", "all");	  
    hXsec_   = fileService_ -> make<TH1F>("xSec", "xSec",20,-0.5,19.5); hXsec_ ->Sumw2();

    hD_["Events"]   = fileService_ -> make<TH1D>("hDEvents", "hDEvents",1,0,1); hD_["Events"] ->Sumw2();
    hD_["TotalMcWeight"]   = fileService_ -> make<TH1D>("hDTotalMcWeight", "hDTotalMcWeight",1,0,1); hD_["TotalMcWeight"] ->Sumw2();
    hD_["Pileup"]   = fileService_ -> make<TH1D>("hDPileup", "hDPileup",100,0,100); hD_["Pileup"] ->Sumw2();

    hD_["scaleReweightSums"]  = fileService_ -> make<TH1D>("scaleReweightSums", "", 6, 0., 6.);

    hD_["scaleReweightSums"]->Sumw2();
    int iBin = 0;
    for (TString label : {"r1f2", "r1f5", "r2f1", "r2f2", "r5f1", "r5f5"})
        hD_["scaleReweightSums"]->GetXaxis()->SetBinLabel(++iBin, label);

    hD_["pdfReweightSums"] = fileService_ -> make<TH1D> ("pdfReweightSums","", 500,0.,500.);

    fileService_ ->make<TNamed>("tag",tag_.c_str() );
    fileService_ ->make<TNamed>("head",head_.c_str() );
    fileService_ ->make<TNamed>("info",info_.c_str() );
    fileService_ ->make<TNamed>("cmssw",cmssw_.c_str() );

    // FILL TRIGGER NAMES INFO
    string myString = "";
    for(auto o : obj)
        if (dynamic_cast<NeroTrigger*>(o) != NULL) 
        {
            NeroTrigger* tr = dynamic_cast<NeroTrigger*>(o) ;
            for( string n : *tr->triggerNames)
                myString +=  n + ",";
        }

    fileService_ -> make<TNamed>("triggerNames",myString.c_str());

    // define branches
    for(auto o : obj)
    {
        if (dynamic_cast<NeroAll*> (o) !=NULL ) { continue ; }  // otherwise I will have also the branch in the main tree.
        #ifdef VERBOSE
            if(VERBOSE) cout<<" -> Calling defineBranch for "<<o->name()<<endl;
        #endif
        o -> defineBranches(tree_);
    }

    for(auto o : lumiObj)
    {
        #ifdef VERBOSE
            if(VERBOSE) cout<<" -> Calling defineBranch for "<<o->name()<<endl;
        #endif
        o -> defineBranches(all_);
    }
    
    //set histogram map
    for(auto o : lumiObj){
        if (dynamic_cast<NeroAll*> (o) !=NULL ) {
            #ifdef VERBOSE
                if(VERBOSE) cout<<" -> Setting Histograms in "<<o->name()<<endl;
            #endif
            NeroAll * na = dynamic_cast<NeroAll*> (o);
            na->hDEvents =  hD_["Events"];
            na->hDTotalMcWeight = hD_["TotalMcWeight"];
            na->hDPileup = hD_["Pileup"];
            na->hDscaleReweightSums = hD_["scaleReweightSums"];
            na->hDpdfReweightSums = hD_["pdfReweightSums"];
        } // end if
        } // end for o in lumiObj
    #ifdef VERBOSE
        if(VERBOSE) cout<<" -- end of begin job "<<endl;
    #endif
} //end beginJob


// ------------ method called once each job just after ending the event loop  ------------
    void 
Nero::endJob() 
{
    #ifdef PROFILE
        for(auto o : obj)
        {
            if (PROFILE>0)profile.Show(o->name().c_str());
        }   
    #endif
    #ifdef VERBOSE
        if(VERBOSE) cout<<" >>>>>>> END JOB <<<<<<<<<"<<endl;
    #endif
}

// ------------ method called when starting to processes a run  ------------

    void 
Nero::beginRun(edm::Run const&iRun, edm::EventSetup const&)
{
    #ifdef VERBOSE
        if(VERBOSE) cout<<" ======= BEGIN RUN ======="<<endl;
    #endif
}


// ------------ method called when ending the processing of a run  ------------

    void 
Nero::endRun(edm::Run const&iRun, edm::EventSetup const&iSetup)
{ // this should be finalized at the end of the run
    //for(auto o : obj)
    //	{
    //	if ( dynamic_cast<NeroMonteCarlo*> (o) != NULL)
    //		{
    //		dynamic_cast<NeroMonteCarlo*> (o) -> crossSection(iRun, hXsec_);
    //		}
    //	}

    for(auto o : runObj )
    {
        #ifdef VERBOSE
            if(VERBOSE> 1) cout<<"[Nero]::[endRun]::[DEBUG] calling object"<<o->name()<<endl;
        #endif
        o->analyzeRun(iRun, hXsec_);
    }
    #ifdef VERBOSE
        if(VERBOSE) cout <<" ======== END RUN ======="<<endl;
    #endif

}


// ------------ method called when starting to processes a luminosity block  ------------

    void 
Nero::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
    #ifdef VERBOSE
        if(VERBOSE) cout <<" -------- BEGIN LUMI --------"<<endl;
    #endif

}


// ------------ method called when ending the processing of a luminosity block  ------------

    void 
Nero::endLuminosityBlock(edm::LuminosityBlock const&iLumi, edm::EventSetup const&)
{
    for(auto o :lumiObj)
    {
        #ifdef VERBOSE
            if(VERBOSE>1) cout<<"[Nero]::[endLuminosityBlock]::[DEBUG] calling object"<<o->name()<<endl;
        #endif

        o->analyzeLumi(iLumi,all_);
    }

    #ifdef VERBOSE
        if(VERBOSE) cout <<" -------- END LUMI --------"<<endl;
    #endif
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Nero::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Nero);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
