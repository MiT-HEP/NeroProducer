// -*- C++ -*-
//
// Package:    NeroProducer/DoubleEGFilter
// Class:      DoubleEGFilter
// 
/**\class DoubleEGFilter DoubleEGFilter.cc NeroProducer/DoubleEGFilter/plugins/DoubleEGFilter.cc

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

#include "TLorentzVector.h"

//
// class declaration
//

class DoubleEGFilter : public edm::EDFilter {
    public:
        explicit DoubleEGFilter(const edm::ParameterSet&);
        ~DoubleEGFilter();

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
        //
        edm::EDGetTokenT<pat::PhotonCollection> pho_token;
        edm::Handle<pat::PhotonCollection> pho_handle;
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
DoubleEGFilter::DoubleEGFilter(const edm::ParameterSet& iConfig):
    pho_token(  consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons")) )
{
    //now do what ever initialization is needed

}


DoubleEGFilter::~DoubleEGFilter()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
    bool
DoubleEGFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(pho_token, pho_handle);

    int nPho=0;
    double massCut=400;

    //for (auto &pho : *handle)
    for (unsigned iPho=0 ; iPho < pho_handle->size() ;++iPho)
    {
        const auto& pho = pho_handle->at(iPho);

        if (fabs(pho.eta()) >2.5) continue;
        if (pho.pt() > 150) ++nPho ; 

        if (nPho >= 2) return true; // AS SOON AS POSSIBLE GIVE THE POSITIVE RESPONSE

        TLorentzVector l(pho.px(),pho.py(),pho.pz(),pho.energy() ) ;
        for (unsigned jPho=0 ; jPho < pho_handle->size() ;++jPho)
        {
            if (iPho == jPho) continue;
            const auto& jpho = pho_handle->at(jPho);
            TLorentzVector l2(jpho.px(),jpho.py(),jpho.pz(),jpho.energy() ) ;
            l2 += l;
            if ( l2.M()  > massCut) return true; // AS SOON AS POSSIBLE GIV THE POSITIVE RESPONSE
        }

    }


    return false;
}

// ------------ method called once each job just before starting event loop  ------------
    void 
DoubleEGFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
DoubleEGFilter::endJob() {
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DoubleEGFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DoubleEGFilter);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
