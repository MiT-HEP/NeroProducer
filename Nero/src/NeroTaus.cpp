#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroTaus::NeroTaus(): BareTaus(){

    mMinPt = 18;
    mMinNtaus = 0;
    mMinEta = 2.3;
    mMinId = "decayModeFinding";
    mMaxIso = -1;
}

NeroTaus::~NeroTaus(){
}

int NeroTaus::analyze(const edm::Event & iEvent)
{
    if ( mOnlyMc  ) return 0;

    iEvent.getByToken(token, handle);
    for (const pat::Tau &tau : *handle) {

        if (tau.pt() < 18 ) continue;	
        if (tau.pt() < mMinPt ) continue;	

        //float trkIso = isolationTracksPtSum ();
        //bool Tau::ExistIsolationCands 	( ) 	const
        //reco::CandidatePtrVector Tau::isolationGammaCands () const

        if ( fabs(tau.eta()) > mMinEta ) continue;
        if ( tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits") >= 5 ) continue;
        if ( !(tau.tauID(mMinId) ) ) continue;

        float phoIso = 0.; for(auto cand : tau.isolationGammaCands() ) phoIso += cand->pt();//tau.isolationPFGammaCandsEtSum() ;
        float chIso  = 0.; for(auto cand : tau.isolationChargedHadrCands() ) chIso += cand->pt();//tau.isolationPFChargedHadrCandsPtSum();
        float nhIso  = 0.; for(auto cand : tau.isolationNeutrHadrCands() ) nhIso += cand->pt(); // PF Cands not exists in miniAOD
        float totIso = phoIso + chIso + nhIso;
        //FILL
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(tau.px(), tau.py(), tau.pz(), tau.energy());
        id -> push_back( tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits"));
        Q -> push_back( tau.charge() );
        M -> push_back( tau.mass() );
        iso -> push_back( totIso ) ; 

    }
    if( int(id->size()) < mMinNtaus) return 1;
    return 0;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
