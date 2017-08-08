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

#define QGVARIABLES_DEBUG 0

using namespace fastjet;
using namespace std;

/**
 * EDProducer class to produced the  QG related variables with subclustering
 * Authors: andrea.carlo.marini@cern.ch
 */

class QGVariables : public edm::EDProducer{
   public:
      explicit QGVariables(const edm::ParameterSet&);
      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

   private:
      virtual void produce(edm::Event&, const edm::EventSetup&);

      //std::tuple<int, int, int, float, float, float, float> calcVariables(const reco::Jet *jet, edm::Handle<reco::VertexCollection>& vC, float dR=0.0001);
      map<string,float> calcVariables(float jeta=0.0, float jphi=0.0, float dR=0.00001,float pt=0.5); // calc Variables from  inputParticles
      map<string,float> calcVariablesPat(const reco::Jet *jet,float dR=0.00001,float pt=0.5); // calc Variables from  inputParticles
      map<string,float> calcVariablesGen(const reco::GenJet *jet,float dR=0.00001,float pt=0.5); // calc Variables from  inputParticles

      template <typename T,typename J> void putInEvent(std::string, const edm::Handle< J >&, std::vector<T>*, edm::Event&);

      edm::EDGetTokenT<edm::View<reco::Jet>> 	jetsToken;
      edm::EDGetTokenT<reco::GenJetCollection> 	genjetsToken;

      vector<PseudoJet> input_particles;
      vector<float> dRToProduce{0.01,0.02,0.03,0.04,0.05,0.075,0.10,0.15,0.20,0.25};
      vector<float> ptToProduce{0.5};
      bool isData{false};
      map<int,JetDefinition> AKDef;
      
};

/// Function to put product into event
template <typename T,typename J> void QGVariables::putInEvent(std::string name, const edm::Handle< J >& jets, std::vector<T>* product, edm::Event& iEvent){
  std::auto_ptr<edm::ValueMap<T>> out(new edm::ValueMap<T>());
  typename edm::ValueMap<T>::Filler filler(*out);
  filler.insert(jets, product->begin(), product->end());
  filler.fill();
  iEvent.put(out, name);
  delete product;
}

QGVariables::QGVariables(const edm::ParameterSet& iConfig) :
  jetsToken( 		consumes<edm::View<reco::Jet>>(		iConfig.getParameter<edm::InputTag>("srcJets"))),//slimmedJets
  genjetsToken( 	consumes<reco::GenJetCollection>(		iConfig.getParameter<edm::InputTag>("srcGenJets"))), //slimmedGenJets
  isData( 	iConfig.getParameter<bool>("isData")) //slimmedGenJets
{
  for(const auto dR : dRToProduce)
  {
        AKDef[int(dR*1000) ] = JetDefinition(antikt_algorithm, dR);
  }

  for(const auto dR : dRToProduce)
  for(const auto pt : ptToProduce){
        produces<edm::ValueMap<float>>(Form("axis2-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
        produces<edm::ValueMap<float>>(Form("axis1-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
        produces<edm::ValueMap<int>>  (Form("mult-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
        produces<edm::ValueMap<float>>(Form("ptD-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
        produces<edm::ValueMap<float>>(Form("ptDrLog-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
        //GEN
        if (not isData){
            produces<edm::ValueMap<float>>(Form("Gen-axis2-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
            produces<edm::ValueMap<float>>(Form("Gen-axis1-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
            produces<edm::ValueMap<int>>  (Form("Gen-mult-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
            produces<edm::ValueMap<float>>(Form("Gen-ptD-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
            produces<edm::ValueMap<float>>(Form("Gen-ptDrLog-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000));
        }
    }
}


/// Produce qgLikelihood using {mult, ptD, -log(axis2)}
void QGVariables::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  if(QGVARIABLES_DEBUG){ 
      cout<<"----------------------------------------"<<endl;
      cout<<"|"<< iEvent.id().run()<<":"<<iEvent.luminosityBlock()<<":"<<iEvent.id().event()<<"|"<<endl;
      cout<<"----------------------------------------"<<endl;
  }
  //clear
  input_particles.clear();
  edm::Handle<edm::View<reco::Jet>> jets;
  iEvent.getByToken(jetsToken, 		jets);

  //
  for(const auto dR : dRToProduce)
  for(const auto pt : ptToProduce)
  {
    std::vector<float>* axis2Product 		= new std::vector<float>;
    std::vector<float>* axis1Product 		= new std::vector<float>;
    std::vector<int>*   multProduct 		= new std::vector<int>;
    std::vector<float>* ptDProduct 		    = new std::vector<float>;
    std::vector<float>* pt_dr_logProduct 	= new std::vector<float>;



    for(auto jet = jets->begin(); jet != jets->end(); ++jet)
    {
        map<string,float> vars = calcVariablesPat(&*jet,dR);

        axis1Product->push_back(vars["axis1"]);
        axis2Product->push_back(vars["axis2"]);
        multProduct ->push_back(vars["mult"]);
        ptDProduct  ->push_back(vars["ptD"]);
        pt_dr_logProduct->push_back(vars["pt_dr_log"]);

    }

    putInEvent(Form("axis2-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),    jets, axis2Product, iEvent);
    putInEvent(Form("axis1-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),    jets, axis1Product, iEvent);
    putInEvent(Form("mult-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),     jets, multProduct,  iEvent);
    putInEvent(Form("ptD-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),      jets, ptDProduct,   iEvent);
    putInEvent(Form("ptDrLog-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),  jets, pt_dr_logProduct,iEvent);
  }

  // gen loop
  if (not isData){
      edm::Handle<reco::GenJetCollection> genjets;
      iEvent.getByToken(genjetsToken, 		genjets);
      for(const auto dR : dRToProduce)
      for(const auto pt : ptToProduce)
      {
          std::vector<float>* axis2Product 		= new std::vector<float>;
          std::vector<float>* axis1Product 		= new std::vector<float>;
          std::vector<int>*   multProduct 		= new std::vector<int>;
          std::vector<float>* ptDProduct 	    = new std::vector<float>;
          std::vector<float>* pt_dr_logProduct 	= new std::vector<float>;



          for(auto jet = genjets->begin(); jet != genjets->end(); ++jet)
          {

              map<string,float> vars = calcVariablesGen(&*jet,dR);

              axis1Product->push_back(vars["axis1"]);
              axis2Product->push_back(vars["axis2"]);
              multProduct ->push_back(vars["mult"]);
              ptDProduct  ->push_back(vars["ptD"]);
              pt_dr_logProduct->push_back(vars["pt_dr_log"]);

              if (QGVARIABLES_DEBUG>1)
              {
                cout <<"Vars GEN jet Pt="<<jet->pt()<<" eta"<<jet->eta()<<" phi="<<jet->phi()<< "dR="<<dR<<endl;
                cout <<"    mult="<<vars["mult"]<<" ptD="<<vars["ptD"]<<endl;
              }
              }

          putInEvent(Form("Gen-axis2-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),      genjets, axis2Product, iEvent);
          putInEvent(Form("Gen-axis1-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),      genjets, axis1Product, iEvent);
          putInEvent(Form("Gen-mult-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),       genjets, multProduct,  iEvent);
          putInEvent(Form("Gen-ptD-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),        genjets, ptDProduct,   iEvent);
          putInEvent(Form("Gen-ptDrLog-dR-0p%03.0f-pT-%.0f",dR*1000,pt*1000),    genjets, pt_dr_logProduct,iEvent);
      }
  }
}


/// Calculation of axis2, mult and ptD
// reco
std::map<string,float> QGVariables::calcVariablesPat(const reco::Jet *jet,float dR,float pt){
    //Loop over the jet constituents
    if (QGVARIABLES_DEBUG) cout<<"calVariables PAT pt="<<jet->pt()<<":"<<jet->eta()<<":dR="<<dR<<endl;
    input_particles.clear();
    for(auto daughter : jet->getJetConstituentsQuick()){
            auto part = static_cast<const pat::PackedCandidate*>(daughter);

            if(part->charge()){
                if(!(part->fromPV() > 1 && part->trackHighPurity())) continue;
                //++mult;
                //++cmult;
            } else {
                if(part->pt() < 1.0) continue;
                //++mult;
                //++nmult;
            }

           input_particles.push_back(PseudoJet (part->px(),part->py(),part->pz(),part->energy()) );
    }
    return calcVariables(jet->eta(),jet->phi(),dR,pt);
}

std::map<string,float> QGVariables::calcVariablesGen(const reco::GenJet *jet,float dR,float pt){
    //Loop over the jet constituents
    if(QGVARIABLES_DEBUG)cout<<"calVariables GEN pt="<<jet->pt()<<":"<<jet->eta()<<":dR="<<dR<<endl;

    input_particles.clear();
    //for(auto daughter : jet->getGenConstituents () ){
    for(auto p : jet->getJetConstituentsQuick()){
           //auto daughter = static_cast<const reco::GenParticle*>(p); 
           //if ( abs(daughter->pdgId() ) == 12 or abs(daughter->pdgId() ) == 14 or abs(daughter->pdgId() ) == 16 ) continue;
           //cout<<"  considering daughter pt="<<daughter->pt()<<":"<<daughter->eta()<<":"<<daughter->phi()<<" PDG"<<daughter->pdgId()<<endl;
           //cout<<"       <*> "<<p->pt()<<":"<<p->eta()<<":"<<p->phi()<<endl;
           //input_particles.push_back(PseudoJet (daughter->px(),daughter->py(),daughter->pz(),daughter->energy()) );
           //protect from gen particles with 0 pt 
           PseudoJet pj(p->px(),p->py(),p->pz(),p->energy()) ;
           if ( pj.eta() <10 and not std::isnan(pj.perp() ) )input_particles.push_back(pj);
    }
    return calcVariables(jet->eta(),jet->phi(),dR,pt);
}

// gen

map<string,float> QGVariables::calcVariables(float jeta, float jphi,float dR,float pt){
    if(QGVARIABLES_DEBUG) cout<<"calVariables dR="<<dR<<endl;
    map<string,float> vars;

    if(QGVARIABLES_DEBUG) cout<<" --> INPUT="<<input_particles.size()<<endl;

    if(QGVARIABLES_DEBUG>1){
            cout<<"    Running cluster sequence on inputs:"<<endl;
        for(size_t i=0;i<input_particles.size();++i){
            cout<<"    * "<<input_particles.at(i).perp()<<" :"<<input_particles.at(i).eta()<<" :"<<input_particles.at(i).phi()<<endl;
        }
    }

    if (input_particles.size() == 0 ){
        vars["mult"] = 0;
        vars["ptD"] = -999;
        vars["axis1"] = -999;
        vars["axis2"] = -999;
        vars["pt_dr_log"] = -999;
        if(QGVARIABLES_DEBUG)cout<<"end calc variables (FAST1): "<<vars["ptD"]<<":"<<vars["mult"]<<endl;
        return vars;
    }

    //JetDefinition ak_def(antikt_algorithm, dR);
    ClusterSequence seq(input_particles, AKDef[int(dR*1000) ] );
    auto inclusive_jets = sorted_by_pt(seq.inclusive_jets(pt)); // ptmin 500 MeV
    
    vars["mult"] = inclusive_jets.size();

    if(QGVARIABLES_DEBUG) cout<<" --> MULT="<<vars["mult"]<<endl;

    if (inclusive_jets.size() == 0 ){
        vars["mult"] = 0;
        vars["ptD"] = -999;
        vars["axis1"] = -999;
        vars["axis2"] = -999;
        vars["pt_dr_log"] = -999;
        if(QGVARIABLES_DEBUG)cout<<"end calc variables (FAST2): "<<vars["ptD"]<<":"<<vars["mult"]<<endl;
        return vars;
    }


    float sum_weight = 0., sum_deta = 0., sum_dphi = 0., sum_deta2 = 0., sum_dphi2 = 0., sum_detadphi = 0., sum_pt = 0.;
    float pt_dr_log = 0;

    for(size_t i=0;i<inclusive_jets.size() ;++i)
    {
        float pt= inclusive_jets.at(i).perp();
        float eta=inclusive_jets.at(i).eta();
        float phi=inclusive_jets.at(i).phi();

        float deta = eta - jeta;
        float dphi = reco::deltaPhi(phi, jphi);
        float weight = pt*pt;

        sum_weight += weight;
        sum_pt += pt;
        sum_deta += deta*weight;
        sum_dphi += dphi*weight;
        sum_deta2 += deta*deta*weight;
        sum_detadphi += deta*dphi*weight;
        sum_dphi2 += dphi*dphi*weight;


        float dr = std::sqrt(reco::deltaPhi(phi, jphi)*reco::deltaPhi(phi, jphi) + (eta-jeta)*(eta-jeta)) ;
        if (dr>0.0001) pt_dr_log += std::log(pt/dr);
        else pt_dr_log += std::log(pt/0.0001);
    }

    //Calculate axis2 and ptD
    float a = 0., b = 0., c = 0.;
    float ave_deta = 0., ave_dphi = 0., ave_deta2 = 0., ave_dphi2 = 0.;
    if(sum_weight > 0){
        ave_deta = sum_deta/sum_weight;
        ave_dphi = sum_dphi/sum_weight;
        ave_deta2 = sum_deta2/sum_weight;
        ave_dphi2 = sum_dphi2/sum_weight;
        a = ave_deta2 - ave_deta*ave_deta;                          
        b = ave_dphi2 - ave_dphi*ave_dphi;                          
        c = -(sum_detadphi/sum_weight - ave_deta*ave_dphi);                
    }
    float delta = sqrt(fabs((a-b)*(a-b)+4*c*c));
    float axis2 = (a+b-delta > 0 ?  sqrt(0.5*(a+b-delta)) : 0);
    float axis1 = (a+b+delta > 0 ?  sqrt(0.5*(a+b+delta)) : 0);
    float ptD   = (sum_weight > 0 ? sqrt(sum_weight)/sum_pt : 0);

    if (sum_pt>0 ) pt_dr_log = pt_dr_log / sum_pt;
    else pt_dr_log = -999;

    vars["ptD"] = ptD;
    vars["axis1"] = axis1;
    vars["axis2"] = axis2;
    vars["pt_dr_log"] = pt_dr_log;

    if(QGVARIABLES_DEBUG)cout<<"end calc variables: "<<ptD<<":"<<vars["mult"]<<endl;
    return vars;
}


/// Descriptions method
void QGVariables::fillDescriptions(edm::ConfigurationDescriptions& descriptions){
  //edm::ParameterSetDescription desc;
  //desc.add<edm::InputTag>("srcJets");
  //desc.add<edm::InputTag>("srcGenJets");
  //descriptions.add("QGVariables",  desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(QGVariables);
