#include "NeroProducer/Nero/interface/BareTaus.hpp"

BareTaus::BareTaus(){
	p4 = NULL;
	id = NULL;
	Q = NULL;
	M = NULL;
	iso = NULL;
}

BareTaus::~BareTaus(){
}

void BareTaus::clear(){
	p4->Clear();
	id->clear();
	Q->clear();
	M->clear();
	iso -> clear();
}

void BareTaus::defineBranches(TTree *t){
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("tauP4","TClonesArray", &p4, 128000, 0);
	//
	id = new vector<float>;
	t->Branch("tauId","vector<float>",&id);
	//
	Q = new vector<int>;
	t->Branch("tauQ","vector<int>",&Q);
	//
	M = new vector<float>;
	t->Branch("tauM","vector<float>",&M);
	//
	iso = new vector<float>;
	t->Branch("tauIso","vector<float>",&iso);
	
}

void BareTaus::setBranchAddresses(TTree *t){
	p4 = new TClonesArray("TLorentzVector", 20);
	t->SetBranchAddress("tauP4"	,&p4);
	t->SetBranchAddress("tauId"	,&id);
	t->SetBranchAddress("tauQ"	,&Q);
	t->SetBranchAddress("tauM"	,&M);
	t->SetBranchAddress("tauIso"	,&iso);
}
