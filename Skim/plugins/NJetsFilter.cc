// -*- C++ -*-
//
// Package:    NeroProducer/NJetsFilter
// Class:      NJetsFilter
// 
/**\class NJetsFilter NJetsFilter.cc NeroProducer/NJetsFilter/plugins/NJetsFilter.cc

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
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"


//
// class declaration
//

class NJetsFilter : public edm::EDFilter {
    public:
        explicit NJetsFilter(const edm::ParameterSet&);
        ~NJetsFilter();

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
        edm::EDGetTokenT<pat::JetCollection> jet_token ;	
        edm::Handle<pat::JetCollection> jet_handle;
        //
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
NJetsFilter::NJetsFilter(const edm::ParameterSet& iConfig):
    jet_token( consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets")) )
{
    //now do what ever initialization is needed

}


NJetsFilter::~NJetsFilter()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
    bool
NJetsFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(jet_token, jet_handle);

    int nJets =0 ; // 2.5 eta , pt20 GeV -- preselection like, stricter can be applied offline
    int nBJets = 0;

    for (auto& j : *jet_handle)
    {
    if (j.pt() <20) continue;
    if (fabs(j.eta())>2.5) continue;
    nJets +=1;
    if (j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags")> 0.5) nBJets +=1; //L=0.5426 , M=  0.8484, T0.9535, slightly looser than loose.
    }

    // TB
    // (NcentralJets>=5 && NBJets >=2) || (NcentralJets==4 && NBJets >=3) || (NcentralJets>=5 && NBJets>=1)
    if (nJets ==4 and nBJets >=3) return true;
    if (nJets >=5 and nBJets >=1) return true;

    return false;
}

// ------------ method called once each job just before starting event loop  ------------
    void 
NJetsFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
NJetsFilter::endJob() {
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
NJetsFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(NJetsFilter);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
