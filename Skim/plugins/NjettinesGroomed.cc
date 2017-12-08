#include <memory>
#include <iostream>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaPhi.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"


// GEN
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"


#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


//fastjet
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/contrib/Njettiness.hh"
#include "fastjet/contrib/NjettinessPlugin.hh"
#include "fastjet/contrib/SoftDrop.hh"

#define NJETTINES_GROOMED_DEBUG 0

using namespace fastjet;
using namespace std;

/**
 * EDProducer class to produced the  QG related variables with subclustering
 * Authors: andrea.carlo.marini@cern.ch
 */

class NjettinessGroomed : public edm::EDProducer{
    public:
        explicit NjettinessGroomed(const edm::ParameterSet&);
        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void produce(edm::Event&, const edm::EventSetup&);


        template <typename T,typename J> void putInEvent(std::string, const edm::Handle< J >&, std::vector<T>*, edm::Event&);

        edm::EDGetTokenT<edm::View<reco::Jet>> 	jetsToken;

        vector<PseudoJet> input_particles;

        int maxTau_{3};
        float R_{0.8};
        JetDefinition AKDef{JetDefinition(antikt_algorithm, R_)};
        // AK8 R=0.8, beta=0 and z=0.1
        // CA R=1.5, beta=1.0 and z=0.15
        fastjet::contrib::SoftDrop softdrop_ {fastjet::contrib::SoftDrop(0.0, 0.1, R_)};
        fastjet::contrib::Njettiness *tau_ { new fastjet::contrib::Njettiness(fastjet::contrib::OnePass_KT_Axes(), fastjet::contrib::NormalizedMeasure(1., R_)) };

};

/// Function to put product into event
template <typename T,typename J> void NjettinessGroomed::putInEvent(std::string name, const edm::Handle< J >& jets, std::vector<T>* product, edm::Event& iEvent){
    std::unique_ptr<edm::ValueMap<T>> out(new edm::ValueMap<T>());
    typename edm::ValueMap<T>::Filler filler(*out);
    filler.insert(jets, product->begin(), product->end());
    filler.fill();
    iEvent.put(std::move(out), name);
    delete product;
}

NjettinessGroomed::NjettinessGroomed(const edm::ParameterSet& iConfig) :
  jetsToken( 		consumes<edm::View<reco::Jet>>(		iConfig.getParameter<edm::InputTag>("srcJets")))//slimmedJets
{
    for(int tau=1 ; tau<=maxTau_;++tau){
        produces<edm::ValueMap<float>>(Form("Nsubjettiness:softdrop:tau%d",tau));
    }
}


/// Produce qgLikelihood using {mult, ptD, -log(axis2)}
void NjettinessGroomed::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    if(NJETTINES_GROOMED_DEBUG){ 
        cout<<"----------------------------------------"<<endl;
        cout<<"|"<< iEvent.id().run()<<":"<<iEvent.luminosityBlock()<<":"<<iEvent.id().event()<<"|"<<endl;
        cout<<"----------------------------------------"<<endl;
    }
    //clear
    input_particles.clear();
    edm::Handle<edm::View<reco::Jet>> jets;
    iEvent.getByToken(jetsToken, 		jets);


    std::map<std::string, std::vector<float>* > mapsProducts;

    for(int tau=1;tau<=maxTau_;++tau)
    {
        mapsProducts[Form("tau%d",tau)]  = new vector<float>;
    }

    for(auto jet = jets->begin(); jet != jets->end(); ++jet)
    {
        input_particles.clear();
        for(auto daughter : jet->getJetConstituentsQuick()){
            auto part = static_cast<const pat::PackedCandidate*>(daughter);

            input_particles.push_back(PseudoJet (part->px(),part->py(),part->pz(),part->energy()) );
        }
        // Rereun cluster sequence
        ClusterSequence seq(input_particles, AKDef);
        auto allJets=sorted_by_pt(seq.inclusive_jets(0.0));
        if (allJets.size()>0)
        {
            auto& leadingJet(allJets[0]);
            PseudoJet sdJet((softdrop_)(leadingJet));
            const auto& sdconsts = sorted_by_pt(sdJet.constituents());
            for(int tau=1;tau<=maxTau_;++tau)
            {
                mapsProducts[Form("tau%d",tau)] -> push_back( tau_->getTau(tau, sdconsts));
            }

        }
        else{
            for(int tau=1;tau<=maxTau_;++tau)
            {
                mapsProducts[Form("tau%d",tau)] -> push_back(-999.);
            }
        }

    } // end jet loop

    for(int tau=1;tau<=maxTau_;++tau)
    {
        putInEvent( Form("Nsubjettiness:softdrop:tau%d",tau), jets, mapsProducts[Form("tau%d",tau)],iEvent );
    }

}


/// Descriptions method
void NjettinessGroomed::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
}


//define this as a plug-in
DEFINE_FWK_MODULE(NjettinessGroomed);
