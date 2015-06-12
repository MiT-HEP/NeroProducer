#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

// -- Electron Isolation
NeroLeptons::NeroLeptons(): BareLeptons(){
	vtx_ = NULL;
	mMinPt= 20;
	mMinNleptons = 0;
}

NeroLeptons::~NeroLeptons(){
}

int NeroLeptons::analyze(const edm::Event & iEvent)
{
	if ( mOnlyMc  ) return 0;

	if ( vtx_ == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Vertex Class not set."<<endl;
	if ( vtx_ -> GetPV() == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Primary Vertex not set."<<endl;

	iEvent.getByToken(mu_token,mu_handle);	
	iEvent.getByToken(el_token,el_handle);	

	iEvent.getByToken(el_mediumid_token,el_medium_id);
	iEvent.getByToken(el_tightid_token,el_tight_id);
	iEvent.getByToken(el_vetoid_token,el_veto_id);

	vector<myLepton> leptons;
	
	for (const pat::Muon &mu : *mu_handle) {
		// selection
		if (mu.pt() < 10 || !mu.isLooseMuon()) continue; 
		if (mu.pt() < mMinPt ) continue;
		if (fabs(mu.eta()) > 2.4 ) continue;  // TODO Make me configurable
		//value map
		//fill
        
        //Muon Corrected MET add back: muon.pfP4().pt() 
    


        float chiso  = mu.pfIsolationR04().sumChargedHadronPt;
        float niso   = mu.pfIsolationR04().sumNeutralHadronEt;
        float phoiso = mu.pfIsolationR04().sumPhotonEt;
		float totiso = chiso + niso + phoiso;

        if ( totiso/mu.pt() > 0.2 ) continue;

		myLepton l;
		l.pdgId = mu.charge()*13;
		l.iso = totiso;
		l.p4.SetPxPyPzE( mu.px(),mu.py(),mu.pz(),mu.energy());
		l.tightId = int(mu.isTightMuon( * vtx_->GetPV() ));
        	l.pfPt = mu.pfP4().pt();
		leptons.push_back(l);
	}
	
	// Electron LOOP
	int iEle = -1;
	for (const pat::Electron &el : *el_handle)
	{
		iEle ++;
		if ( el.pt() <10 ) continue;
		if ( el.pt() < mMinPt ) continue;
		if ( fabs(el.eta()) > 2.5 ) continue; // TODO make me config
		if ( not el.passConversionVeto() ) continue;  // ve
		
		edm::RefToBase<pat::Electron> ref ( edm::Ref< pat::ElectronCollection >(el_handle, iEle) ) ;
	
		bool isPassVeto = (*el_veto_id)[ref];
		bool isPassTight = (*el_tight_id)[ref];
		
		//bool isPassLoose = (*el_loose_id)[ref];
		//bool isPassMedium = (*el_medium_id)[ref];
		//
		
		if (not isPassVeto ) continue;
		
		myLepton l;
		l.pdgId = el.charge()*11;
		//l.iso = el.ecalPFClusterIso() + el.hcalPFClusterIso(); //not working, use GEDIdTools or ValueMap
	      	float chIso = el.chargedHadronIso();
	      	float nhIso = el.neutralHadronIso();
	      	float phoIso = el.photonIso();
	      	//float puChIso= el.puChargedHadronIso();
		l.iso = chIso + nhIso + phoIso; 
		l.p4.SetPxPyPzE( el.px(),el.py(),el.pz(),el.energy());
		l.tightId = int(isPassTight);
        	l.pfPt = 0.;
		leptons.push_back(l);
		
	}

	if ( int(leptons.size()) < mMinNleptons ) return 1;
	// sort leptons
	sort( leptons.begin(),leptons.end() , [](const myLepton &x, const myLepton&y){ return x.p4.Pt() > y.p4.Pt();} );

	// fill storage
	for(const auto &l : leptons)
	{
		new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(l.p4);
		iso     -> push_back(l.iso);
		tightId -> push_back(l.tightId);
		pdgId   -> push_back(l.pdgId);
       	 	lepPfPt -> push_back(l.pfPt);
	}
	return 0;
}



