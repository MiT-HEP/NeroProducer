// -*- C++ -*-
//
// Package:    NeroProducer/MuonFilter
// Class:      MuonFilter
// 
/**\class MuonFilter MuonFilter.cc NeroProducer/MuonFilter/plugins/MuonFilter.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Andrea Carlo Marini
// Wed Oct 31 16:09:47 CET 2018
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

class MuonFilter : public edm::EDFilter {
public:
    explicit MuonFilter(const edm::ParameterSet&);
    ~MuonFilter();
    
    static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
    virtual bool filter(edm::Event&, const edm::EventSetup&) override;
    bool isGoodMuon(const pat::Muon&);
    
    edm::EDGetTokenT<pat::MuonCollection> mu_token;
    edm::Handle<pat::MuonCollection> mu_handle;

    double min_pt;
    int    n_muons;
};

MuonFilter::MuonFilter(const edm::ParameterSet& iConfig):
    mu_token(  consumes<pat::MuonCollection>(iConfig.getParameter<edm::InputTag>("muons")) ),
    min_pt(   iConfig.getParameter<double>("minPt")),
    n_muons(   iConfig.getParameter<double>("nMuons"))
{
}


MuonFilter::~MuonFilter()
{
}

bool MuonFilter::isGoodMuon(const pat::Muon& muon){
    if (muon.pt()<min_pt) return false;
    if (not muon.isLooseMuon()) return false;
    //if (muon.pfIsolationR04().sumChargedHadronPt/muon.pt()>0.20) return false;
    return true;
}


bool
MuonFilter::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    iEvent.getByToken(mu_token, mu_handle);

    // loop over dilepton pairs
    int nmuons=0;
    for (unsigned int imuon=0; imuon+1<mu_handle->size(); imuon++){
        if (not isGoodMuon(mu_handle->at(imuon))) continue;
        if (mu_handle->at(imuon).pt()< min_pt) continue;
        nmuons+=1;
    }
    if (nmuons>= n_muons )return true;
    return false;
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
MuonFilter::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MuonFilter);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
