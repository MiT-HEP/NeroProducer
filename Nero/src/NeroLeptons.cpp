#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

// -- Electron Isolation
NeroLeptons::NeroLeptons(): 
    NeroCollection(),
    BareLeptons()
{
    vtx_   = NULL;
    evt_   = NULL;

    mMinPt_mu = 10;
    mMinEta_mu = 2.4;
    mMaxIso_mu = 0.2;

    mMinPt_el = 10;
    mMinEta_el = 2.5;
    mMaxIso_el = -1.;

    mMinNleptons = 0;    

}

NeroLeptons::~NeroLeptons(){
}

int NeroLeptons::analyze(const edm::Event & iEvent)
{
    if ( mOnlyMc  ) return 0;

    if ( vtx_ == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Vertex Class not set."<<endl;
    if ( vtx_ -> GetPV() == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Primary Vertex not set."<<endl;
    if ( evt_ == NULL) cout<<"[NeroLeptons]::[analyze]::[WARNING] Event Class not set."<<endl;

    iEvent.getByToken(mu_token,mu_handle);	
    iEvent.getByToken(el_token,el_handle);	

    iEvent.getByToken(el_mediumid_token,el_medium_id);
    iEvent.getByToken(el_tightid_token,el_tight_id);
    iEvent.getByToken(el_vetoid_token,el_veto_id);
    iEvent.getByToken(el_looseid_token,el_loose_id);

    if ( not mu_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] mu_handle is not valid"<<endl;
    if ( not el_handle.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_handle is not valid"<<endl;
    if ( not el_medium_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_medium_id is not valid"<<endl;
    if ( not el_tight_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_tight_id is not valid"<<endl;
    if ( not el_veto_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_veto_id is not valid"<<endl;
    if ( not el_loose_id.isValid() ) cout<<"[NeroLeptons]::[analyze]::[ERROR] el_loose_id is not valid"<<endl;

    vector<myLepton> leptons;

    for (const pat::Muon &mu : *mu_handle) {
        // selection
        if (mu.pt() < 10. ) continue;
        if (mu.pt() < mMinPt_mu || fabs(mu.eta()) > mMinEta_mu || !mu.isLooseMuon()) continue; 
        float chiso  = mu.pfIsolationR04().sumChargedHadronPt;
        float niso   = mu.pfIsolationR04().sumNeutralHadronEt;
        float phoiso = mu.pfIsolationR04().sumPhotonEt;
        float puiso = mu.pfIsolationR04().sumPUPt;
        float totiso = chiso + TMath::Max( niso + phoiso - .5*puiso, 0. ) ;

        if ( totiso/mu.pt() > mMaxIso_mu ) continue;

        myLepton l;
        l.pdgId = -mu.charge()*13;
        l.iso = totiso;
        l.p4.SetPxPyPzE( mu.px(),mu.py(),mu.pz(),mu.energy());
        l.selBits =  0 ;
        l.selBits |= unsigned(mu.isLooseMuon()) * LepLoose;
        l.selBits |= unsigned(mu.isTightMuon( * vtx_->GetPV() ))*LepTight ;
        l.selBits |= unsigned(mu.isMediumMuon() * LepMedium);
        l.pfPt = mu.pfP4().pt();

        l.chiso  = chiso;
        l.nhiso  = niso;
        l.phoiso = phoiso;
        l.puiso  = puiso;

        leptons.push_back(l);
    }

    // Electron LOOP
    int iEle = -1;
    for (const pat::Electron &el : *el_handle)
    {
        iEle ++;

        if ( el.pt() < 10 ) continue;
        if ( el.pt() < mMinPt_el || fabs(el.eta()) > mMinEta_el ) continue;
        if ( not el.passConversionVeto() ) continue;  // ve

        edm::RefToBase<pat::Electron> ref ( edm::Ref< pat::ElectronCollection >(el_handle, iEle) ) ;

        bool isPassVeto = (*el_veto_id)[ref];
        bool isPassTight = (*el_tight_id)[ref];
        bool isPassMedium = (*el_medium_id)[ref];
        bool isPassLoose = (*el_loose_id)[ref];

        if (not isPassVeto ) continue;

        myLepton l;
        l.pdgId = -el.charge()*11;


        // float chIso = el.chargedHadronIso();
        // float nhIso = el.neutralHadronIso();
        // float phoIso = el.photonIso();
        float chIso = el.pfIsolationVariables().sumChargedHadronPt;
        float nhIso = el.pfIsolationVariables().sumNeutralHadronEt;
        float phoIso = el.pfIsolationVariables().sumPhotonEt;

        float puChIso= el.puChargedHadronIso();

        bool isEB = el.isEB();
        bool isEE = el.isEE();
        bool isEBEEGap  = el.isEBEEGap(); // for EB EE
        bool isEBEtaGap = el.isEBEtaGap(); //for EB
        bool isEBPhiGap = el.isEBPhiGap (); //for EB
        bool isEERingGap = el.isEERingGap(); //for EE
        bool isEEDeeGap = el.isEEDeeGap ();//for EE

        //l.iso = chIso + nhIso + phoIso; 
        // effective area from
        // https://indico.cern.ch/event/369239/contribution/4/attachments/1134761/1623262/talk_effective_areas_25ns.pdf
        double ea = 0.;
        if ( fabs(el.eta() ) < 1.0 ) ea= 0.1752 ; 
        else if (fabs(el.eta() ) < 1.479 ) ea = 0.1862 ;
        else if (fabs(el.eta() ) < 2.0 ) ea = 0.1411 ;
        else if (fabs(el.eta() ) < 2.2 ) ea = 0.1534 ;
        else if (fabs(el.eta() ) < 2.3 ) ea = 0.1903 ;
        else if (fabs(el.eta() ) < 2.4 ) ea = 0.2243 ;
        else if (fabs(el.eta() ) < 2.5 ) ea = 0.2687 ;
        l.iso = chIso + TMath::Max( nhIso + phoIso - evt_->rho * ea , 0. ) ; 

        l.p4.SetPxPyPzE( el.px(),el.py(),el.pz(),el.energy());
        l.selBits = 0 ;
        l.selBits |= unsigned(isPassTight)*LepTight;
        l.selBits |= unsigned(isPassMedium) * LepMedium;
        l.selBits |= unsigned(isPassVeto) * LepVeto;
        l.selBits |= unsigned(isPassLoose) * LepLoose;
        //--
        l.selBits |= unsigned(isEB and (not isEBEEGap and not isEBEtaGap and not isEBPhiGap)  ) * LepEBEE;
        l.selBits |= unsigned(isEE and (not isEBEEGap and not isEERingGap and not isEEDeeGap)  ) * LepEBEE;
        l.pfPt = 0.;

        l.chiso  = chIso;
        l.nhiso  = nhIso;
        l.phoiso = phoIso;
        l.puiso  = puChIso;

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
        selBits -> push_back(l.selBits);
        pdgId   -> push_back(l.pdgId);
        lepPfPt -> push_back(l.pfPt);

        chIso	-> push_back(l.chiso);
        nhIso	-> push_back(l.nhiso);
        phoIso	-> push_back(l.phoiso);
        puIso	-> push_back(l.puiso);
    }
    return 0;
}



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
