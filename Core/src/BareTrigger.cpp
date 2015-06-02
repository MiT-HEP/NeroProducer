#include "NeroProducer/Core/interface/BareTrigger.hpp"

BareTrigger::BareTrigger(){
	triggerFired=NULL;
	triggerNames = new vector<string>;
}
BareTrigger::~BareTrigger(){}


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
	triggerFired =new vector<int>;
	t->Branch("triggerFired","vector<int>",&triggerFired);
	triggerPrescale =new vector<float>;
	t->Branch("triggerPrescale","vector<float>",&triggerPrescale);
	// ---
	triggerLeps =new vector<int>;
	t->Branch("triggerLeps","vector<int>",&triggerLeps);
	triggerJets =new vector<int>;
	t->Branch("triggerJets","vector<int>",&triggerLeps);
	triggerTaus =new vector<int>;
	t->Branch("triggerTaus","vector<int>",&triggerTaus);
	triggerPhotons =new vector<int>;
	t->Branch("triggerPhotons","vector<int>",&triggerPhotons);
}

void BareTrigger::setBranchAddresses(TTree*t)
{
	t -> SetBranchAddress("triggerFired", &triggerFired);
	t -> SetBranchAddress("triggerPrescale", &triggerPrescale);
	// ---
	t -> SetBranchAddress("triggerLeps", &triggerLeps);
	t -> SetBranchAddress("triggerJets", &triggerJets);
	t -> SetBranchAddress("triggerTaus", &triggerTaus);
	t -> SetBranchAddress("triggerPhotons", &triggerPhotons);
}

#ifdef TREE_STREAMER
ClassImp(BareTrigger);
#endif
