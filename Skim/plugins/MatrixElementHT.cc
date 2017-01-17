// -*- C++ -*-
//
// Package:    NeroProducer/MatrixElementHT
// Class:      MatrixElementHT
// 
/**\class MatrixElementHT MatrixElementHT.cc NeroProducer/MatrixElementHT/plugins/MatrixElementHT.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Andrea Carlo Marini
//         Created:  Tue Jan 17 01:37:39 CET 2017
//
//


// system include files
#include <memory>
#include <iostream>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDFilter.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"

#include "TLorentzVector.h"

//
// class declaration
//

class MatrixElementHT : public edm::EDFilter {
    public:
        explicit MatrixElementHT(const edm::ParameterSet&);
        ~MatrixElementHT();

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
        edm::EDGetTokenT<LHEEventProduct> token_lhe ;	
        edm::Handle<LHEEventProduct> handle_lhe;
        //
        double htMin{-1};
        double htMax{-1};
        double qtMin{-1};
        double qtMax{-1};
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
//LHEEventProduct                       "externalLHEProducer"       ""                "LHE"     
MatrixElementHT::MatrixElementHT(const edm::ParameterSet& iConfig):
    token_lhe( consumes<LHEEventProduct>(edm::InputTag("externalLHEProducer")) )
{
    //now do what ever initialization is needed
    htMin=iConfig.getParameter<double>("htMin");
    htMax=iConfig.getParameter<double>("htMax");
    qtMin=iConfig.getParameter<double>("qtMin");
    qtMax=iConfig.getParameter<double>("qtMax");

}


MatrixElementHT::~MatrixElementHT()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called on each new Event  ------------
    bool
MatrixElementHT::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(token_lhe, handle_lhe);

    //compute HT
    const auto& hepeup = handle_lhe->hepeup();

    double ht=0;
    TLorentzVector Q;

    for(unsigned i=0; i<hepeup.IDUP.size(); ++i)
    {
        if (hepeup.ISTUP[i] != 1) continue; //Outgoing final state particle
        TLorentzVector x; 
        // px,py,pz,e,m
        x.SetPxPyPzE( hepeup.PUP[i][0],hepeup.PUP[i][1],hepeup.PUP[i][2],hepeup.PUP[i][3]) ;
        if ((abs(hepeup.IDUP[i]) <=6)  or hepeup.IDUP[i] ==21) {ht+= x.Pt();}
        if ((abs(hepeup.IDUP[i]) >=11) and (abs(hepeup.IDUP[i]) <=16)) Q += x;
    }
    //std::cout <<"HT is "<<ht<<" R="<<H.Pt()<<" QT is "<<Q.Pt()<<std::endl;

    if ( htMin >0 and ht<htMin) return false;
    if ( htMax >0 and ht>=htMax) return false;
    if ( qtMin >0 and Q.Pt()<qtMin) return false;
    if ( qtMax >0 and Q.Pt()>=qtMax) return false;

    return true;
}

// ------------ method called once each job just before starting event loop  ------------
    void 
MatrixElementHT::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
MatrixElementHT::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
   void
   MatrixElementHT::beginRun(edm::Run const&, edm::EventSetup const&)
   { 
   }
   */

// ------------ method called when ending the processing of a run  ------------
/*
   void
   MatrixElementHT::endRun(edm::Run const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when starting to processes a luminosity block  ------------
/*
   void
   MatrixElementHT::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method called when ending the processing of a luminosity block  ------------
/*
   void
   MatrixElementHT::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
   {
   }
   */

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MatrixElementHT::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MatrixElementHT);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
