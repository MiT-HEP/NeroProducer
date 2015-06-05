#include "NeroProducer/Core/interface/BareLeptons.hpp"

BareLeptons::BareLeptons(){
	p4 = NULL;
	pdgId = NULL;
	iso = NULL;
	tightId = NULL;
}

BareLeptons::~BareLeptons(){
}

void BareLeptons::clear(){
	p4 -> Clear();
	pdgId -> clear();
	iso -> clear();
	tightId ->clear();
}

void BareLeptons::defineBranches(TTree*t){
	//
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("lepP4","TClonesArray", &p4, 128000, 0);
	//
	pdgId = new vector<int>;
	t->Branch("lepPdgId","vector<int>",&pdgId);
	//
	iso = new vector<float>;
	t->Branch("lepIso","vector<float>",&iso);
	//
	tightId = new vector<int>;
	t->Branch("lepTightId","vector<int>",&tightId);
}

void BareLeptons::setBranchAddresses(TTree*t){

	p4 = new TClonesArray("TLorentzVector", 20);
	t->SetBranchAddress("lepP4"	, &p4	);
	t->SetBranchAddress("lepPdgId"	,&pdgId);
	t->SetBranchAddress("lepIso"	,&iso);
	t->SetBranchAddress("lepTightId"	,&tightId);
}

