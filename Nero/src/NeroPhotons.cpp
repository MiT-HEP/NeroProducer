#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroPhotons::NeroPhotons(){
	p4 = NULL;
	sieie = NULL;
	iso = NULL;
}

NeroPhotons::~NeroPhotons(){
}

void NeroPhotons::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	iso -> clear();
	sieie -> clear();
}

void NeroPhotons::defineBranches(TTree *t){
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

int NeroPhotons::analyze(const edm::Event& iEvent){

	// maybe handle should be taken before
	iEvent.getByToken(token, handle);

	// ID and ISO
	iEvent.getByToken(medium_id_token,medium_id);
	iEvent.getByToken(iso_ch_token, iso_ch);
	iEvent.getByToken(iso_nh_token, iso_nh);
	iEvent.getByToken(iso_pho_token, iso_pho);

	int iPho = -1;	
	for (auto &pho : *handle)
	{
	++iPho;
		if (pho.pt() <20 or pho.chargedHadronIso()/pho.pt() > 0.3) continue;		
		edm::RefToBase<pat::Photon> ref ( edm::Ref< pat::PhotonCollection >(handle, iPho) ) ;
		float chIso =  (*iso_ch) [ref];
        	float nhIso =  (*iso_nh) [ref];
        	float phIso =  (*iso_pho)[ref];	
		float totIso = chIso + nhIso + phIso;
        	bool isPassMedium = (*medium_id)[ref];	

		if (not isPassMedium) continue;
		
		//FILL
		new ( (*p4)[p4->GetSize()]) TLorentzVector(pho.px(),pho.py(),pho.pz(),pho.energy());
		iso->push_back(totIso);	
		sieie -> push_back( pho. full5x5_sigmaIetaIeta() );
		
	}

	return 0;
}

