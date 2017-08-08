#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroMet::NeroMet(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
    BareMet() 
{
    token_pf = cc.consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("pfCands"));

    token = cc.consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("mets"));
    token_puppi = cc.consumes<pat::METCollection>(iConfig.getParameter<edm::InputTag>("metsPuppi"));
    SetExtend (iConfig.getParameter<bool>("extendMet"));
}

NeroMet::~NeroMet(){
}



int NeroMet::analyze(const edm::Event& iEvent){
    // --- Handle
    edm::Handle<pat::METCollection> handle;	
    edm::Handle<pat::METCollection> handle_puppi;
    edm::Handle<pat::METCollection> handle_cleanmu;
    edm::Handle<pat::METCollection> handle_cleaneg;
    edm::Handle<pat::METCollection> handle_unclean;
    edm::Handle<pat::PackedCandidateCollection> handle_pf;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    iEvent.getByToken(token_pf, handle_pf);
    if ( not handle.isValid() ) cout<<"[NeroMet]::[analyze]::[ERROR] handle is not valid"<<endl;

    iEvent.getByToken(token_puppi,handle_puppi);
    if ( not handle_puppi.isValid() ) cout<<"[NeroMet]::[analyze]::[ERROR] handle_puppi is not valid"<<endl;

    //iEvent.getByToken(token_cleanmu,handle_cleanmu);
    //if ( not handle_cleanmu.isValid() ) cout<<"[NeroMet]::[analyze]::[ERROR] handle_cleanmu is not valid"<<endl;



    const pat::MET &met = handle->front();

    TLorentzVector caloMet(0,0,0,0);
    caloMet.SetPtEtaPhiE(met.caloMETPt(),0,met.caloMETPhi(),met.caloMETPt());
    *CaloMet = TLorentzVector(caloMet);
    
    TLorentzVector rawMet(0,0,0,0);
    rawMet.SetPtEtaPhiE(met.uncorPt(),0,met.uncorPhi(),met.uncorPt());
    *RawMet = TLorentzVector(rawMet);

    // FILL PFMET
    new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( met.px(),met.py(),met.pz(),met.energy()  );

    // FILL Puppi MET
    auto &puppi = handle_puppi->front(); 
    *metPuppi =  TLorentzVector( puppi.px(), puppi.py(),puppi.pz(),puppi.energy() );

    // saving only Jes, Jer, Uncluster
    for(Syst mysyst = (Syst)0; mysyst < TauUp and mysyst < MaxSyst  ; mysyst = (Syst)((int)mysyst +1 ) )
    {
        pat::MET::METUncertainty miniAODUnc=pat::MET::METUncertaintySize;
        // JetResUp=0, JetResDown=1, JetEnUp=2, JetEnDown=3,
        // MuonEnUp=4, MuonEnDown=5, ElectronEnUp=6, ElectronEnDown=7,
        // TauEnUp=8, TauEnDown=9, UnclusteredEnUp=10, UnclusteredEnDown=11,
        // PhotonEnUp=12, PhotonEnDown=13, NoShift=14, METUncertaintySize=15,
        // JetResUpSmear=16, JetResDownSmear=17, METFullUncertaintySize=18
        // translate
        switch (mysyst)
        {
        case  JesUp : {miniAODUnc = pat::MET::JetEnUp; break;}
        case  JesDown : {miniAODUnc = pat::MET::JetEnDown; break;}
        case  JerUp : {miniAODUnc = pat::MET::JetResUp; break;}
        case  JerDown : {miniAODUnc = pat::MET::JetResDown; break;}
        case  UnclusterUp : {miniAODUnc = pat::MET::UnclusteredEnUp; break;}
        case  UnclusterDown : {miniAODUnc = pat::MET::UnclusteredEnDown; break;}
        case  TauUp : {miniAODUnc = pat::MET::TauEnUp; break;}
        case  TauDown : {miniAODUnc = pat::MET::TauEnDown; break;}
        case  PhotonUp : {miniAODUnc = pat::MET::PhotonEnDown; break;}
        case  PhotonDown : {miniAODUnc = pat::MET::PhotonEnDown; break;}
        case  ElectronUp : {miniAODUnc = pat::MET::ElectronEnUp; break;}
        case  ElectronDown : {miniAODUnc = pat::MET::ElectronEnDown; break;}
        case  MuonUp : {miniAODUnc = pat::MET::MuonEnUp; break;}
        case  MuonDown : {miniAODUnc = pat::MET::MuonEnDown; break;}
        default : break;
        }
        
        if (miniAODUnc == pat::MET::METUncertaintySize)
            cout <<"[NeroMet]::[analyze]::[WARNING] unable to translate met syst,"<< int(mysyst) <<endl;

        /*
          new ( (*metPuppiSyst)[ mysyst ] ) TLorentzVector( 
          puppi . shiftedP4( miniAODUnc).px(), 
          puppi . shiftedP4(miniAODUnc).py(),  
          puppi . shiftedP4(miniAODUnc).pz(),  
          puppi . shiftedP4(miniAODUnc).energy()
          );
        */
        
        new ( (*metSyst)[ mysyst ] ) TLorentzVector( 
            met . shiftedP4( miniAODUnc).px(), 
            met . shiftedP4(miniAODUnc).py(),  
            met . shiftedP4(miniAODUnc).pz(),  
            met . shiftedP4(miniAODUnc).energy()
            );
    }// end syst loop
    

    if (IsExtend())
    {
        *SmearedMet = TLorentzVector(met.shiftedPx(pat::MET::NoShift,pat::MET::Type1Smear),met.shiftedPy(pat::MET::NoShift,pat::MET::Type1Smear),0,met.shiftedPt(pat::MET::NoShift,pat::MET::Type1Smear));
        
        //Different MET Computations
        TLorentzVector metnomu(met.px(),met.py(),met.pz(),met.energy());
        TLorentzVector tkMet(0,0,0,0); 
        TLorentzVector pfmet_3p0(0,0,0,0); 
        TLorentzVector ntrlMet(0,0,0,0);
        TLorentzVector phoMet(0,0,0,0);
        TLorentzVector hfMet(0,0,0,0);    
    
        if ( not handle_pf.isValid() ) { Logger::getInstance().Log( string("[NeroMet]: PF handle is not valid"),0);}

        for (unsigned int i = 0, n = handle_pf->size(); i < n; ++i) {
            const pat::PackedCandidate &cand = (*handle_pf)[i];

            // only up to eta 3
            if (std::abs(cand.pdgId()) == 13)
                metnomu += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());  

            // only charge hadrons
            if ( cand.charge() != 0 )
                tkMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());

            // neutral pf hadrons
            if ( cand.pdgId()==130 )
                ntrlMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
            
            // photon 
            if ( cand.pdgId()==22 )
                phoMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());

            // HF Candidates
            if ( cand.pdgId()==1 || cand.pdgId()==2 )
                hfMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());    
            
            if (std::abs(cand.eta()) < 3.0 ) 
                pfmet_3p0 += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
        }
        

        *pfMet_e3p0 = TLorentzVector( -pfmet_3p0 );
        *metNoMu = TLorentzVector(metnomu);  // no minus
        *trackMet = TLorentzVector( -tkMet );
        *neutralMet = TLorentzVector( -ntrlMet );
        *photonMet = TLorentzVector( -phoMet );
        *HFMet = TLorentzVector( -hfMet );

    }    
    // GEN INFO
    if ( not iEvent.isRealData () ){
        new ( (*genP4)[genP4->GetEntriesFast()]) TLorentzVector( met.genMET()->px(),met.genMET()->py(),met.genMET()->pz(),met.genMET()->energy()  );
    }


    return 0;
}

NEROREGISTER(NeroMet);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
