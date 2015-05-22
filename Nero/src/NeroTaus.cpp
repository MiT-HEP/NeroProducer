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
	
	 	 //float trkIso = isolationTracksPtSum ();
		 //bool Tau::ExistIsolationCands 	( ) 	const
		 //reco::CandidatePtrVector Tau::isolationGammaCands () const
		 float phoIso = 0.; for(auto cand : tau.isolationGammaCands() ) phoIso += cand->pt();//tau.isolationPFGammaCandsEtSum() ;
		 float chIso  = 0.; for(auto cand : tau.isolationChargedHadrCands() ) chIso += cand->pt();//tau.isolationPFChargedHadrCandsPtSum();
		 float nhIso  = 0.; for(auto cand : tau.isolationNeutrHadrCands() ) nhIso += cand->pt(); // PF Cands not exists in miniAOD
		 float totIso = phoIso + chIso + nhIso;
		 //FILL
		 new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(tau.px(), tau.py(), tau.pz(), tau.energy());
		 id -> push_back( tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits"));
		 Q -> push_back( tau.charge() );
		 M -> push_back( tau.mass() );
		 iso -> push_back( totIso ) ; 

	}
	return 0;
}


