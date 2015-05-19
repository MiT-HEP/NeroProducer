#include "NeroProducer/Nero/interface/NeroFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp" // JetId
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroFatJets::NeroFatJets(){
	p4 = NULL;
	rawPt = NULL;
	flavour = NULL;
}

NeroFatJets::~NeroFatJets(){
}

void NeroFatJets::clear(){
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

void NeroFatJets::defineBranches(TTree *t){
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

int NeroFatJets::analyze(const edm::Event& iEvent){

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
		new ( (*p4)[p4->GetSize()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());

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

