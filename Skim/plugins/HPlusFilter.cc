// -*- C++ -*-
//
// Package:    NeroProducer/HPlusFilter
// Class:      HPlusFilter
// 
/**\class HPlusFilter HPlusFilter.cc NeroProducer/HPlusFilter/plugins/HPlusFilter.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Andrea Carlo Marini
//         Created:  Sun, 24 May 2015 08:43:14 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"


//
// class declaration
//

class HPlusFilter : public edm::EDFilter {
    public:
        explicit HPlusFilter(const edm::ParameterSet&);
        ~HPlusFilter();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual bool filter(edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;

        //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
        //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
        //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
        //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

        // ----------member data ---------------------------
        edm::EDGetTokenT<pat::TauCollection> tau_token ;	
        edm::Handle<pat::TauCollection> tau_handle;
        //
        edm::EDGetTokenT<pat::MuonCollection> mu_token;
        edm::Handle<pat::MuonCollection> mu_handle;
        //
        edm::EDGetTokenT<pat::ElectronCollection> el_token;
        edm::Handle<pat::ElectronCollection> el_handle;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HPlusFilter::HPlusFilter(const edm::ParameterSet& iConfig):
    tau_token( consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus")) ),
    mu_token(  consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons")) ),
    el_token(  consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons")) )
{
    //now do what ever initialization is needed

}


HPlusFilter::~HPlusFilter()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
    bool
HPlusFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(mu_token, mu_handle);
    iEvent.getByToken(el_token, el_handle);

    int nLeptons = mu_handle->size() + el_handle->size();

    if (nLeptons >=2) return true;

    // OR ...
    iEvent.getByToken(tau_token, tau_handle);
    // at least 1 tau
    if (tau_handle->size() >0 ) return true;


    return false;
}

// ------------ method called once each job just before starting event loop  ------------
    void 
HPlusFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
HPlusFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
   void
   HPlusFilter::beginRun(edm::Run const&, edm::EventSetup const&)
   { 
   }
   */

// ------------ method called when ending the processing of a run  ------------
/*
   void
   HPlusFilter::endRun(edm::Run const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when starting to processes a luminosity block  ------------
/*
   void
   HPlusFilter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when ending the processing of a luminosity block  ------------
/*
   void
   HPlusFilter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
HPlusFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(HPlusFilter);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
