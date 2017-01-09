#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Nero/interface/NeroFunctions.hpp"
#include <time.h>

//#define VERBOSE 2

NeroPhotons::NeroPhotons() : 
        NeroCollection(),
        BarePhotons()
{

    mMinPt = 15;
    mMaxIso = -1;
    mMinNpho = 0;
    mMaxEta = 2.5;
    mMinId = "loose";

    pf = NULL;
    rnd_ = new TRandom3() ;
}

NeroPhotons::~NeroPhotons(){
    delete PhoCorr; 
    delete rnd_; 
}

unsigned NeroPhotons::idStringToEnum(std::string idString)
{
    unsigned idEnum = 0;
    if (idString == "loose") { idEnum = PhoLoose; }
    else if (idString == "medium") { idEnum = PhoMedium; }
    else if (idString == "tight") { idEnum = PhoTight; }
    else if (idString == "vloose50") { idEnum = PhoVLoose50; }
    else if (idString == "vloose25") { idEnum = PhoVLoose25; }
    else if (idString == "highpt") { idEnum = PhoHighPt; }
    else if (idString == "monoph") { idEnum = PhoMonophBaseline; }
    else if (idString == "none")   { idEnum = PhoBaseline; }
    return idEnum;
}

int NeroPhotons::analyze(const edm::Event& iEvent,const edm::EventSetup &iSetup){

    if ( mOnlyMc  ) return 0;
   
    kMinId = idStringToEnum(mMinId);

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);

    // ID and ISO
    iEvent.getByToken(loose_id_token,loose_id);
    iEvent.getByToken(medium_id_token,medium_id);
    iEvent.getByToken(tight_id_token,tight_id);
    iEvent.getByToken(iso_ch_token, iso_ch);
    iEvent.getByToken(iso_nh_token, iso_nh);
    iEvent.getByToken(iso_pho_token, iso_pho);

    edm::Handle<EcalRecHitCollection> ebRecHits;
    edm::Handle<EcalRecHitCollection> eeRecHits;
    iEvent.getByToken(ebRecHits_token,ebRecHits);
    iEvent.getByToken(eeRecHits_token,eeRecHits);

    
    if ( not handle.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] handle is not valid"<<endl;
    if ( not loose_id.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] loose_id is not valid"<<endl;
    if ( not medium_id.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] medium_id is not valid"<<endl;
    if ( not tight_id.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] tight_id is not valid"<<endl;
    if ( not iso_ch.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] iso_ch is not valid"<<endl;
    if ( not iso_nh.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] iso_nh is not valid"<<endl;
    if ( not iso_pho.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] iso_pho is not valid"<<endl;
    
    if (IsExtend() ) {
        iEvent.getByToken(iso_wch_token, iso_wch);
        if ( not iso_wch.isValid() ) cout<<"[NeroPhotons]::[analyze]::[ERROR] iso_wch is not valid"<<endl;
    }

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
            <<endl;
        #endif

        edm::RefToBase<pat::Photon> ref ( edm::Ref< pat::PhotonCollection >(handle, iPho) ) ;
        float _chIso_ =  (*iso_ch) [ref];
        float _nhIso_ =  (*iso_nh) [ref];
        float _phIso_ =  (*iso_pho)[ref];	
        
        float _puIso_ =  pho.puChargedHadronIso() ;  // the other are eff area corrected, no need for this correction
        float totIso = _chIso_ + _nhIso_ + _phIso_;

        bool isPassLoose  = (*loose_id)[ref];	
        bool isPassMedium = (*medium_id)[ref];	
        bool isPassTight  = (*tight_id)[ref];	
        bool isPassVLoose50 = cutBasedPhotonId( pho, "loose_50ns", false, false); // no pho iso , no sieie
        bool isPassVLoose25 = cutBasedPhotonId( pho, "loose_25ns", false, false); // no pho iso , no sieie
        bool isPassMonophBaseline = cutBasedPhotonId( pho, "monoph_baseline", true, false); // iso bool does nothing right now, sieie false allows events with sieie > 0.015

        //if (not isPassVLoose) continue;
        if (mMaxIso >=0 and totIso > mMaxIso) continue;

        unsigned bits=0;

        bits |= 1; // baseline bit
        bits |= isPassTight * PhoTight;
        bits |= isPassMedium * PhoMedium;
        bits |= isPassLoose * PhoLoose;
        bits |= isPassVLoose50 * PhoVLoose50;
        bits |= isPassVLoose25 * PhoVLoose25;
        bits |= isPassMonophBaseline * PhoMonophBaseline;
        
        if ( not (bits & kMinId) ) continue; // even if there is some misalignment ntuples will not be corrupted

        bits |= pho.passElectronVeto() * PhoElectronVeto;
        bits |= !pho.hasPixelSeed() * PhoPixelSeedVeto;

        double Ecorr=NeroFunctions::getEGSeedCorrections(pho,ebRecHits); 
        TLorentzVector phoP4=TLorentzVector(pho.px(),pho.py(),pho.pz(),pho.energy());
        
        float smear = 0.0, scale = 1.0;
        float aeta = std::abs(pho.eta());
        float et = pho.energy()/cosh(aeta);

        if (iEvent.isRealData() )
        {
                
                scale = PhoCorr->ScaleCorrection(iEvent.id().run(), pho.isEB(), pho.r9(), aeta, et);
                Ecorr *= scale;
        }
        else
        {
                 // the  kNone refers to syst changes
                 // arbitrary func of run,lumi, event
                 rnd_->SetSeed(iEvent.id().run()*1000000+iEvent.luminosityBlock()*100 + iEvent.id().event()) ;
                 smear = PhoCorr->getSmearingSigma((int) iEvent.id().run(), pho.isEB(), pho.r9(), aeta, pho.energy(), 0,0);  
                 float corr = 1.0  + smear * rnd_->Gaus(0,1);
                 Ecorr *= corr;
        
        }
        

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

        /*
        chIsoRC -> push_back( _chIsoRC_);
        phoIsoRC -> push_back( _phIsoRC_ ) ;
        nhIsoRC -> push_back ( _nhIsoRC_ ) ;
        puIsoRC -> push_back ( _puIsoRC_ ) ;
        */

        if (IsExtend() ){
            rawpt->push_back(pho.pt());
            rawScEnergy->push_back(pho.superCluster()->rawEnergy());
            
            hOverE->push_back(pho.hadTowOverEm()); //pho.hadronicOverEm());
            chWorstIso->push_back( (*iso_wch)[ref] );
            // chIsoMax->push_back( ??? );
            
            e33->push_back(pho.full5x5_e3x3());
            e55->push_back(pho.full5x5_e5x5());

            sipip->push_back(pho.full5x5_showerShapeVariables().sigmaIphiIphi);
            sieip->push_back(pho.full5x5_showerShapeVariables().sigmaIetaIphi);
            r9->push_back(pho.r9());
            s4->push_back(pho.eMax()/(pho.eMax()+pho.eTop()+pho.eBottom()+pho.eLeft()+pho.eRight()));
            
            mipEnergy->push_back(pho.mipTotEnergy());

            clusterTools = new EcalClusterLazyTools(iEvent, iSetup, ebRecHits_token, eeRecHits_token);
            time->push_back(clusterTools->SuperClusterSeedTime(*pho.superCluster()));
            emax->push_back(clusterTools->eMax(*pho.superCluster()));
            e2nd->push_back(clusterTools->e2nd(*pho.superCluster()));

            // timeSpan->push_back( ??? );
            delete clusterTools;
            
            // genMatched->push_back( ??? );
        }
    }
   
    if ( int(selBits -> size()) < mMinNpho  ) return 1;

    return 0;
}

bool NeroPhotons::cutBasedPhotonId( const pat::Photon& pho, string type, bool withIso, bool withSieie)
{
// https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedPhotonIdentificationRun2#
    float hoe   = pho.hadTowOverEm();
    float sieie = pho.full5x5_sigmaIetaIeta() ;
    float chiso = pho.chargedHadronIso();
    float nhiso= pho.neutralHadronIso() ;
    float phoiso = pho.photonIso();
    //float puiso = pho.puChargedHadronIso ();
    float rho  = evt->rho;
    //
    //float aeta = fabs(pho.eta());

    // ------------ BARREL ------------
    if ( pho.isEB() )
        {
        if (not withSieie and sieie > 0.15) return false;  // pu a very loose sieie requirement
        if (not withIso and phoiso > 10 ) return false; // put  a very loose pho-iso requirement

        if ( type == "loose_50ns" ) 
            {
                if (hoe >= 0.0559  ) return false;
                if (sieie >= 0.010 and withSieie  ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_50ns") * rho >= 2.51 ) return false;// 
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_50ns")*rho >= 21.11 + 0.0065*pho.pt() ) return false;// 
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_50ns") *rho >= 3.70 + 0.0032*pho.pt() ) return false ;//  2.09 + 0.0032*pho_pt    1.60 + 0.0032*pho_pt 

                return true;
            }
        if ( type == "medium_50ns" )
            {
                if (hoe >= 0.0138 ) return false;
                if (sieie >= 0.010 and withSieie ) return false;
                if (chiso -  cutBasedPhotonIdEffArea(pho,"ch_50ns")*rho >=2.23 ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_50ns")*rho >= 8.85 + 0.0065*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_50ns") *rho >= 2.09 + 0.0032*pho.pt() ) return false ; 
                return true;
            }
        if ( type == "tight_50ns" )
            {
                if (hoe >= 0.0099 ) return false;
                if (sieie >= 0.0099 and withSieie ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_50ns") * rho >= 1.96 ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_50ns")*rho >= 4.18 + 0.0065*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_50ns") *rho >= 1.60 + 0.0032*pho.pt() ) return false ;
                return true;
            }
        // ---------------- 25ns ---------------
        if ( type == "monoph_baseline") // using spring15 loose selections, but with sieie and chiso sidebands
            {
                if (hoe >= 0.05   ) return false;
                if (sieie >= 0.015  and withSieie  ) return false;
                if (chiso - cutBasedPhotonIdEffArea(pho,"ch_25ns") * rho >= 11.0) return false;// 
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_25ns")*rho >= 1.92 + 0.014*pho.pt() + 0.000019*pho.pt()*pho.pt() ) return false;// 
                if (phoiso - cutBasedPhotonIdEffArea(pho,"pho_25ns") *rho >= 0.81 + 0.0053*pho.pt() ) return false ;//

                return true;
            }
        // selections below are CSA14; latest version used by most people is Spring15
        if ( type == "loose_25ns" ) 
            {
                if (hoe >= 0.553   ) return false;
                if (sieie >= 0.0099  and withSieie  ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_25ns") * rho >= 2.49  ) return false;// 
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_25ns")*rho >= 15.43 + 0.007*pho.pt() ) return false;// 
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_25ns") *rho >= 9.42 + 0.0033*pho.pt() ) return false ;//

                return true;
            }
        if ( type == "medium_25ns" )
            {
                if (hoe >= 0.058  ) return false;
                if (sieie >= 0.0099  and withSieie ) return false;
                if (chiso -  cutBasedPhotonIdEffArea(pho,"ch_25ns")*rho >= 1.91 ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_25ns")*rho >= 4.66 + 0.007*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_25ns") *rho >= 4.29 + 0.0033*pho.pt() ) return false ; 
                return true;
            }
        if ( type == "tight_25ns" )
            {
                if (hoe >= 0.0019 ) return false;
                if (sieie >= 0.0099 and withSieie ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_25ns") * rho >= 1.61 ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_25ns")*rho >= 3.98 + 0.007*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_25ns") *rho >= 3.01 + 0.0033*pho.pt() ) return false ;
                return true;
            }
        }
    // ------------ ENDCAP ------------
    else if (pho.isEE()){
        if (not withSieie and sieie > 0.035) return false;  // pu a very loose sieie requirement
        if (not withIso and phoiso > 20 ) return false; // put  a very loose pho-iso requirement
        // ---------------- 50ns --------------- 
        if ( type == "loose_50ns" ) 
            {
                if (hoe >= 0.049  ) return false;
                if (sieie >= 0.0321 and withSieie  ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_50ns") * rho >= 0.98 ) return false;// 
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_50ns")*rho >= 23.67 + 0.0116*pho.pt() ) return false;// 
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_50ns") *rho >= 6.57 + 0.0095 *pho.pt() ) return false ;//  2.09 + 0.0032*pho_pt    1.60 + 0.0032*pho_pt 

                return true;
            }
        if ( type == "medium_50ns" )
            {
                if (hoe >= 0.016 ) return false;
                if (sieie >= 0.0275 and withSieie ) return false;
                if (chiso -  cutBasedPhotonIdEffArea(pho,"ch_50ns")*rho >= 0.89 ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_50ns")*rho >= 14.48 + 0.0116*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_50ns") *rho >= 2.88 + 0.0095*pho.pt() ) return false ; 
                return true;
            }
        if ( type == "tight_50ns" )
            {
                if (hoe >= 0.016  ) return false;
                if (sieie >= 0.0275 and withSieie ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_50ns") * rho >= .67 ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_50ns")*rho >= 2.95 + 0.0116*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_50ns") *rho >= 1.56 + 0.0095*pho.pt() ) return false ;
                return true;
            }
        // ---------------- 25ns ---------------  TODO
        if ( type == "loose_25ns" ) 
            {
                if (hoe >= 0.062    ) return false;
                if (sieie >=    0.0284   and withSieie  ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_25ns") * rho >= 1.04   ) return false;// 
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_25ns")*rho >= 19.71 + 0.0129*pho.pt() ) return false;// 
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_25ns") *rho >= 11.88 + 0.0108*pho.pt() ) return false ;//

                return true;
            }
        if ( type == "medium_25ns" )
            {
                if (hoe >= 0.020  ) return false;
                if (sieie >= 0.0268   and withSieie ) return false;
                if (chiso -  cutBasedPhotonIdEffArea(pho,"ch_25ns")*rho >= 0.82  ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_25ns")*rho >= 14.65 + 0.0129*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_25ns") *rho >= 4.06 + 0.0108*pho.pt() ) return false ; 
                return true;
            }
        if ( type == "tight_25ns" )
            {
                if (hoe >= 0.016  ) return false;
                if (sieie >= 0.0263  and withSieie ) return false;
                if ( chiso - cutBasedPhotonIdEffArea(pho,"ch_25ns") * rho >= 0.69  ) return false;
                if (nhiso - cutBasedPhotonIdEffArea(pho,"nh_25ns")*rho >= 4.52+ 0.0129*pho.pt() ) return false;
                if (withIso and phoiso - cutBasedPhotonIdEffArea(pho,"pho_25ns") *rho >= 3.61 + 0.0108*pho.pt() ) return false ;
                return true;
            }

        }
    return false;

}

float NeroPhotons::cutBasedPhotonIdEffArea( const pat::Photon & pho,string type){
    float aeta = fabs( pho.eta() ) ;
    // --------------- 50 ns ----------------
    if (type == "ch_50ns")
        {
        if (aeta < 1.0   ) return 0.0064 ;
        if (aeta < 1.479 ) return 0.0055 ;
        if (aeta < 2.0   ) return 0.0055 ;
        if (aeta < 2.2   ) return 0.0049 ;
        if (aeta < 2.3   ) return 0.0047 ;
        if (aeta < 2.4   ) return 0.0037 ;
                           return 0.0017 ;
        }
    if (type == "nh_50ns")
        {
        if (aeta < 1.0   ) return  0.0129;
        if (aeta < 1.479 ) return  0.0245;
        if (aeta < 2.0   ) return  0.0125;
        if (aeta < 2.2   ) return  0.0022;
        if (aeta < 2.3   ) return  0.0039;
        if (aeta < 2.4   ) return  0.0033;
                           return  0.1147;
        }
    if (type == "pho_50ns")
        {
        if (aeta < 1.0   ) return  0.1020;
        if (aeta < 1.479 ) return  0.0889;
        if (aeta < 2.0   ) return  0.0423;
        if (aeta < 2.2   ) return  0.0441;
        if (aeta < 2.3   ) return  0.0559;
        if (aeta < 2.4   ) return  0.0653;
                           return  0.0862;
        }
    // --------------- 25 ns ----------------
    if (type == "ch_25ns")
        {
        if (aeta < 1.0   ) return 0.0080;
        if (aeta < 1.479 ) return 0.0079;
        if (aeta < 2.0   ) return 0.0080;
        if (aeta < 2.2   ) return 0.0048;
        if (aeta < 2.3   ) return 0.0029;
        if (aeta < 2.4   ) return 0.0036;
                           return 0.0016;
        }
    if (type == "nh_25ns")
        {
        if (aeta < 1.0   ) return 0.0126 ;
        if (aeta < 1.479 ) return 0.0237 ;
        if (aeta < 2.0   ) return 0      ;
        if (aeta < 2.2   ) return 0      ;
        if (aeta < 2.3   ) return 0      ;
        if (aeta < 2.4   ) return 0      ;
                           return 0.0769 ;
        }
    if (type == "pho_25ns")
        {
        if (aeta < 1.0   ) return 0.0982;
        if (aeta < 1.479 ) return 0.0857;
        if (aeta < 2.0   ) return 0.0484;
        if (aeta < 2.2   ) return 0.0668;
        if (aeta < 2.3   ) return 0.0868;
        if (aeta < 2.4   ) return 0.0982;
                           return 0.1337;
        }
    
    return -999.;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
