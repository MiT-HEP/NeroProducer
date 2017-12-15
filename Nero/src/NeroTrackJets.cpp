#include "NeroProducer/Nero/interface/NeroTrackJets.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroTrackJets::NeroTrackJets(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
    BareTrackJets()
{

    token = cc.consumes< std::vector<reco::PFJet> >(iConfig.getParameter<edm::InputTag>("jets"));
    mMinPt = iConfig.getParameter<double>("minPt");
}

NeroTrackJets::~NeroTrackJets(){
}

int NeroTrackJets::analyze(const edm::Event& iEvent, const edm::EventSetup &iSetup){

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    if ( not handle.isValid() ) cout<<"[NeroTrackJets]::[analyze]::[ERROR] handle is not valid"<<endl;
    for (const auto& j : *handle)
    {
        if (j.pt() < mMinPt ) continue;
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
    }
    return 0;
}

NEROREGISTER(NeroTrackJets);
