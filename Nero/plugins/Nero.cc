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


#define VERBOSE 0

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
   // not push_back inline because he needs to know the class type for init
   NeroEvent *evt = new NeroEvent();
   obj.push_back(evt);

   // -- Before Leptons (mu uses Vtx)
   NeroVertex *vtx = new NeroVertex();
   vtx -> token = consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
   obj.push_back(vtx);

   //now do what ever initialization is needed
   NeroJets *jets = new NeroJets();
   jets -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets"));
   jets -> qg_token = consumes<edm::ValueMap<float>>(edm::InputTag("QGTagger", "qgLikelihood"));
   obj.push_back(jets);

   // --- 
   NeroTaus *taus = new NeroTaus();
   taus -> token = consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus"));
   obj.push_back(taus);

   //--
   NeroLeptons *leps = new NeroLeptons();
   leps -> mu_token = consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons"));
   leps -> el_token = consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons"));
   leps -> el_mediumid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleMediumIdMap"));
   leps -> el_tightid_token = consumes<edm::ValueMap<bool> >(iConfig.getParameter<edm::InputTag>("eleTightIdMap"));
   obj. push_back(leps);

   //--
   NeroFatJets *fatjets = new NeroFatJets();
   fatjets -> token = consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("fatjets"));
   obj.push_back(fatjets);

   //--
   NeroMet *met = new NeroMet();
   met -> token = consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("mets"));
   obj.push_back(met);

   // --
   NeroPhotons *phos = new NeroPhotons();
   phos -> token = consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons"));
   phos -> medium_id_token = consumes<edm::ValueMap<bool>>(iConfig.getParameter<edm::InputTag>("phoMediumIdMap"));
   phos -> iso_ch_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoChargedIsolation"));
   phos -> iso_nh_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoNeutralHadronIsolation"));
   phos -> iso_pho_token = consumes<edm::ValueMap<float>>(iConfig.getParameter<edm::InputTag>("phoPhotonIsolation"));
   obj.push_back(phos);

   NeroMonteCarlo *mc = new NeroMonteCarlo();
   mc -> packed_token = consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("packedgen"));
   mc -> pruned_token = consumes<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("prunedgen")) ;
   mc -> info_token   = consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generator"));
   mc -> pu_token     = consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileup"));
   mc -> jet_token    = consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjets"));
   mc -> runinfo_token = consumes<GenRunInfoProduct>(iConfig.getParameter<edm::InputTag>("genruninfo") );
   obj.push_back(mc);

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
   if (VERBOSE) cout<<"------- begin event --------"<<endl;
  
   for(auto o : obj)
   	o->clear();

   const reco::Vertex *pv = NULL;
   for(auto o : obj)
	{
	if (VERBOSE){sw_.Reset(); sw_.Start();}
	if ( dynamic_cast<NeroLeptons*>(o) != NULL ) dynamic_cast<NeroLeptons*>(o) -> SetPV( pv );

   	if (o->analyze(iEvent) ) return; // analyze return 0 on success (VTX ..)

	if ( dynamic_cast<NeroVertex*>(o) != NULL ) pv = dynamic_cast<NeroVertex*>(o) ->GetPV(); 

	if (VERBOSE){sw_.Stop(); cout<< "object "<<o->name()<<" took:"<< sw_.CpuTime()<< "CPU Time and "<<sw_.RealTime()<<"RealTime"<<endl;}
	}
   	

   tree_->Fill();
   all_->Fill();
   if (VERBOSE) cout<<"------- end event (success) --------"<<endl;

}


// ------------ method called once each job just before starting event loop  ------------
void 
Nero::beginJob()
{
	if(VERBOSE) cout<<" >>>>>>> BEGIN JOB <<<<<<<<<"<<endl;
	tree_    = fileService_ -> make<TTree>("events", "events");
	all_     = fileService_ -> make<TTree>("all", "all");	  
	hXsec_   = fileService_ -> make<TH1F>("xSec", "xSec",20,-0.5,19.5); hXsec_ ->Sumw2();

	for(auto o : obj)
		o -> defineBranches(tree_);
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
	for(auto o : obj)
		{
		if ( dynamic_cast<NeroMonteCarlo*> (o) != NULL)
			{
			dynamic_cast<NeroMonteCarlo*> (o) -> crossSection(iRun, hXsec_);
			}
		}
	if(VERBOSE) cout <<" ======== END RUN ======="<<endl;
}


// ------------ method called when starting to processes a luminosity block  ------------
/*
void 
Nero::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

// ------------ method called when ending the processing of a luminosity block  ------------
/*
void 
Nero::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}
*/

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
