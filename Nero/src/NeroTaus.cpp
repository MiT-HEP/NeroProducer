#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroTaus::NeroTaus(){
}

NeroTaus::~NeroTaus(){
}

void NeroTaus::clear(){
	p4->Clear();
	id->clear();
	Q->clear();
	M->clear();
	iso -> clear();
}


void NeroTaus::defineBranches(TTree *t){
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

int NeroTaus::analyze(const edm::Event & iEvent)
{
	iEvent.getByToken(token, handle);
	 for (const pat::Tau &tau : *handle) {
		 if (tau.pt() <20 ) continue;	
		
		 //FILL
		 new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(tau.px(), tau.py(), tau.pz(), tau.energy());
		 id -> push_back( tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits"));
		 Q -> push_back( tau.charge() );
		 M -> push_back( tau.mass() );
		 iso -> push_back( -999 ) ; //TODO

	}
	return 0;
}


