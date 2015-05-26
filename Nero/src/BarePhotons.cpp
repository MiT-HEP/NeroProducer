#include "NeroProducer/Nero/interface/BarePhotons.hpp"


BarePhotons::BarePhotons(){
	p4 = NULL;
	sieie = NULL;
	iso = NULL;
}

BarePhotons::~BarePhotons(){}

void BarePhotons::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	iso -> clear();
	sieie -> clear();
}

void BarePhotons::defineBranches(TTree *t){
	//
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("photonP4","TClonesArray", &p4, 128000, 0);
	//
	iso = new vector<float>;
	t->Branch("photonIso","vector<float>",&iso);
	//
	sieie = new vector<float>;
	t->Branch("photonSieie","vector<float>",&sieie);
}

void BarePhotons::setBranchAddresses(TTree *t){
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("photonP4"	,&p4);
	t->Branch("photonIso"	,&iso);
	t->Branch("photonSieie"	,&sieie);
}
