#include "NeroProducer/Core/interface/BareTrigger.hpp"

BareTrigger::BareTrigger():
    BareCollection(),
    triggerNames(new vector<string>)
{
}

BareTrigger::~BareTrigger(){
    delete triggerNames;

    delete triggerFired;
    delete triggerPrescale;
    delete triggerLeps;
    delete triggerJets;
    delete triggerTaus;
    delete triggerPhotons;
}

void BareTrigger::init(){
    if (!triggerFired)
        triggerFired =new vector<int>;
    if (!triggerPrescale)
        triggerPrescale =new vector<float>;
    // ---
    if (!triggerLeps)
        triggerLeps =new vector<int>;
    if (!triggerJets)
        triggerJets =new vector<int>;
    if (!triggerTaus)
        triggerTaus =new vector<int>;
    if (!triggerPhotons)
        triggerPhotons =new vector<int>;
}

void BareTrigger::clear(){

    triggerFired -> clear();
    triggerPrescale -> clear();
    //---
    triggerLeps -> clear();
    triggerJets -> clear();
    triggerTaus -> clear();
    triggerPhotons -> clear();
}

void BareTrigger::defineBranches(TTree *t){
    init();

    t->Branch("triggerFired","vector<int>",&triggerFired);
    t->Branch("triggerPrescale","vector<float>",&triggerPrescale);
    // ---
    t->Branch("triggerLeps","vector<int>",&triggerLeps);
    t->Branch("triggerJets","vector<int>",&triggerLeps);
    t->Branch("triggerTaus","vector<int>",&triggerTaus);
    t->Branch("triggerPhotons","vector<int>",&triggerPhotons);
}

void BareTrigger::setBranchAddresses(TTree*t)
{
    init();

    if (t->GetBranchStatus("triggerFired"))
        t -> SetBranchAddress("triggerFired", &triggerFired);
    if (t->GetBranchStatus("triggerPrescale"))
        t -> SetBranchAddress("triggerPrescale", &triggerPrescale);
    // ---
    if (t->GetBranchStatus("triggerLeps"))
        t -> SetBranchAddress("triggerLeps", &triggerLeps);
    if (t->GetBranchStatus("triggerJets"))
        t -> SetBranchAddress("triggerJets", &triggerJets);
    if (t->GetBranchStatus("triggerTaus"))
        t -> SetBranchAddress("triggerTaus", &triggerTaus);
    if (t->GetBranchStatus("triggerPhotons"))
        t -> SetBranchAddress("triggerPhotons", &triggerPhotons);
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
