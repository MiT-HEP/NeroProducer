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
    //--

    const pat::MET &met = handle->front();

    caloMet_Pt = met.caloMETPt();
    caloMet_Phi = met.caloMETPhi();
    caloMet_SumEt = met.caloMETSumEt();
    

    // FILL
    new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( met.px(),met.py(),met.pz(),met.energy()  );

    ptJESUP -> push_back( met.shiftedPt(pat::MET::JetEnUp) );
    ptJESDOWN -> push_back( met.shiftedPt(pat::MET::JetEnDown) );


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

            if (cand.eta() <3.0) 
                pfmet_3p0 += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
        }
       
        *pfMet_e3p0 = TLorentzVector( pfmet_3p0 );
        *metNoMu = TLorentzVector(metnomu);    
        *trackMet = TLorentzVector(tkMet);

        const pat::MET &metnohf = handle_noHF->front(); 
        *metNoHF = TLorentzVector( metnohf.px(),metnohf.py(),metnohf.pz(),metnohf.energy());

        const pat::MET &puppi = handle_puppi->front(); 
        *metPuppi =  TLorentzVector( puppi.px(), puppi.py(),puppi.pz(),puppi.energy() );
    
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
