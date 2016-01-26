// -*- C++ -*-
//
// Package:    NeroProducer/MonoJetFilter
// Class:      MonoJetFilter
// 
/**\class MonoJetFilter MonoJetFilter.cc NeroProducer/MonoJetFilter/plugins/MonoJetFilter.cc

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

class MonoJetFilter : public edm::EDFilter {
public:
    explicit MonoJetFilter(const edm::ParameterSet&);
    ~MonoJetFilter();
    
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    virtual bool filter(edm::Event&, const edm::EventSetup&) override;
    bool isGoodMuon(const pat::Muon&);
    bool isGoodElectron(const pat::Electron&);
    bool isGoodPhoton(const pat::Photon&);
    
    edm::EDGetTokenT<pat::METCollection> met_token ;	
    edm::Handle<pat::METCollection> met_handle;

    edm::EDGetTokenT<pat::MuonCollection> mu_token;
    edm::Handle<pat::MuonCollection> mu_handle;

    edm::EDGetTokenT<pat::ElectronCollection> el_token;
    edm::Handle<pat::ElectronCollection> el_handle;

    edm::EDGetTokenT<pat::PhotonCollection> ph_token;
    edm::Handle<pat::PhotonCollection> ph_handle;

    double min_met;
    bool keep_wjets;
    bool keep_zll;
    bool keep_photons;
};

MonoJetFilter::MonoJetFilter(const edm::ParameterSet& iConfig):
    met_token( consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("met")) ),
    mu_token(  consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons")) ),
    el_token(  consumes<pat::ElectronCollection>(iConfig.getParameter<edm::InputTag>("electrons")) ),
    ph_token(  consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons")) ),
    min_met(   iConfig.getParameter<double>("minMet")),
    keep_wjets( iConfig.getParameter<bool>("keepWjets")),
    keep_zll(  iConfig.getParameter<bool>("keepZll")),
    keep_photons( iConfig.getParameter<bool>("keepPhotons"))
{
}


MonoJetFilter::~MonoJetFilter()
{
}

bool MonoJetFilter::isGoodMuon(const pat::Muon& muon){
    if (muon.pt()<20) return false;
    if (not muon.isLooseMuon()) return false;
    if (muon.pfIsolationR04().sumChargedHadronPt/muon.pt()>0.12) return false;
    return true;
}

bool MonoJetFilter::isGoodElectron(const pat::Electron& ele){
    if (ele.pt()<20) return false;
    if (ele.chargedHadronIso()/ele.pt()>0.1) return false;
    bool isEB = ele.isEB();
    bool isEE = ele.isEE();
    if (isEB and ele.full5x5_sigmaIetaIeta()>0.011) return false;
    if (isEE and ele.full5x5_sigmaIetaIeta()>0.030) return false;
    return true;
}

bool MonoJetFilter::isGoodPhoton(const pat::Photon& photon){
    if (photon.chargedHadronIso()>10) return false;
    if (not photon.isEB()) return false;
    if (photon.full5x5_sigmaIetaIeta()>0.020) return false;
    return true;
}


bool
MonoJetFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(met_token, met_handle);
    iEvent.getByToken(mu_token, mu_handle);
    iEvent.getByToken(el_token, el_handle);
    iEvent.getByToken(ph_token, ph_handle);

    for (pat::METCollection::const_iterator met = met_handle->begin(); met!=met_handle->end(); met++){
        
        // if MET is big enough keep the event
        if (met->pt()>min_met) return true;
        
        if (keep_wjets){
            // loop over leptons to get W+jets events
            for (pat::MuonCollection::const_iterator muon = mu_handle->begin(); muon!=mu_handle->end(); muon++){
                if (not isGoodMuon(*muon)) continue;
                if ((met->p4()+muon->p4()).pt()>min_met) return true;
            }
            for (pat::ElectronCollection::const_iterator ele = el_handle->begin(); ele!=el_handle->end(); ele++){
                if (not isGoodElectron(*ele)) continue;
                if ((met->p4()+ele->p4()).pt()>min_met) return true;
            }
        }
    }

    if (keep_zll){
        // loop over dilepton pairs
        for (unsigned int imuon=0; imuon+1<mu_handle->size(); imuon++){
            if (not isGoodMuon(mu_handle->at(imuon))) continue;
            for (unsigned int jmuon=imuon+1; jmuon<mu_handle->size(); jmuon++){
                if (not isGoodMuon(mu_handle->at(jmuon))) continue;
                if ((mu_handle->at(imuon).p4()+mu_handle->at(jmuon).p4()).pt()>min_met) return true;
            }           
        }
        for (unsigned int iele=0; iele+1<el_handle->size(); iele++){
            if (not isGoodElectron(el_handle->at(iele))) continue;
            for (unsigned int jele=iele+1; jele<el_handle->size(); jele++){
                if (not isGoodElectron(el_handle->at(jele))) continue;
                if ((el_handle->at(iele).p4()+el_handle->at(jele).p4()).pt()>min_met) return true;
            }           
        }
    }
        
    if (keep_photons){
        for (pat::PhotonCollection::const_iterator photon = ph_handle->begin(); photon!=ph_handle->end(); photon++){
            if (not isGoodPhoton(*photon)) continue;
            if (photon->pt()>min_met) return true;
        }
    }
    return false;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MonoJetFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MonoJetFilter);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
