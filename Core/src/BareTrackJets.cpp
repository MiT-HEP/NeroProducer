#include "NeroProducer/Core/interface/BareTrackJets.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareTrackJets::BareTrackJets(): BareP4(){
}

BareTrackJets::~BareTrackJets(){
}

void BareTrackJets::init(){
    BareP4::init();
}

void BareTrackJets::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
}

void BareTrackJets::defineBranches(TTree *t){
    BareP4::defineBranches(t, "trackjet");
}

void BareTrackJets::setBranchAddresses(TTree* t, std::string prefix)
{
    BareP4::setBranchAddresses(t,"trackjet");
}

BAREREGISTER(BareTrackJets);
