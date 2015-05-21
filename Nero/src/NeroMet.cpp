#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroMet::NeroMet(){
	p4 = NULL;
	ptJESUP = NULL;
	ptJESDOWN = NULL;
	genP4 = NULL;
}

NeroMet::~NeroMet(){
}

void NeroMet::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	ptJESUP -> clear();
	ptJESDOWN -> clear();
	genP4 -> Clear();
}

void NeroMet::defineBranches(TTree *t){
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

int NeroMet::analyze(const edm::Event& iEvent){

	// maybe handle should be taken before
	iEvent.getByToken(token, handle);
	const pat::MET &met = handle->front();

	//p4 -> AddLast(new TLorentzVector(met.px(),met.py(),met.pz(),met.energy()));
	new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( met.px(),met.py(),met.pz(),met.energy()  );
	ptJESUP -> push_back( met.shiftedPt(pat::MET::JetEnUp) );
	ptJESDOWN -> push_back( met.shiftedPt(pat::MET::JetEnDown) );

	new ( (*genP4)[genP4->GetEntriesFast()]) TLorentzVector( met.genMET()->px(),met.genMET()->py(),met.genMET()->pz(),met.genMET()->energy()  );

	return 0;
}

