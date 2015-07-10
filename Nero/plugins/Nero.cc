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
#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"
#include "NeroProducer/Nero/interface/NeroAll.hpp"
#include "NeroProducer/Nero/interface/NeroTrigger.hpp"
#include "NeroProducer/Nero/interface/NeroMatching.hpp"

#define VERBOSE 2

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

    bool onlyMc = iConfig.getParameter<bool>("onlyMc");

    // not push_back inline because he needs to know the class type for init
    NeroEvent *evt = new NeroEvent();
    evt -> mOnlyMc = onlyMc;
    evt -> rho_token = consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
    obj.push_back(evt);

    // -- Before Leptons (mu uses Vtx)
    NeroVertex *vtx = new NeroVertex();
    vtx -> mOnlyMc = onlyMc;
    vtx -> token = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
    obj.push_back(vtx);

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
    obj.push_back(jets);

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
    leps -> mu_token = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
    leps -> el_token = consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
    leps -> el_vetoid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleVetoIdMap"));
    leps -> el_mediumid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleMediumIdMap"));
    leps -> el_tightid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleTightIdMap"));
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

    obj. push_back(leps);

    //--
    NeroFatJets *fatjets = new NeroFatJets();
    fatjets -> mOnlyMc = onlyMc;
    fatjets -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("fatjets"));
    obj.push_back(fatjets);

    //--
    NeroMet *met = new NeroMet();
    met -> mOnlyMc = onlyMc;
    met -> token = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("mets"));
    obj.push_back(met);

    // --
    NeroPhotons *phos = new NeroPhotons();
    phos -> mOnlyMc = onlyMc;
    phos -> token = consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons"));
    phos -> loose_id_token = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoLooseIdMap"));
    phos -> medium_id_token = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoMediumIdMap"));
    phos -> tight_id_token = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoTightIdMap"));
    phos -> iso_ch_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoChargedIsolation"));
    phos -> iso_nh_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoNeutralHadronIsolation"));
    phos -> iso_pho_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoPhotonIsolation"));
    phos -> mMinPt = iConfig.getParameter<double>("minPhoPt");
    phos -> mMaxIso = iConfig.getParameter<double>("maxPhoIso");
    phos -> mMinNpho = iConfig.getParameter<int>("minPhoN");
    phos -> mMinEta = iConfig.getParameter<double>("minPhoEta");
    phos -> SetMatch( iConfig.getParameter<bool>("matchPho") );

    obj.push_back(phos);

    NeroMonteCarlo *mc = new NeroMonteCarlo();
    mc -> mOnlyMc = onlyMc;
    mc -> packed_token = consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("packedgen"));
    mc -> pruned_token = consumes<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("prunedgen")) ;
    mc -> info_token   = consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generator"));
    mc -> pu_token     = consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileup"));
    mc -> jet_token    = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjets"));
    mc -> runinfo_token = consumes<GenRunInfoProduct>(iConfig.getParameter<edm::InputTag>("genruninfo") );
    mc -> mMinGenParticlePt = iConfig.getParameter<double>("minGenParticlePt");
    mc -> mMinGenJetPt = iConfig.getParameter<double>("minGenJetPt");

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
    if (VERBOSE>1) cout<<"------- begin event --------"<<endl;

    for(auto o : obj)
        o->clear();

    if (VERBOSE>1) cout<<"------- analyze event --------"<<endl;
    for(auto o : obj)
    {
        if (VERBOSE){sw_.Reset(); sw_.Start();}

        if (o->analyze(iEvent) ) return; // analyze return 0 on success (VTX ..)

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
    }

    if (VERBOSE>1) cout<<"------- fill event --------"<<endl;
    tree_->Fill();
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

}


// ------------ method called once each job just before starting event loop  ------------
    void 
Nero::beginJob()
{
    if(VERBOSE) cout<<" >>>>>>> BEGIN JOB <<<<<<<<<"<<endl;
    tree_    = fileService_ -> make<TTree>("events", "events");
    all_     = fileService_ -> make<TTree>("all", "all");	  
    hXsec_   = fileService_ -> make<TH1F>("xSec", "xSec",20,-0.5,19.5); hXsec_ ->Sumw2();

    fileService_ ->make<TNamed>("tag",tag_.c_str() );
    fileService_ ->make<TNamed>("head",head_.c_str() );
    fileService_ ->make<TNamed>("info",info_.c_str() );

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

    for(auto o : obj)
        o -> defineBranches(tree_);

    for(auto o : lumiObj)
        o -> defineBranches(all_);
}


// ------------ method called once each job just after ending the event loop  ------------
    void 
Nero::endJob() 
{
    if(VERBOSE) cout<<" >>>>>>> END JOB <<<<<<<<<"<<endl;
}

// ------------ method called when starting to processes a run  ------------

    void 
Nero::beginRun(edm::Run const&iRun, edm::EventSetup const&)
{
    if(VERBOSE) cout<<" ======= BEGIN RUN ======="<<endl;
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
        if(VERBOSE> 1) cout<<"[Nero]::[endRun]::[DEBUG] calling object"<<o->name()<<endl;
        o->analyzeRun(iRun, hXsec_);
    }
    if(VERBOSE) cout <<" ======== END RUN ======="<<endl;
}


// ------------ method called when starting to processes a luminosity block  ------------

    void 
Nero::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
    if(VERBOSE) cout <<" -------- BEGIN LUMI --------"<<endl;
}


// ------------ method called when ending the processing of a luminosity block  ------------

    void 
Nero::endLuminosityBlock(edm::LuminosityBlock const&iLumi, edm::EventSetup const&)
{
    for(auto o :lumiObj)
    {
        if(VERBOSE>1) cout<<"[Nero]::[endLuminosityBlock]::[DEBUG] calling object"<<o->name()<<endl;
        o->analyzeLumi(iLumi,all_);
    }

    if(VERBOSE) cout <<" -------- END LUMI --------"<<endl;
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
