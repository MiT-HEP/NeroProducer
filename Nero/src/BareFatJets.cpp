#include "NeroProducer/Nero/interface/BareFatJets.hpp"


BareFatJets::BareFatJets(){
	p4 = NULL;
	rawPt = NULL;
	flavour = NULL;
	tau1 = NULL;
	tau2 = NULL;
	tau3 = NULL;
	trimmedMass = NULL;
	prunedMass = NULL;
	filteredMass = NULL;
	softdropMass = NULL;
}

BareFatJets::~BareFatJets(){
}

void BareFatJets::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	rawPt -> clear();
	flavour -> clear();
	tau1 -> clear();
	tau2 -> clear();
	tau3 -> clear();
	trimmedMass -> clear();
	prunedMass -> clear();
	filteredMass -> clear();
	softdropMass -> clear();
}

void BareFatJets::defineBranches(TTree *t){
	//
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("fatjetP4","TClonesArray", &p4, 128000, 0);
	//
	rawPt = new vector<float>;
	t->Branch("fatjetRawPt","vector<float>",&rawPt);
	// -- Jet Flavour by PAT
	flavour = new vector<int>;
	t->Branch("fatjetFlavour","vector<int>",&flavour);
	//
	tau1 = new vector<float>;
	t->Branch("fatjetTau1","vector<float>",&tau1);
	tau2 = new vector<float>;
	t->Branch("fatjetTau2","vector<float>",&tau2);
	tau3 = new vector<float>;
	t->Branch("fatjetTau3","vector<float>",&tau3);

	//
	trimmedMass = new vector<float>;
	t->Branch("fatjetTrimmedMass","vector<float>",&trimmedMass);
	prunedMass = new vector<float>;
	t->Branch("fatjetPrunedMass","vector<float>",&prunedMass);
	filteredMass = new vector<float>;
	t->Branch("fatjetFilteredMass","vector<float>",&filteredMass);
	softdropMass = new vector<float>;
	t->Branch("fatjetSoftdropMass","vector<float>",&softdropMass);
}
