#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroMet::NeroMet() : BareMet() {
}

NeroMet::~NeroMet(){
}


int NeroMet::analyze(const edm::Event& iEvent){

	if ( mOnlyMc  ) return 0; // in principle I would like to have the gen met: TODO

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

