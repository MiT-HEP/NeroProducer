#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroMet::NeroMet() : 
    NeroCollection(),
    BareMet() 
{
    pf = NULL;
}

NeroMet::~NeroMet(){
}


int NeroMet::analyze(const edm::Event& iEvent){

    if ( mOnlyMc  ) return 0; // in principle I would like to have the gen met: TODO

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    if ( not handle.isValid() ) cout<<"[NeroMet]::[analyze]::[ERROR] handle is not valid"<<endl;

    iEvent.getByToken(token_noHF,handle_noHF);
    if ( not handle_noHF.isValid() ) cout<<"[NeroMet]::[analyze]::[ERROR] handle_noHF is not valid"<<endl;

    iEvent.getByToken(token_puppi,handle_puppi);
    if ( not handle_puppi.isValid() ) cout<<"[NeroMet]::[analyze]::[ERROR] handle_puppi is not valid"<<endl;

    iEvent.getByToken(token_puppiUncorr,handle_puppiUncorr);
    if ( not handle_puppiUncorr.isValid() ) cout<<"[NeroMet]::[analyze]::[ERROR] handle_puppiUncorr is not valid"<<endl;
    //--

    const pat::MET &met = handle->front();

    caloMet_Pt = met.caloMETPt();
    caloMet_Phi = met.caloMETPhi();
    caloMet_SumEt = met.caloMETSumEt();


    // FILL
    if (jets->mApplyJEC) {
        TLorentzVector metHolder(met.px(),met.py(),met.pz(),met.energy());
        TLorentzVector *jetVec;
        for (UInt_t iJet = 0; iJet != jets->refPt->size(); ++iJet) {
            jetVec = (TLorentzVector*) jets->p4->At(iJet);
            metHolder += (1 - ((*jets->refPt)[iJet]/jetVec->Pt())) * (*jetVec);
        }
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(metHolder);
    }
    else {
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( met.px(),met.py(),met.pz(),met.energy()  );
    }

    sumEtRaw = met.uncorSumEt();

    ptJESUP -> push_back( met.shiftedPt(pat::MET::JetEnUp) );
    ptJESDOWN -> push_back( met.shiftedPt(pat::MET::JetEnDown) );

    rawMet_Pt = met.uncorPt(); 
    rawMet_Phi = met.uncorPhi();


    if (IsExtend())
    {
        //MetNoMu
        TLorentzVector metnomu(met.px(),met.py(),met.pz(),met.energy());
        TLorentzVector tkMet(0,0,0,0); 
        TLorentzVector pfmet_3p0(0,0,0,0); 

        if ( pf == NULL ) cout<<"[NeroMet]::[analyze]::[ERROR] PF pointer is null. Run NeroPF. "<<endl; 

        for (unsigned int i = 0, n = pf->handle->size(); i < n; ++i) {
            const pat::PackedCandidate &cand = (*pf->handle)[i];

            // only up to eta 3
            if (std::abs(cand.pdgId()) == 13)
                metnomu += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());  

            // only charge hadrons
            if ( cand.charge() != 0 )
                tkMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());

            if (std::abs(cand.eta()) < 3.0 ) 
                pfmet_3p0 += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
        }

        *pfMet_e3p0 = TLorentzVector( -pfmet_3p0 );
        *metNoMu = TLorentzVector(metnomu);  // no minus
        *trackMet = TLorentzVector( -tkMet );

        const pat::MET &metnohf = handle_noHF->front(); 
        *metNoHF = TLorentzVector( metnohf.px(),metnohf.py(),metnohf.pz(),metnohf.energy());
        sumEtRawNoHF = metnohf.uncorSumEt();

        auto &puppi = handle_puppi->front(); 
        *metPuppi =  TLorentzVector( puppi.px(), puppi.py(),puppi.pz(),puppi.energy() );
        sumEtRawPuppi = handle_puppiUncorr->front().sumEt();

        /*
        for(Syst mysyst = (Syst)0; mysyst < MaxSyst ; mysyst = (Syst)((int)mysyst +1 ) )
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

            new ( (*metPuppiSyst)[ mysyst ] ) TLorentzVector( 
                    puppi . shiftedP4( miniAODUnc).px(), 
                    puppi . shiftedP4(miniAODUnc).py(),  
                    puppi . shiftedP4(miniAODUnc).pz(),  
                    puppi . shiftedP4(miniAODUnc).energy()
                    );
        }// end syst loop
        */

    }    
    // GEN INFO
    if ( not iEvent.isRealData () ){
        new ( (*genP4)[genP4->GetEntriesFast()]) TLorentzVector( met.genMET()->px(),met.genMET()->py(),met.genMET()->pz(),met.genMET()->energy()  );
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
