#include "NeroProducer/Nero/interface/NeroFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp" // JetId
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroFatJets::NeroFatJets() : BareFatJets(){
}

NeroFatJets::~NeroFatJets(){
}

int NeroFatJets::analyze(const edm::Event& iEvent){

	if ( mOnlyMc  ) return 0;

	// maybe handle should be taken before
	iEvent.getByToken(token, handle);

	int ijetRef = -1;
	for (const pat::Jet& j : *handle)
		{
		ijetRef++;
		if (j.pt() < 20 ) continue;

		// JET ID
		if ( !NeroJets::JetId(j) ) continue;
		
		// GET  ValueMaps
		
		// Fill output object	
		//p4 -> AddLast(new TLorentzVector(j.px(), j.py(), j.pz(), j.energy())  );
		new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());

		rawPt -> push_back (j.pt()*j.jecFactor("Uncorrected"));
        	flavour -> push_back( j.partonFlavour() );
		tau1 -> push_back(j.userFloat("NjettinessAK8:tau1"));
		tau2 -> push_back(j.userFloat("NjettinessAK8:tau2"));
		tau3 -> push_back(j.userFloat("NjettinessAK8:tau3"));

		trimmedMass->push_back(j.userFloat("ak8PFJetsCHSTrimmedMass"));
		prunedMass->push_back(j.userFloat("ak8PFJetsCHSPrunedMass"));
		filteredMass->push_back(j.userFloat("ak8PFJetsCHSFilteredMass"));
		softdropMass->push_back(j.userFloat("ak8PFJetsCHSSoftDropMass"));

		// how do I save these guys :
		// int nsubjet = 0;
		// auto Subjets = j.subjets("SoftDrop");
		// for ( auto const & sj : Subjets )
		// 	{
		// 	nsubjet++;
		// 	}
		
		}
	return 0;
}

