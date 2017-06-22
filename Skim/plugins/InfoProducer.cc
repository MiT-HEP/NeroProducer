// -*- C++ -*-
//
// Package:    NtupleTools/InfoProducer
// Class:      InfoProducer
// 
/**\class InfoProducer InfoProducer.cc NtupleTools/InfoProducer/plugins/InfoProducer.cc

Description: Additional information about jets

Implementation:
Add information missing in the input edm files.
*/


// system include files
#include <memory>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/one/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Common/interface/View.h"
#include "DataFormats/JetReco/interface/Jet.h"

#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "DataFormats/Common/interface/MergeableCounter.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"

using namespace edm;
using namespace std;

class InfoProducer : public edm::one::EDProducer<edm::one::WatchLuminosityBlocks,
    edm::EndLuminosityBlockProducer>{
        public:
            explicit InfoProducer(const edm::ParameterSet&);
            ~InfoProducer();


        private:
            virtual void produce(edm::Event&, const edm::EventSetup&) override;
            virtual void beginLuminosityBlock(const edm::LuminosityBlock &, const edm::EventSetup&) override;
            virtual void endLuminosityBlock(edm::LuminosityBlock const&, const edm::EventSetup&) override;
            virtual void endLuminosityBlockProduce(edm::LuminosityBlock &, const edm::EventSetup&) override;
            // ----------member data ---------------------------
            unsigned int eventsProcessedInLumi_;
            double mcWeights_;

            template <class T>
                void inline copyVector( const vector<T> &a, vector<T> &b){ b.clear(); for (auto x : a) b.push_back(x); } 

            vector<long> vecEvents_;
            vector<float> vecMcWeights_;
            vector<int> vecPuTrueInt_;

            edm::EDGetTokenT<GenEventInfoProduct> info_token; 
            edm::Handle<GenEventInfoProduct> info_handle;

            edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pu_token;
            edm::Handle< std::vector<PileupSummaryInfo> > pu_handle;


    };

InfoProducer::InfoProducer(const edm::ParameterSet& iConfig)
    : info_token(consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generator")) ),
    //pu_token(consumes<std::vector<PileupSummaryInfo> >(edm::InputTag("addPileupInfo")))
    pu_token(consumes<std::vector<PileupSummaryInfo> >(edm::InputTag("slimmedAddPileupInfo")))
{
    //register your products
    //produces<edm::ValueMap<bool> >("monojetSelection");
    produces<edm::MergeableCounter, edm::InLumi>("numberEvents");
    produces<edm::MergeableCounter, edm::InLumi>("sumMcWeights");

    // save this info for all the events, in the lumiblock
    produces<std::vector<long>,   edm::InLumi>("vecEvents");
    produces<std::vector<float>, edm::InLumi>("vecMcWeights");
    produces<std::vector<int>,  edm::InLumi>("vecPuTrueInt");
}


InfoProducer::~InfoProducer(){}

    void
InfoProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    // edm::Handle<edm::View<reco::Jet> > ak4Jets;
    // iEvent.getByLabel("slimmedJets", ak4Jets);
    // prepare output

    // fill information
    eventsProcessedInLumi_++;
    vecEvents_.push_back(iEvent.id().event());


    if ( not iEvent.isRealData() )
    {
        // define input
        iEvent.getByToken(info_token, info_handle);  
        mcWeights_ += info_handle -> weight();

        vecMcWeights_ . push_back( info_handle -> weight());

        iEvent.getByToken(pu_token, pu_handle);

        int  puTrueInt = 0;
        for(const auto & pu : *pu_handle)
        {
            //Intime
            if (pu.getBunchCrossing() == 0)
                puTrueInt += pu.getTrueNumInteractions();
            //puInt += getPU_NumInteractions(); //old
            //Out-of-time
        }


        vecPuTrueInt_ . push_back(puTrueInt);
    }

    // finalize
}


void InfoProducer::beginLuminosityBlock(const edm::LuminosityBlock &, const edm::EventSetup&) 
{
    mcWeights_ = 0;
    eventsProcessedInLumi_ = 0;

    vecMcWeights_ . clear();
    vecPuTrueInt_ . clear();
    vecEvents_    . clear();
}

void InfoProducer::endLuminosityBlock(edm::LuminosityBlock const&, const edm::EventSetup&) 
{}

void InfoProducer::endLuminosityBlockProduce(edm::LuminosityBlock &iLumi, const edm::EventSetup&)
{
    // ---
    unique_ptr<edm::MergeableCounter> numEventsPtr(new edm::MergeableCounter);
    unique_ptr<edm::MergeableCounter> sumMcWeights(new edm::MergeableCounter);

    numEventsPtr->value = eventsProcessedInLumi_;
    sumMcWeights->value = mcWeights_;

    iLumi.put(numEventsPtr,"numberEvents");
    iLumi.put(sumMcWeights,"sumMcWeights");

    // ---
    unique_ptr<std::vector<long> > vecEventsPtr( new vector<long> );
    unique_ptr<std::vector<float> > vecMcWeightsPtr( new vector<float> );
    unique_ptr<std::vector<int> > vecPuTrueIntPtr( new vector<int> );

    copyVector(vecEvents_, *vecEventsPtr.get());
    copyVector(vecMcWeights_, *vecMcWeightsPtr.get());
    copyVector(vecPuTrueInt_, *vecPuTrueIntPtr.get());
    //
    iLumi.put(vecEventsPtr,"vecEvents");
    iLumi.put(vecMcWeightsPtr,"vecMcWeights");
    iLumi.put(vecPuTrueIntPtr,"vecPuTrueInt");
}
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------

//define this as a plug-in
DEFINE_FWK_MODULE(InfoProducer);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
