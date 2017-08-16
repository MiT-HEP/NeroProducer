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
#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"
#include "NeroProducer/Nero/interface/NeroAll.hpp"
#include "NeroProducer/Nero/interface/NeroTrigger.hpp"
#include "NeroProducer/Nero/interface/NeroMatching.hpp"

//#define NERO_VERBOSE 2
//#define NERO_VERBOSE 1

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
    leps -> el_vetoid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleVetoIdMap"));
    leps -> el_looseid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleLooseIdMap"));
    leps -> el_mediumid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleMediumIdMap"));
    leps -> el_tightid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleTightIdMap"));
    leps -> el_hltid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleHLTIdMap"));
    leps -> el_mva_token = consumes<edm::ValueMap<float> > (iConfig.getParameter<edm::InputTag>("eleMvaMap"));
    leps -> SetMatch( iConfig.getParameter<bool>("matchLep") );
    leps -> rho_token = consumes<double> (edm::InputTag("fixedGridRhoFastjetCentralNeutral")); // for miniIso

    leps -> el_uncalib_token = consumes<pat::ElectronCollection>(edm::InputTag("slimmedElectrons"));

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

    leps -> mMinId = iConfig.getParameter<string>("minLepId");

    leps -> mMinNleptons = iConfig.getParameter<int>("minLepN");
    leps -> SetMatch( iConfig.getParameter<bool>("matchLep") );

    leps -> ebRecHits_token = mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ebRecHits"));
    //leps -> eeRecHits_token = mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("eeRecHits"));

    // eventually configure
    //leps -> EleCorr = new EnergyScaleCorrection_class("EgammaAnalysis/ElectronTools/data/ScalesSmearings/Winter_2016_reReco_v1_ele");
    // leps->EleCorr -> doSmearings= true;
    // leps->EleCorr -> doScale= true;
    

    obj. push_back(leps);

    //--

    NeroMet *met = new NeroMet();
    met -> mOnlyMc = onlyMc;
    met -> token = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("mets"));
    met -> pf = pf;
    met -> token_puppi = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metsPuppi"));
    met -> token_cleanmu = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metscleanmu"));
    met -> token_cleaneg = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metscleaneg"));
    met -> token_unclean = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metsunclean"));
    met -> SetExtend (iConfig.getParameter<bool>("extendMet"));
    obj.push_back(met);



    NeroMonteCarlo *mc = new NeroMonteCarlo();
    mc -> mOnlyMc = onlyMc;
    mc -> SetExtend ( iConfig.getUntrackedParameter<bool>("extendMonteCarlo",false) );
    mc -> packed_token = consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("packedgen"));
    mc -> pruned_token = consumes<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("prunedgen")) ;
    mc -> info_token   = consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generator"));
    mc -> lhe_token   = mayConsume<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lhe"));//LHEEventProduct_externalLHEProducer__LHE
    mc -> pu_token     = consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileup"));
    mc -> jet_token    = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjets"));
    mc -> jetAK8_token    = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjetsAK8"));
    mc -> runinfo_token = consumes<GenRunInfoProduct,edm::InRun>(iConfig.getParameter<edm::InputTag>("genruninfo") );
    mc -> mMinGenParticlePt = iConfig.getParameter<double>("minGenParticlePt");
    mc -> mMinGenJetPt = iConfig.getParameter<double>("minGenJetPt");
    mc -> mParticleGun = iConfig.getUntrackedParameter<bool>("particleGun",false);
    // match
    mc -> genBHadFlavour_token = consumes<std::vector<int> > (edm::InputTag("matchGenBHadron", "genBHadFlavour"));
    mc -> genCHadJetIndex_token = consumes<std::vector<int> > (edm::InputTag("matchGenCHadron", "genCHadJetIndex"));
    mc -> genCHadBHadronId_token = consumes<std::vector<int> > (edm::InputTag("matchGenCHadron", "genCHadBHadronId"));
    mc -> genTtbarId_token = consumes<int> (edm::InputTag("categorizeGenTtbar", "genTtbarId"));

    obj.push_back(mc);
    runObj.push_back(mc);

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
    jets -> mc = mc;
    jets -> cachedPrefix = "";

    jets -> qg_token_Mult = consumes<edm::ValueMap<int>>(edm::InputTag("QGTagger", "mult"));
    jets -> qg_token_PtD = consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "ptD"));
    jets -> qg_token_Axis2 = consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "axis2"));
    jets -> qg_token_Axis1 = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGTagger", "axis1"));
    jets -> qg_token_pt_dr_log = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGTagger", "ptDrLog"));
    jets -> qg_token_cmult = mayConsume<edm::ValueMap<int>>(edm::InputTag("QGTagger", "cmult"));
    jets -> qg_token_nmult = mayConsume<edm::ValueMap<int>>(edm::InputTag("QGTagger", "nmult"));
    jets -> gen_token    = mayConsume<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjets"));
    for(const auto& dR: jets->dRToProduce)
    {
    jets -> qg_dR_tokens_i[Form("mult-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<int>>(edm::InputTag("QGVariables", Form("mult-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));
    jets -> qg_dR_tokens_i[Form("Gen-mult-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<int>>(edm::InputTag("QGVariables", Form("Gen-mult-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));

    jets -> qg_dR_tokens_f[Form("axis2-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGVariables", Form("axis2-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));
    jets -> qg_dR_tokens_f[Form("Gen-axis2-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGVariables", Form("Gen-axis2-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));
    jets -> qg_dR_tokens_f[Form("ptD-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGVariables", Form("ptD-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));
    jets -> qg_dR_tokens_f[Form("Gen-ptD-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGVariables", Form("Gen-ptD-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));
    jets -> qg_dR_tokens_f[Form("axis1-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGVariables", Form("axis1-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));
    jets -> qg_dR_tokens_f[Form("Gen-axis1-dR0p%03.0f",dR*1000)] = mayConsume<edm::ValueMap<float>>(edm::InputTag("QGVariables", Form("Gen-axis1-dR-0p%03.0f-pT-%.0f",dR*1000,float(500))));
    
    }

    obj.push_back(jets);

    // --
    NeroPhotons *phos = new NeroPhotons();
    phos -> mOnlyMc = onlyMc;
    phos -> token = consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons"));
    phos -> uncalib_token = consumes<pat::PhotonCollection>(edm::InputTag("slimmedPhotons"));
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
    */
    //phos -> PhoCorr = new EnergyScaleCorrection_class("EgammaAnalysis/ElectronTools/data/ScalesSmearings/Winter_2016_reReco_v1_ele");
    //phos->PhoCorr -> doSmearings= true;
    //phos->PhoCorr -> doScale= true;
    
    obj.push_back(phos);

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
    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- begin event --------"<<endl;
    #endif

    for(auto o : obj)
        o->clear();

    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- analyze event --------"<<endl;
    #endif

    for(auto o : obj)
    {
        #ifdef NERO_VERBOSE
            if (NERO_VERBOSE > 1) { cout<<"[Nero]::[analyze] going to analyze "<<o->name() <<endl; }
            if (NERO_VERBOSE){sw_.Reset(); sw_.Start();}
        #endif

        if (o->analyze(iEvent, iSetup) ) return; // analyze return 0 on success (VTX ..)

        #ifdef NERO_VERBOSE
            if (NERO_VERBOSE)
            {
                sw_.Stop(); 
                times_[o->name()] += sw_.CpuTime() ;
            }
            if(NERO_VERBOSE>1)
            {
                cout<< "[Nero]::[analyze] object "<<o->name()
                    <<" took:"<< sw_.CpuTime()<< "CPU Time and "
                    <<sw_.RealTime()<<"RealTime"<<endl;
            }
        #endif
    }

    // compress double precision to float precision
    for(auto o : obj)
    {
        o->compress();
    }

    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- fill event --------"<<endl;
    #endif

    tree_->Fill();
    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- end event (success) --------"<<endl;
        if (NERO_VERBOSE){
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
        } // end NERO_VERBOSE
    #endif

}


// ------------ method called once each job just before starting event loop  ------------
    void 
Nero::beginJob()
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" >>>>>>> BEGIN JOB <<<<<<<<<"<<endl;
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
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE) cout<<" -> Calling defineBranch for "<<o->name()<<endl;
        #endif
        o -> defineBranches(tree_);
    }

    for(auto o : lumiObj)
    {
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE) cout<<" -> Calling defineBranch for "<<o->name()<<endl;
        #endif
        o -> defineBranches(all_);
    }
    
    //set histogram map
    for(auto o : lumiObj){
        if (dynamic_cast<NeroAll*> (o) !=NULL ) {
            #ifdef NERO_VERBOSE
                if(NERO_VERBOSE) cout<<" -> Setting Histograms in "<<o->name()<<endl;
            #endif
            NeroAll * na = dynamic_cast<NeroAll*> (o);
            na->hDEvents =  hD_["Events"];
            na->hDTotalMcWeight = hD_["TotalMcWeight"];
            na->hDPileup = hD_["Pileup"];
            na->hDscaleReweightSums = hD_["scaleReweightSums"];
            na->hDpdfReweightSums = hD_["pdfReweightSums"];
        } // end if
        } // end for o in lumiObj
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" -- end of begin job "<<endl;
    #endif
} //end beginJob


// ------------ method called once each job just after ending the event loop  ------------
    void 
Nero::endJob() 
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" >>>>>>> END JOB <<<<<<<<<"<<endl;
    #endif
}

// ------------ method called when starting to processes a run  ------------

    void 
Nero::beginRun(edm::Run const&iRun, edm::EventSetup const&)
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" ======= BEGIN RUN ======="<<endl;
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
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE> 1) cout<<"[Nero]::[endRun]::[DEBUG] calling object"<<o->name()<<endl;
        #endif
        o->analyzeRun(iRun, hXsec_);
    }
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout <<" ======== END RUN ======="<<endl;
    #endif

}


// ------------ method called when starting to processes a luminosity block  ------------

    void 
Nero::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout <<" -------- BEGIN LUMI --------"<<endl;
    #endif

}


// ------------ method called when ending the processing of a luminosity block  ------------

    void 
Nero::endLuminosityBlock(edm::LuminosityBlock const&iLumi, edm::EventSetup const&)
{
    for(auto o :lumiObj)
    {
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE>1) cout<<"[Nero]::[endLuminosityBlock]::[DEBUG] calling object"<<o->name()<<endl;
        #endif

        o->analyzeLumi(iLumi,all_);
    }

    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout <<" -------- END LUMI --------"<<endl;
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
