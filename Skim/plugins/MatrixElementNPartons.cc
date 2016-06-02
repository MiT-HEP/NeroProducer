// -*- C++ -*-
//
// Package:    NeroProducer/MatrixElementNPartons
// Class:      MatrixElementNPartons
// 
/**\class MatrixElementNPartons MatrixElementNPartons.cc NeroProducer/MatrixElementNPartons/plugins/MatrixElementNPartons.cc

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


#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"


//
// class declaration
//

class MatrixElementNPartons : public edm::EDFilter {
    public:
        explicit MatrixElementNPartons(const edm::ParameterSet&);
        ~MatrixElementNPartons();

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
        edm::EDGetTokenT<GenEventInfoProduct> token_ge ;	
        edm::Handle<GenEventInfoProduct> handle_ge;
        //
        int mePartons{-1};
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
MatrixElementNPartons::MatrixElementNPartons(const edm::ParameterSet& iConfig):
    token_ge( consumes<GenEventInfoProduct>(edm::InputTag("generator")) )
{
    //now do what ever initialization is needed
    mePartons=iConfig.getParameter<int>("matrixElementPartons");

}


MatrixElementNPartons::~MatrixElementNPartons()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
    bool
MatrixElementNPartons::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(token_ge, handle_ge);

    if ((*handle_ge).nMEPartons() == mePartons) return true;

    return false;
}

// ------------ method called once each job just before starting event loop  ------------
    void 
MatrixElementNPartons::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MatrixElementNPartons::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
   void
   MatrixElementNPartons::beginRun(edm::Run const&, edm::EventSetup const&)
   { 
   }
   */

// ------------ method called when ending the processing of a run  ------------
/*
   void
   MatrixElementNPartons::endRun(edm::Run const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when starting to processes a luminosity block  ------------
/*
   void
   MatrixElementNPartons::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when ending the processing of a luminosity block  ------------
/*
   void
   MatrixElementNPartons::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MatrixElementNPartons::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MatrixElementNPartons);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
