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
    const pat::MET &met = handle->front();

    // FILL
    new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( met.px(),met.py(),met.pz(),met.energy()  );

    ptJESUP -> push_back( met.shiftedPt(pat::MET::JetEnUp) );
    ptJESDOWN -> push_back( met.shiftedPt(pat::MET::JetEnDown) );


    if (IsExtend())
    {
        //MetNoMu
        TLorentzVector metnomu(met.px(),met.py(),met.pz(),met.energy());
        TLorentzVector pfmet_e3p0(0,0,0,0);
        TLorentzVector chMet(0,0,0,0); 
        TLorentzVector nhMet(0,0,0,0); 
        TLorentzVector phoMet(0,0,0,0); 
        
    
        if ( pf == NULL ) cout<<"[NeroMet]::[analyze]::[ERROR] PF pointer is null. Run NeroPF. "<<endl; 
        
        for (unsigned int i = 0, n = pf->handle->size(); i < n; ++i) {
            const pat::PackedCandidate &cand = (*pf->handle)[i];
            
            // only up to eta 3
            if (std::abs(cand.eta()) < 3.0)
                pfmet_e3p0 += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
            
            if (std::abs(cand.pdgId()) == 13)
                metnomu += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());  
            
            // only charge hadrons
            if ( cand.charge() != 0 and cand.pdgId() > 20 )
                chMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
            if ( cand.charge() == 0 and cand.pdgId() == 22 ) 
                phoMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
            if ( cand.charge() == 0 and cand.pdgId() != 22 ) 
                nhMet += TLorentzVector(cand.px(),cand.py(),cand.pz(),cand.energy());
        }
        
        *metNoMu = TLorentzVector(metnomu);    
        *metChargedHadron = TLorentzVector(chMet);
        *metNeutralHadron = TLorentzVector(nhMet);
        *metNeutralEM = TLorentzVector(phoMet);
        *pfMet_e3p0 = TLorentzVector(pfmet_e3p0);
    
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
