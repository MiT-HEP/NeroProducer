#include "NeroProducer/Core/interface/BareMet.hpp"


BareMet::BareMet(){
	p4 = NULL;
	ptJESUP = NULL;
	ptJESDOWN = NULL;
	genP4 = NULL;
}

BareMet::~BareMet(){
}

void BareMet::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	ptJESUP -> clear();
	ptJESDOWN -> clear();
	genP4 -> Clear();
}

void BareMet::defineBranches(TTree *t){
	//
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("metP4","TClonesArray", &p4, 128000, 0);
	//
	ptJESUP = new vector<float>;
	t->Branch("metPtJESUP","vector<float>",&ptJESUP);
	//
	ptJESDOWN = new vector<float>;
	t->Branch("metPtJESDOWN","vector<float>",&ptJESDOWN);
	//	
	genP4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("metP4_GEN","TClonesArray", &p4, 128000, 0);
}

void BareMet::setBranchAddresses(TTree *t){
	p4 = new TClonesArray("TLorentzVector", 20);
	t->SetBranchAddress("metP4"	, &p4	);
	t->SetBranchAddress("metPtJESUP"	,&ptJESUP);
	t->SetBranchAddress("metPtJESDOWN",&ptJESDOWN);
	genP4 = new TClonesArray("TLorentzVector", 20);
	t->SetBranchAddress("metP4_GEN"	, &genP4 );
}

