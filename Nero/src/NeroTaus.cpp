#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroTaus::NeroTaus(): 
        NeroCollection(),
        BareTaus()
{
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
    if ( not handle.isValid() ) cout<<"[NeroTaus]::[analyze]::[ERROR] handle is not valid"<<endl;

    for (const pat::Tau &tau : *handle) {

        if (tau.pt() < 18 ) continue;	
        if (tau.pt() < mMinPt ) continue;	
        
        /// miniaod taus = decayModeFindingNewDMs
        if ( mMinId != "" and !(tau.tauID(mMinId)) ) continue; // minimum requirement to be saved.
        if ( mMaxIso >=0 and tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits") >= mMaxIso ) continue;

        if ( fabs(tau.eta()) > mMinEta ) continue;
        
        // ------------ END SELECTION 

         float phoIso = 0.; for(auto cand : tau.isolationGammaCands() ) phoIso += cand->pt();//tau.isolationPFGammaCandsEtSum() ;
         float chIso  = 0.; for(auto cand : tau.isolationChargedHadrCands() ) chIso += cand->pt();//tau.isolationPFChargedHadrCandsPtSum();
         float nhIso  = 0.; for(auto cand : tau.isolationNeutrHadrCands() ) nhIso += cand->pt(); // PF Cands not exists in miniAOD
         float totIso = phoIso + chIso + nhIso;
        
        //FILL
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(tau.px(), tau.py(), tau.pz(), tau.energy());
        id -> push_back( tau.tauID("decayModeFinding"));
        Q -> push_back( tau.charge() );
        M -> push_back( tau.mass() );
        iso -> push_back( totIso ) ; 

        if (IsExtend() ){
            chargedIsoPtSum  -> push_back( tau.tauID("chargedIsoPtSum") );
            neutralIsoPtSum  -> push_back( tau.tauID("neutralIsoPtSum") );
            isoDeltaBetaCorr -> push_back( tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits"));

            againstEleLoose  -> push_back( tau.tauID("againstElectronLooseMVA5") );
            againstEleMedium -> push_back( tau.tauID("againstElectronMediumMVA5") );
            
            againstMuLoose   -> push_back( tau.tauID("againstMuonLoose3"));
            againstMuTight   -> push_back( tau.tauID("againstMuonTight3"));
        }


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
