#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroLeptons::NeroLeptons(){
	pv_ = NULL;
}

NeroLeptons::~NeroLeptons(){
}

void NeroLeptons::clear(){

	p4 -> Clear();
	pdgId -> clear();
	iso ->clear();
	tightId ->clear();

}

void NeroLeptons::defineBranches(TTree*t){
	//
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("lepP4","TClonesArray", &p4, 128000, 0);
	//
	pdgId = new vector<float>;
	t->Branch("lepPdgId","vector<float>",&pdgId);
	//
	iso = new vector<float>;
	t->Branch("lepIso","vector<float>",&iso);
	//
	tightId = new vector<int>;
	t->Branch("lepTightId","vector<int>",&tightId);

}

int NeroLeptons::analyze(const edm::Event & iEvent)
{
	if (pv_ == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] PV not set."<<endl;

	iEvent.getByToken(mu_token,mu_handle);	
	iEvent.getByToken(el_token,el_handle);	

	iEvent.getByToken(el_mediumid_token,el_medium_id);
	iEvent.getByToken(el_tightid_token,el_tight_id);
		
	for (const pat::Muon &mu : *mu_handle) {
		// selection
		if (mu.pt() < 20 || !mu.isLooseMuon()) continue; 
		//value map
		//fill
		//p4 -> AddLast(new TLorentzVector(mu.px(),mu.py(),mu.pz(),mu.energy()));
		new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(mu.px(),mu.py(),mu.pz(),mu.energy());
		pdgId -> push_back(mu.charge()*13);
                float chiso = mu.pfIsolationR04().sumChargedHadronPt;
                float niso = mu.pfIsolationR04().sumNeutralHadronEt;
                float phoiso = mu.pfIsolationR04().sumPhotonEt;
		float totiso = chiso + niso + phoiso;
		iso -> push_back(totiso);
		tightId -> push_back( mu.isTightMuon(*pv_));
	}
	
	// Electron LOOP
	int iEle = -1;
	for (const pat::Electron &el : *el_handle)
	{
		iEle ++;
		if ( el.pt() <20 ) continue;
		if ( not el.passConversionVeto() ) continue;  // ve
		
		edm::RefToBase<pat::Electron> ref ( edm::Ref< pat::ElectronCollection >(el_handle, iEle) ) ;
		
		bool isPassMedium = (*el_medium_id)[ref];
		bool isPassTight = (*el_tight_id)[ref];
		
		if (not isPassMedium ) continue;
		
		
		//p4 -> AddLast(new TLorentzVector(el.px(),el.py(),el.pz(),el.energy()));
		new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(el.px(),el.py(),el.pz(),el.energy());
		pdgId -> push_back(el.charge()*11);
		tightId -> push_back( int(isPassTight) );
		
		iso ->push_back( -999);
	}
	return 0;
}



