#include "NeroProducer/Core/interface/BareTrigger.hpp"

BareTrigger::BareTrigger(){
	triggerFired=NULL;
	triggerNames = new vector<string>;
}
BareTrigger::~BareTrigger(){}


void BareTrigger::clear(){

	triggerFired -> clear();
	triggerPrescale -> clear();
}

void BareTrigger::defineBranches(TTree *t){
	triggerFired =new vector<int>;
	t->Branch("triggerFired","vector<int>",&triggerFired);
	triggerPrescale =new vector<float>;
	t->Branch("triggerPrescale","vector<float>",&triggerPrescale);
}

void BareTrigger::setBranchAddresses(TTree*t)
{
	t -> SetBranchAddress("triggerFired", &triggerFired);
	t -> SetBranchAddress("triggerPrescale", &triggerPrescale);
}
