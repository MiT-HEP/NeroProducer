#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroLeptons::NeroLeptons(): BareLeptons(){
	pv_ = NULL;
}

NeroLeptons::~NeroLeptons(){
}

int NeroLeptons::analyze(const edm::Event & iEvent)
{
	if (pv_ == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] PV not set."<<endl;

	iEvent.getByToken(mu_token,mu_handle);	
	iEvent.getByToken(el_token,el_handle);	

	iEvent.getByToken(el_mediumid_token,el_medium_id);
	iEvent.getByToken(el_tightid_token,el_tight_id);

	vector<myLepton> leptons;
		
	for (const pat::Muon &mu : *mu_handle) {
		// selection
		if (mu.pt() < 20 || !mu.isLooseMuon()) continue; 
		//value map
		//fill
                float chiso = mu.pfIsolationR04().sumChargedHadronPt;
                float niso = mu.pfIsolationR04().sumNeutralHadronEt;
                float phoiso = mu.pfIsolationR04().sumPhotonEt;
		float totiso = chiso + niso + phoiso;
		//new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(mu.px(),mu.py(),mu.pz(),mu.energy());
		//pdgId -> push_back(mu.charge()*13);
		//iso -> push_back(totiso);
		//tightId -> push_back( mu.isTightMuon(*pv_));
		myLepton l;
		l.pdgId = mu.charge()*13;
		l.iso = totiso;
		l.p4.SetPxPyPzE( mu.px(),mu.py(),mu.pz(),mu.energy());
		l.tightId = int(mu.isTightMuon(*pv_));
		leptons.push_back(l);
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
		
		//new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(el.px(),el.py(),el.pz(),el.energy());
		//pdgId -> push_back(el.charge()*11);
		//tightId -> push_back( int(isPassTight) );
		//iso ->push_back( -999);
		//
		myLepton l;
		l.pdgId = el.charge()*11;
		l.iso = -999.; //TODO
		l.p4.SetPxPyPzE( el.px(),el.py(),el.pz(),el.energy());
		l.tightId = int(isPassTight);
		leptons.push_back(l);
		
	}
	// sort leptons
	sort( leptons.begin(),leptons.end() , [](const myLepton &x, const myLepton&y){ return x.p4.Pt() < y.p4.Pt();} );

	// fill storage
	for(const auto &l : leptons)
	{
		new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(l.p4);
		iso -> push_back(l.iso);
		tightId -> push_back(l.tightId);
		pdgId -> push_back(l.pdgId);
	}
	return 0;
}



