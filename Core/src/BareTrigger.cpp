#include "NeroProducer/Core/interface/BareTrigger.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareTrigger::BareTrigger():
    BareCollection(),
    triggerNames(new vector<string>)
{
}

BareTrigger::~BareTrigger(){
    BareFunctions::Delete(triggerNames);

    BareFunctions::Delete(triggerFired);
    BareFunctions::Delete(triggerPrescale);
    BareFunctions::Delete(triggerLeps);
    BareFunctions::Delete(triggerJets);
    BareFunctions::Delete(triggerTaus);
    BareFunctions::Delete(triggerPhotons);

    BareFunctions::Delete(triggerNoneTaus);
}

void BareTrigger::init(){
    BareFunctions::New(triggerFired);
    BareFunctions::New(triggerPrescale);
    BareFunctions::New(triggerLeps);
    BareFunctions::New(triggerJets);
    BareFunctions::New(triggerTaus);
    BareFunctions::New(triggerPhotons);

    BareFunctions::New(triggerNoneTaus);
}

void BareTrigger::clear(){

    triggerFired -> clear();
    triggerPrescale -> clear();
    //---
    triggerLeps -> clear();
    triggerJets -> clear();
    triggerTaus -> clear();
    triggerPhotons -> clear();
    // ---
    triggerNoneTaus -> clear();
}

void BareTrigger::defineBranches(TTree *t){
    init();

    t->Branch("triggerFired","vector<int>",&triggerFired);
    t->Branch("triggerPrescale","vector<float>",&triggerPrescale);
    // ---
    t->Branch("triggerLeps","vector<ULong64_t>",&triggerLeps);
    t->Branch("triggerJets","vector<ULong64_t>",&triggerJets);
    t->Branch("triggerTaus","vector<ULong64_t>",&triggerTaus);
    t->Branch("triggerPhotons","vector<ULong64_t>",&triggerPhotons);
    // ---
    t->Branch("triggerNoneTaus","vector<ULong64_t>",&triggerNoneTaus);
}

void BareTrigger::setBranchAddresses(TTree*t)
{
    init();

    BareFunctions::SetBranchAddress(t,"triggerFired", &triggerFired);
    BareFunctions::SetBranchAddress(t,"triggerPrescale", &triggerPrescale);

    BareFunctions::SetBranchAddress(t,"triggerLeps", &triggerLeps);
    BareFunctions::SetBranchAddress(t,"triggerJets", &triggerJets);
    BareFunctions::SetBranchAddress(t,"triggerTaus", &triggerTaus);
    BareFunctions::SetBranchAddress(t,"triggerPhotons", &triggerPhotons);
    BareFunctions::SetBranchAddress(t,"triggerNoneTaus", &triggerNoneTaus);
}

BAREREGISTER(BareTrigger);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
