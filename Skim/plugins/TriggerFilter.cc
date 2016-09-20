// -*- C++ -*-
//
// Package:    NeroProducer/TriggerFilter
// Class:      TriggerFilter
// 
/**\class TriggerFilter TriggerFilter.cc NeroProducer/TriggerFilter/plugins/TriggerFilter.cc

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
// TRIGGER
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"


//
// class declaration
//

class TriggerFilter : public edm::EDFilter {
    public:
        explicit TriggerFilter(const edm::ParameterSet&);
        ~TriggerFilter();

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
        edm::EDGetTokenT<edm::TriggerResults> token ;	
        edm::Handle<edm::TriggerResults> handle;
        //
        std::vector<std::string> triggerNames;
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
TriggerFilter::TriggerFilter(const edm::ParameterSet& iConfig):
    token ( consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("trigger") ) ),
    triggerNames (iConfig.getParameter<std::vector<std::string> > ("triggerNames") )
{
    //now do what ever initialization is needed

}


TriggerFilter::~TriggerFilter()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
    bool
TriggerFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(token, handle);

    const edm::TriggerNames &names = iEvent.triggerNames(*handle);

    if ( handle.isValid() and not handle.failedToGet() ) {
        int ntrigs = (int)names.size();
        for (int i = 0; i < ntrigs; i++) {

            std::string name = names.triggerName(i);

            for(unsigned int j=0;j< triggerNames.size() ;++j) // destination loop
            {
                if (name.find( (triggerNames)[j]) != std::string::npos)
                {
                    if (handle->accept(i)){
                        return true;
                    }
                }

            } // my trigger end
        } // trigger loop
    } // handle is valid
    else{ return true; } // pass the event if failed to get the trigger


    return false;
}

// ------------ method called once each job just before starting event loop  ------------
    void 
TriggerFilter::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TriggerFilter::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
   void
   TriggerFilter::beginRun(edm::Run const&, edm::EventSetup const&)
   { 
   }
   */

// ------------ method called when ending the processing of a run  ------------
/*
   void
   TriggerFilter::endRun(edm::Run const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when starting to processes a luminosity block  ------------
/*
   void
   TriggerFilter::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when ending the processing of a luminosity block  ------------
/*
   void
   TriggerFilter::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TriggerFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TriggerFilter);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
