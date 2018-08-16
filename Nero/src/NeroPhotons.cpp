#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Nero/interface/NeroFunctions.hpp"
#include <time.h>

//#define VERBOSE 2

NeroPhotons::NeroPhotons(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
    BarePhotons()
{

    mMinPt = 15;
    mMaxIso = -1;
    mMinNpho = 0;
    mMaxEta = 2.5;
    mMinId = "loose";

    //rnd_ = new TRandom3() ;

    token = cc.consumes<pat::PhotonCollection>(iConfig.getParameter<edm::InputTag>("photons"));
    uncalib_token = cc.consumes<pat::PhotonCollection>(edm::InputTag("slimmedPhotons"));

    ebRecHits_token = cc.mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("ebRecHits"));
    eeRecHits_token = cc.mayConsume<EcalRecHitCollection>(iConfig.getParameter<edm::InputTag>("eeRecHits"));
    mMinPt = iConfig.getParameter<double>("minPt");
    mMaxIso = iConfig.getParameter<double>("maxIso");
    mMinNpho = iConfig.getParameter<int>("minN");
    mMinId = iConfig.getParameter<string>("minId");
    mMaxEta = iConfig.getParameter<double>("minEta");

    rho_token       = cc.consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
    SetExtend (iConfig.getParameter<bool>("extendPhotons"));
}

NeroPhotons::~NeroPhotons(){
    // delete PhoCorr; 
    // delete rnd_; 
}

unsigned NeroPhotons::idStringToEnum(std::string idString)
{
    unsigned idEnum = 0;
    if (idString == "loose") { idEnum = PhoLoose; }
    else if (idString == "medium") { idEnum = PhoMedium; }
    else if (idString == "tight") { idEnum = PhoTight; }
    else if (idString == "vloose") { idEnum = PhoVLoose; }
    else if (idString == "highpt") { idEnum = PhoHighPt; }
    else if (idString == "monoph") { idEnum = PhoMonophBaseline; }
    else if (idString == "none")   { idEnum = PhoBaseline; }
    return idEnum;
}

int NeroPhotons::analyze(const edm::Event& iEvent,const edm::EventSetup &iSetup){

    kMinId = idStringToEnum(mMinId);

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    iEvent.getByToken(uncalib_token, uncalib_handle);
    iEvent.getByToken(rho_token, rho_handle);


    edm::Handle<EcalRecHitCollection> ebRecHits;
    edm::Handle<EcalRecHitCollection> eeRecHits;
    iEvent.getByToken(ebRecHits_token,ebRecHits);
    iEvent.getByToken(eeRecHits_token,eeRecHits);

    
    if ( not handle.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] handle is not valid"<<endl;
    if ( not uncalib_handle.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] uncalib_handle is not valid"<<endl;
    
    int iPho = -1;	
    for (auto &pho : *handle)
    {
        ++iPho;
        #ifdef VERBOSE
        if (VERBOSE>0) cout<<"[NeroPhotons]::[analyze]::[DEBUG] analyzing photon"<<iPho<<" pt="<<pho.pt() <<" pz"<<pho.pz() <<endl;
        #endif

        //if ( not pho.passElectronVeto ()  ) continue;
        // pt> 14 hOe <.15
        // r9()>0.8 , chargedHadronIso()<20, chargedHadronIso()<0.3*pt()
        if (pho.pt() <15 or pho.chargedHadronIso()/pho.pt() > 0.3) continue; // 10 -- 14  GeV photons are saved if chargedHadronIso()<10
        if (fabs(pho.eta()) > mMaxEta ) continue;
        if (pho.pt() < mMinPt) continue;

        #ifdef VERBOSE
        if (VERBOSE>1) cout<<"[NeroPhotons]::[analize]::[DEBUG2] photonInfo:" <<endl
            <<" \t pho.chargedHadronIso()/pho.pt() (0.3) "<<pho.chargedHadronIso()/pho.pt() <<endl
            <<" \t chargedHadronIso() (20) "<<pho.chargedHadronIso()<<endl
            <<" \t r9 (0.8) "<<pho.r9()<<endl
            <<" \t SC is non null? "<< pho.superCluster().isNonnull()<<endl
            <<" \t SEED is non null ? "<<pho.superCluster()->seed().isNonnull()<<endl
            <<endl;
            DetId id =  pho.superCluster()->seed() -> seed();
            if ( id.subdetId() == EcalBarrel ) {
                EcalRecHitCollection::const_iterator theSeedHit = ebRecHits->find (id); 
                cout<<" \t IS BARRELL SEED in RECHITS list? "<<int(theSeedHit != ebRecHits->end() )<<endl;
            }
            else if ( id.subdetId() == EcalEndcap ) {
                EcalRecHitCollection::const_iterator theSeedHit = eeRecHits->find (id); 
                cout<<" \t IS ENDCAP SEED in RECHITS list? "<<int(theSeedHit != eeRecHits->end() )<<endl;
            }
        #endif

        float _chIso_ =  pho.chargedHadronIso();
        float _nhIso_ =  pho.neutralHadronIso() ;
        float _phIso_ =  pho.photonIso();
        
        float _puIso_ =  pho.puChargedHadronIso() ; 
        float totIso = _chIso_ + _nhIso_ + _phIso_;

        //cutBasedPhotonID-Fall17-94X-V1-loose cutBasedPhotonID-Fall17-94X-V1-medium cutBasedPhotonID-Fall17-94X-V1-tight 
        //mvaPhoID-RunIIFall17-v1-wp80 mvaPhoID-RunIIFall17-v1-wp90 mvaPhoID-RunIIFall17-v1p1-wp80 mvaPhoID-RunIIFall17-v1p1-wp90
        //mvaPhoID-Spring16-nonTrig-V1-wp80 mvaPhoID-Spring16-nonTrig-V1-wp90 
        //bool isPassLoose  = cutBasedPhotonId(pho,Loose);	
        //bool isPassMedium = cutBasedPhotonId(pho,Medium);	
        //bool isPassTight  = cutBasedPhotonId(pho,Tight);	
        //bool isPassVLoose25 = cutBasedPhotonId( pho, Loose, false, false); // no pho iso , no sieie
        bool isPassMonophBaseline = cutBasedPhotonId( pho, MonoPhotonBaseline, true, false); // iso bool does nothing right now, sieie false allows events with sieie > 0.015
        bool isPassLoose = pho.photonID("cutBasedPhotonID-Fall17-94X-V1-loose");
        bool isPassMedium = pho.photonID("cutBasedPhotonID-Fall17-94X-V1-medium");
        bool isPassTight = pho.photonID("cutBasedPhotonID-Fall17-94X-V1-tight");

        if (not isPassLoose and not isPassMonophBaseline) continue;
        if (mMaxIso >=0 and totIso > mMaxIso) continue;

        unsigned bits=0;

        bits |= 1; // baseline bit
        bits |= (pho. passElectronVeto() * isPassTight) * PhoTight  ;
        bits |= (pho. passElectronVeto() * isPassMedium) * PhoMedium;
        bits |= (pho. passElectronVeto() * isPassLoose) * PhoLoose;
        //bits |= (pho. passElectronVeto() * isPassVLoose25) * PhoVLoose;
        bits |= isPassMonophBaseline * PhoMonophBaseline;

        bits |= isPassTight * PhoTightNoEVeto  ;
        bits |= isPassMedium * PhoMediumNoEVeto;
        bits |= isPassLoose * PhoLooseNoEVeto;
        
        if ( not (bits & kMinId) ) continue; // even if there is some misalignment ntuples will not be corrupted

        bits |= pho.passElectronVeto() * PhoElectronVeto;
        bits |= !pho.hasPixelSeed() * PhoPixelSeedVeto;

        TLorentzVector phoP4 = TLorentzVector(pho.px(),pho.py(),pho.pz(),pho.energy());
        phoP4 *= pho.userFloat("ecalEnergyPostCorr") / pho.energy();
        
        //double Ecorr=NeroFunctions::getEGSeedCorrections(pho,ebRecHits); 
        double Ecorr=pho.userFloat("ecalEnergyPostCorr") / pho.energy();
        //
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(phoP4);
        corr->push_back(Ecorr);
        iso->push_back(totIso);	
        sieie -> push_back( pho. full5x5_sigmaIetaIeta() );

        selBits -> push_back( bits);

        chIso -> push_back( _chIso_);
        phoIso -> push_back( _phIso_ ) ;
        nhIso -> push_back ( _nhIso_ ) ;
        puIso -> push_back ( _puIso_ ) ;

        etaSC -> push_back(  pho.superCluster()->eta() ) ;

        //Requested UserFloat PhotonMVAEstimatorRunIIFall17v1 is not available! Possible UserFloats are:
        //PhotonMVAEstimatorRun2Spring15NonTrig25nsV2p1Values PhotonMVAEstimatorRun2Spring15NonTrig50nsV2p1Values PhotonMVAEstimatorRun2Spring16NonTrigV1Values PhotonMVAEstimatorRunIIFall17v1Values PhotonMVAEstimatorRunIIFall17v1p1Values ecalEnergyErrPostCorr ecalEnergyErrPreCorr ecalEnergyPostCorr ecalEnergyPreCorr energyScaleDown energyScaleEtDown energyScaleEtUp energyScaleGainDown energyScaleGainUp energyScaleStatDown energyScaleStatUp energyScaleSystDown energyScaleSystUp energyScaleUp energyScaleValue energySigmaDown energySigmaPhiDown energySigmaPhiUp energySigmaRhoDown energySigmaRhoUp energySigmaUp energySigmaValue energySmearNrSigma phoChargedIsolation phoNeutralHadronIsolation phoPhotonIsolation phoWorstChargedIsolation
        double mvaValue= pho.userFloat("PhotonMVAEstimatorRunIIFall17v1Values");
        mva   -> push_back(mvaValue);

        /*
        chIsoRC -> push_back( _chIsoRC_);
        phoIsoRC -> push_back( _phIsoRC_ ) ;
        nhIsoRC -> push_back ( _nhIsoRC_ ) ;
        puIsoRC -> push_back ( _puIsoRC_ ) ;
        */

        if (IsExtend() ){
            #ifdef VERBOSE
            if (VERBOSE>0) cout<<"[NeroPhotons]::[analyze]::[DEBUG] Extend"<<endl;
            #endif
            rawpt->push_back( (*uncalib_handle)[iPho].pt());
            rawScEnergy->push_back(pho.superCluster()->rawEnergy());
            
            #ifdef VERBOSE
            if (VERBOSE>0) cout<<"[NeroPhotons]::[analyze]::[DEBUG] ShowerShapes"<<endl;
            #endif
            hOverE->push_back(pho.hadTowOverEm()); //pho.hadronicOverEm());
            //chWorstIso->push_back( (*iso_wch)[ref] );
            
            e33->push_back(pho.full5x5_e3x3());
            e55->push_back(pho.full5x5_e5x5());

            sipip->push_back(pho.full5x5_showerShapeVariables().sigmaIphiIphi);
            sieip->push_back(pho.full5x5_showerShapeVariables().sigmaIetaIphi);
            r9->push_back(pho.r9());
            s4->push_back(pho.eMax()/(pho.eMax()+pho.eTop()+pho.eBottom()+pho.eLeft()+pho.eRight()));

            mipEnergy->push_back(pho.mipTotEnergy());

            #ifdef VERBOSE
            if (VERBOSE>0) cout<<"[NeroPhotons]::[analyze]::[DEBUG] CLUSTER TOOLS"<<endl;
            #endif

            clusterTools = new EcalClusterLazyTools(iEvent, iSetup, ebRecHits_token, eeRecHits_token);

            #ifdef VERBOSE
            if (VERBOSE>0) cout<<"[NeroPhotons]::[analyze]::[DEBUG] TIME"<<endl;
            #endif

            /* miniAOD requirements are based on miniAOD pt. Rare case can switch only due to pT. 
             * Time requires recHit seed to be present.
             * It may throw invalidCluster, otherwise
             */
            bool seedInRecHits=false;
            DetId id =  pho.superCluster()->seed() -> seed();
            if ( id.subdetId() == EcalBarrel ) {
                EcalRecHitCollection::const_iterator theSeedHit = ebRecHits->find (id); 
                seedInRecHits= (theSeedHit != ebRecHits->end() );
            }
            else if ( id.subdetId() == EcalEndcap ) {
                EcalRecHitCollection::const_iterator theSeedHit = eeRecHits->find (id); 
                seedInRecHits= (theSeedHit != eeRecHits->end() );
            }

            if (seedInRecHits)
            {
                time->push_back(clusterTools->SuperClusterSeedTime(*pho.superCluster()));
                emax->push_back(clusterTools->eMax(*pho.superCluster()));
                e2nd->push_back(clusterTools->e2nd(*pho.superCluster()));
            }
            else
            {
                time->push_back(-999.);
                emax->push_back(-999.);
                e2nd->push_back(-999.);
            }



            // timeSpan->push_back( ??? );
            delete clusterTools;
            
            // genMatched->push_back( ??? );
        }
        #ifdef VERBOSE
        if (VERBOSE>0) cout<<"[NeroPhotons]::[analyze]::[DEBUG] DONE With this photon"<<endl;
        #endif
    }
   
    if ( int(selBits -> size()) < mMinNpho  ) return 1;

    return 0;
}

bool NeroPhotons::cutBasedPhotonId( const pat::Photon& pho, PHOID type, bool withIso, bool withSieie)
{
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2#
    float hoe   = pho.hadTowOverEm();
    float sieie = pho.full5x5_sigmaIetaIeta() ;
    float chiso = pho.chargedHadronIso();
    float nhiso= pho.neutralHadronIso() ;
    float phoiso = pho.photonIso();
    float rho  = *rho_handle;
    float pt=pho.pt();

    //float aeta = fabs(pho.eta());

    // ------------ BARREL ------------
    if ( pho.isEB() )
        {

        // ---------------- 25ns ---------------
        if ( type == MonoPhotonBaseline) // using spring15 loose selections, but with sieie and chiso sidebands
            {
                if (hoe >= 0.05   ) return false;
                else if (sieie >= 0.015  and withSieie  ) return false;
                else if (chiso - cutBasedPhotonIdEffArea(pho,CH25) * rho >= 11.0) return false;// 
                else if (nhiso - cutBasedPhotonIdEffArea(pho,NH25)*rho >= 1.92 + 0.014*pho.pt() + 0.000019*pho.pt()*pho.pt() ) return false;// 
                else if (phoiso - cutBasedPhotonIdEffArea(pho,PH25) *rho >= 0.81 + 0.0053*pho.pt() ) return false ;//

                else return true;
            }
        //  Moriond17
        if ( type == Loose ) 
            {
                if (hoe >= 0.043   ) return false;
                else if (sieie >=0.0101   and withSieie  ) return false;
                else if ( chiso - cutBasedPhotonIdEffArea(pho,CH25) * rho >= 1.403   ) return false;// 
                else if (nhiso - cutBasedPhotonIdEffArea(pho,NH25)*rho >= 15.959  ) return false;// 
                else if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,PH25) *rho >=3.060 ) return false ;//

                else return true;
            }
        if ( type == Medium )
            {
                if (hoe >= 0.032   ) return false;
                else if (sieie >=0.0101   and withSieie ) return false;
                else if (chiso -  cutBasedPhotonIdEffArea(pho,CH25)*rho >=  0.430  ) return false;
                else if (nhiso - cutBasedPhotonIdEffArea(pho,NH25)*rho >= 2.133) return false;
                else if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,PH25) *rho >= 2.344  ) return false ; 
                else return true;
            }
        if ( type == Tight )
            {
                if (hoe >= 0.022  ) return false;
                else if (sieie >= 0.0099  and withSieie ) return false;
                else if ( chiso - cutBasedPhotonIdEffArea(pho,CH25) * rho >= 0.101  ) return false;
                else if (nhiso - cutBasedPhotonIdEffArea(pho,NH25)*rho >= 0.137 ) return false;
                else if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,PH25) *rho >= 2.308 ) return false ;
                else return true;
            }
        }
    // ------------ ENDCAP ------------
    else if (pho.isEE()){
        if (not withSieie and sieie > 0.050) return false;  // pu a very loose sieie requirement
        if (not withIso and phoiso > 20 ) return false; // put  a very loose pho-iso requirement
        if ( type == Loose ) 
            {
                if (hoe >= 0.026     ) return false;
                else if (sieie >= 0.0267 and withSieie  ) return false;
                else if ( chiso - cutBasedPhotonIdEffArea(pho,CH25) * rho >= 2.809   ) return false;// 
                else if (nhiso - cutBasedPhotonIdEffArea(pho,NH25)*rho >= 7.056) return false;// 
                else if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,PH25) *rho >= 4.766  ) return false ;//

                else return true;
            }
        if ( type == Medium )
            {
                if (hoe >= 0.024   ) return false;
                else if (sieie >= 0.0267  and withSieie ) return false;
                else if (chiso -  cutBasedPhotonIdEffArea(pho,CH25)*rho >= 0.846  ) return false;
                else if (nhiso - cutBasedPhotonIdEffArea(pho,NH25)*rho >= 1.679 ) return false;
                else if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,PH25) *rho >= 4.271  ) return false ; 
                else return true;
            }
        if ( type == Tight )
            {
                if (hoe >= 0.021   ) return false;
                else if (sieie >= 0.0267  and withSieie ) return false;
                else if ( chiso - cutBasedPhotonIdEffArea(pho,CH25) * rho >= 0.134  ) return false;
                else if (nhiso - cutBasedPhotonIdEffArea(pho,NH25)*rho >= 1.615  ) return false;
                else if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,PH25) *rho >= 3.107  ) return false ;
                else return true;
            }

        }
    return false;

}

float NeroPhotons::cutBasedPhotonIdEffArea( const pat::Photon & pho,PHOISO type){
    float aeta = fabs( pho.eta() ) ;
    // --------------- 25 ns ----------------
    if (type == CH25)
        {
        if (aeta < 1.0   ) return 0.057;
        else if (aeta < 1.479 ) return 0.052;
        else if (aeta < 2.0   ) return 0.045;
        else if (aeta < 2.2   ) return 0.044;
        else if (aeta < 2.3   ) return 0.043;
        else if (aeta < 2.4   ) return 0.038;
        else                    return 0.034;
        }
    if (type == NH25)
        {
        if (aeta < 1.0   ) return 0.0620 ;
        else if (aeta < 1.479 ) return 0.1120 ;
        else if (aeta < 2.0   ) return 0.0770 ;
        else if (aeta < 2.2   ) return 0.0330 ;
        else if (aeta < 2.3   ) return 0.0070 ;
        else if (aeta < 2.4   ) return 0.0070 ;
        else                    return 0.0120 ;
        }
    if (type == PH25)
        {
        if (aeta < 1.0   ) return 0.1220;
        else if (aeta < 1.479 ) return 0.1100;
        else if (aeta < 2.0   ) return 0.0610;
        else if (aeta < 2.2   ) return 0.0770;
        else if (aeta < 2.3   ) return 0.0990;
        else if (aeta < 2.4   ) return 0.1090;
        else                    return 0.1340;
        }
    
    return -999.;
}

NEROREGISTER(NeroPhotons);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
