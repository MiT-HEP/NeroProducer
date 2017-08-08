#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "DataFormats/TauReco/interface/PFTau.h"

NeroTaus::NeroTaus(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
        NeroCollection(cc, iConfig),
        BareTaus()
{

    token = cc.consumes<pat::TauCollection>(iConfig.getParameter<edm::InputTag>("taus"));
    mMinPt = iConfig.getParameter<double>("minPt");
    mMinNtaus = iConfig.getParameter<int>("minN");
    mMinEta = iConfig.getParameter<double>("minEta");
    mMinId = iConfig.getParameter<string>("minId");
    mMaxIso = iConfig.getParameter<double>("maxIso");
    SetExtend ( iConfig.getParameter<bool>("extendTau") );
    //taus -> SetMatch( iConfig.getParameter<bool>("matchTau") );
}

NeroTaus::~NeroTaus(){
}

int NeroTaus::analyze(const edm::Event & iEvent)
{
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

        unsigned bits = 0;
        bits |= bool(tau.tauID("decayModeFindingNewDMs") ) * TauDecayModeFindingNewDMs;
        bits |= bool(tau.tauID("decayModeFinding") ) * TauDecayModeFinding;
        bits |= bool(tau.tauID("againstElectronLooseMVA6") )* AgainstEleLoose  ; 
        bits |= bool(tau.tauID("againstElectronMediumMVA6"))* AgainstEleMedium ;
        bits |= bool(tau.tauID("againstElectronTightMVA6") )* AgainstEleTight  ; 
        bits |= bool(tau.tauID("againstMuonLoose3"))        * AgainstMuLoose   ;
        bits |= bool(tau.tauID("againstMuonTight3"))        * AgainstMuTight   ;
        // old id the following only in 76 v2 
        bits |= bool(tau.tauID("byVLooseIsolationMVArun2v1DBoldDMwLT")) * byVLooseIsolationMVArun2v1DBoldDMwLT;
        bits |= bool(tau.tauID("byLooseIsolationMVArun2v1DBoldDMwLT")) * byLooseIsolationMVArun2v1DBoldDMwLT;
        bits |= bool(tau.tauID("byMediumIsolationMVArun2v1DBoldDMwLT")) * byMediumIsolationMVArun2v1DBoldDMwLT;
        bits |= bool(tau.tauID("byTightIsolationMVArun2v1DBoldDMwLT")) * byTightIsolationMVArun2v1DBoldDMwLT;
        bits |= bool(tau.tauID("byVTightIsolationMVArun2v1DBoldDMwLT")) * byVTightIsolationMVArun2v1DBoldDMwLT;
        // new id
        bits |= bool(tau.tauID("byVLooseIsolationMVArun2v1DBnewDMwLT")) * byVLooseIsolationMVArun2v1DBnewDMwLT;
        bits |= bool(tau.tauID("byLooseIsolationMVArun2v1DBnewDMwLT")) * byLooseIsolationMVArun2v1DBnewDMwLT;
        bits |= bool(tau.tauID("byMediumIsolationMVArun2v1DBnewDMwLT")) * byMediumIsolationMVArun2v1DBnewDMwLT;
        bits |= bool(tau.tauID("byTightIsolationMVArun2v1DBnewDMwLT")) * byTightIsolationMVArun2v1DBnewDMwLT;
        bits |= bool(tau.tauID("byVTightIsolationMVArun2v1DBnewDMwLT")) * byVTightIsolationMVArun2v1DBnewDMwLT;
        // DB ISO WP
        bits |= bool(tau.tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits")) * byLooseCombinedIsolationDeltaBetaCorr3Hits;
        bits |= bool(tau.tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits")) * byMediumCombinedIsolationDeltaBetaCorr3Hits;
        bits |= bool(tau.tauID("byTightCombinedIsolationDeltaBetaCorr3Hits")) * byTightCombinedIsolationDeltaBetaCorr3Hits;

        switch (tau.decayMode() ) {
            case reco::PFTau::kOneProng0PiZero : { bits |= OneProng; bits |= ZeroPiZero; break;}
            case reco::PFTau::kOneProng1PiZero : { bits |= OneProng; bits |= OnePiZero; break;}
            case reco::PFTau::kOneProng2PiZero : { bits |= OneProng; bits |= TwoPiZero; break;}
            case reco::PFTau::kOneProng3PiZero : { bits |= OneProng; bits |= ThreePiZero; break;}
            case reco::PFTau::kOneProngNPiZero : { bits |= OneProng; bits |= ThreePiZero; break;}

            case reco::PFTau::kTwoProng0PiZero : { bits |= TwoProng; bits |= ZeroPiZero; break;}
            case reco::PFTau::kTwoProng1PiZero : { bits |= TwoProng; bits |= OnePiZero; break;}
            case reco::PFTau::kTwoProng2PiZero : { bits |= TwoProng; bits |= TwoPiZero; break;}
            case reco::PFTau::kTwoProng3PiZero : { bits |= TwoProng; bits |= ThreePiZero; break;}
            case reco::PFTau::kTwoProngNPiZero : { bits |= TwoProng; bits |= ThreePiZero; break;} // three or moreY

            case reco::PFTau::kThreeProng0PiZero : { bits |= ThreeProng; bits |= ZeroPiZero; break;}
            case reco::PFTau::kThreeProng1PiZero : { bits |= ThreeProng; bits |= OnePiZero; break;}
            case reco::PFTau::kThreeProng2PiZero : { bits |= ThreeProng; bits |= TwoPiZero; break;}
            case reco::PFTau::kThreeProng3PiZero : { bits |= ThreeProng; bits |= ThreePiZero; break;}
            case reco::PFTau::kThreeProngNPiZero : { bits |= ThreeProng; bits |= ThreePiZero; break;} // three or moreY

            case reco::PFTau::kNull : {break;} 
            case reco::PFTau::kRareDecayMode : {break;} 
        }  

        selBits -> push_back(bits);
        Q -> push_back( tau.charge() );
        M -> push_back( tau.mass() );
        iso -> push_back( totIso ) ; 
        leadTrackPt -> push_back( tau.leadChargedHadrCand()->p4().Pt() ) ;

        chargedIsoPtSum  -> push_back( tau.tauID("chargedIsoPtSum") );
        neutralIsoPtSum  -> push_back( tau.tauID("neutralIsoPtSum") );
        isoDeltaBetaCorr -> push_back( tau.tauID("byCombinedIsolationDeltaBetaCorrRaw3Hits"));
        isoMva -> push_back(tau.tauID("byIsolationMVArun2v1DBnewDMwLTraw") );


    }
    if( int(selBits->size()) < mMinNtaus) return 1;
    return 0;
}

NEROREGISTER(NeroTaus);


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
