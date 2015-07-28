#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroMet::NeroMet() : BareMet() {
    pf = NULL;
}

NeroMet::~NeroMet(){
}


int NeroMet::analyze(const edm::Event& iEvent){

    if ( mOnlyMc  ) return 0; // in principle I would like to have the gen met: TODO

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    const pat::MET &met = handle->front();

    //p4 -> AddLast(new TLorentzVector(met.px(),met.py(),met.pz(),met.energy()));
    new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( met.px(),met.py(),met.pz(),met.energy()  );

    ptJESUP -> push_back( met.shiftedPt(pat::MET::JetEnUp) );
    ptJESDOWN -> push_back( met.shiftedPt(pat::MET::JetEnDown) );

    //MetNoMu
    float metnomu_x = met.px();
    float metnomu_y = met.py();
   
    
    //iEvent.getByToken(pfToken_, pfs);
    if ( pf == NULL ) cout<<"[NeroMet]::[analyze]::[ERROR] PF pointer is null. Run NeroPF. "<<endl; 

    for (unsigned int i = 0, n = pf->handle->size(); i < n; ++i) {
        const pat::PackedCandidate &cand = (*pf->handle)[i];
        if (std::abs(cand.pdgId()) == 13 ){
            metnomu_x += cand.px();
            metnomu_y += cand.py();
        }
    }
    
    metNoMu = sqrt(metnomu_x*metnomu_x + metnomu_y*metnomu_y);

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
