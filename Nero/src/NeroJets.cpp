#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

//JES
#include "FWCore/Framework/interface/ESHandle.h"
#include "JetMETCorrections/Modules/interface/JetResolution.h"

#include <ctime>

/********** FUNCTIONS **********/
int ReMatch( const pat::Jet *jet , const edm::View<reco::GenParticle> *genp ,float dR=0.4)
{
    int pdg=0;
    float ptMax=0.0;
    for (const auto& gp : *genp)
    {
        int apdg=std::abs(gp.pdgId());
        // consider only udscb g
        if ( not (( apdg>=1 and apdg<6) or apdg==21)) continue;
        // dR
        if ( not (reco::deltaR(gp,*jet) <dR) ) continue;
        // ptMax
        if ( ptMax >= gp.pt() ) continue;
        // update tmp values
        ptMax=gp.pt();
        pdg=gp.pdgId();
    }

    return pdg;
}


/********** FUNCTIONS **********/
const reco::Candidate * getMother(const reco::Candidate * part){
    if(part==NULL){return NULL;}
    if(part->numberOfMothers()<1){return NULL;}
    else{
        int partPdgId = part->pdgId(); 
        const reco::Candidate * partMother = part->mother(0);
        int partMotherPdgId = partMother->pdgId();
        if(partMotherPdgId == partPdgId){return getMother(partMother);}
        else{return partMother;}
    }
}

NeroJets::NeroJets() : 
    NeroCollection(),
    BareJets()
{
    mMinPt = 15.;
    mMinNjets = 0;
    mMinEta = 2.5;
    mMinId = "loose";
    pf = NULL;
    // JES
    isJecUncSet_= false;
    //
    rnd_=new TRandom3();
    rnd_->SetSeed((unsigned)time(NULL));
}

NeroJets::~NeroJets(){
}

int NeroJets::analyze(const edm::Event& iEvent, const edm::EventSetup &iSetup){

    if ( mOnlyMc  ) return 0;


    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    iEvent.getByToken(qg_token,qg_handle);

    if ( not handle.isValid() ) cout<<"[NeroJets]::[analyze]::[ERROR] handle is not valid"<<endl;
    if ( not qg_handle.isValid() ) cout<<"[NeroJets]::[analyze]::[ERROR] qg_handle is not valid"<<endl;

    iEvent.getByToken(qg_token_Mult,qg_handle_Mult);
    iEvent.getByToken(qg_token_Axis2,qg_handle_Axis2);
    iEvent.getByToken(qg_token_PtD,qg_handle_PtD);
    iEvent.getByToken(qg_token_Axis1,qg_handle_Axis1);
    iEvent.getByToken(qg_token_cmult,qg_handle_cmult);
    iEvent.getByToken(qg_token_nmult,qg_handle_nmult);
    iEvent.getByToken(qg_token_pt_dr_log,qg_handle_pt_dr_log);

    map<string,edm::Handle<edm::ValueMap<float> > >  qg_handle_f ;
    map<string,edm::Handle<edm::ValueMap<int> > >  qg_handle_i ;
    for( const auto& dR: dRToProduce){
        iEvent.getByToken( qg_dR_tokens_f[Form("axis2-dR0p%03.0f",dR*1000)] , qg_handle_f[Form("axis2-dR0p%03.0f",dR*1000)]) ;
        iEvent.getByToken( qg_dR_tokens_f[Form("Gen-axis2-dR0p%03.0f",dR*1000)] , qg_handle_f[Form("Gen-axis2-dR0p%03.0f",dR*1000)]) ;
        iEvent.getByToken( qg_dR_tokens_f[Form("axis1-dR0p%03.0f",dR*1000)] , qg_handle_f[Form("axis1-dR0p%03.0f",dR*1000)]) ;
        iEvent.getByToken( qg_dR_tokens_f[Form("Gen-axis1-dR0p%03.0f",dR*1000)] , qg_handle_f[Form("Gen-axis1-dR0p%03.0f",dR*1000)]) ;
        iEvent.getByToken( qg_dR_tokens_f[Form("ptD-dR0p%03.0f",dR*1000)] , qg_handle_f[Form("ptD-dR0p%03.0f",dR*1000)]) ;
        iEvent.getByToken( qg_dR_tokens_f[Form("Gen-ptD-dR0p%03.0f",dR*1000)] , qg_handle_f[Form("Gen-ptD-dR0p%03.0f",dR*1000)]) ;

        iEvent.getByToken( qg_dR_tokens_i[Form("mult-dR0p%03.0f",dR*1000)] , qg_handle_i[Form("mult-dR0p%03.0f",dR*1000)]) ;
        iEvent.getByToken( qg_dR_tokens_i[Form("Gen-mult-dR0p%03.0f",dR*1000)] , qg_handle_i[Form("Gen-mult-dR0p%03.0f",dR*1000)]) ;
    }

    // -- need to init JES here, where there is the iSetup
    InitJes(iSetup);

    int ijetRef = -1;
    for (const pat::Jet& j : *handle)
    {
        ijetRef++;

        if (j.pt() < mMinPt ) continue;
        if ( fabs(j.eta()) > mMinEta ) continue;
        if ( !JetId(j,mMinId) ) continue;

        // GET  ValueMaps
        edm::RefToBase<pat::Jet> jetRef(edm::Ref<pat::JetCollection>(handle, ijetRef) );
        float qgLikelihood = (*qg_handle)[jetRef];

        for(const auto & dR : dRToProduce)
        {
            qglMult_dR[Form("0p%03.0f",dR*1000)]	    ->push_back((*qg_handle_i[Form("mult-dR0p%03.0f",dR*1000)])[jetRef]);
            qglPtD_dR[Form("0p%03.0f",dR*1000)]	    ->push_back((*qg_handle_f[Form("ptD-dR0p%03.0f",dR*1000)])[jetRef]);
            qglAxis2_dR[Form("0p%03.0f",dR*1000)]	    ->push_back((*qg_handle_f[Form("axis2-dR0p%03.0f",dR*1000)])[jetRef]);
            qglAxis1_dR[Form("0p%03.0f",dR*1000)]	    ->push_back((*qg_handle_f[Form("axis1-dR0p%03.0f",dR*1000)])[jetRef]);
        }

        if (not iEvent.isRealData())
        {

            edm::Handle<reco::GenJetCollection> gen_handle;
            iEvent.getByToken(gen_token, gen_handle);
            int igenjetRef = -1;
            bool matched=false;
            for(const auto& gj: *gen_handle)
            {
                igenjetRef+=1;
                if (reco::deltaR(gj,j)<0.4 ) {matched=true;break;}
            }
            edm::RefToBase<reco::GenJet> genjetRef(edm::Ref<reco::GenJetCollection>(gen_handle, igenjetRef) );
            if (matched){
                for(const auto & dR : dRToProduce)
                {
                    qglGenMult_dR[Form("0p%03.0f",dR*1000)]	->push_back((*qg_handle_i[Form("Gen-mult-dR0p%03.0f",dR*1000)])[genjetRef]);
                    qglGenPtD_dR[Form("0p%03.0f",dR*1000)]	->push_back((*qg_handle_f[Form("Gen-ptD-dR0p%03.0f",dR*1000)])[genjetRef]);
                    qglGenAxis2_dR[Form("0p%03.0f",dR*1000)]	->push_back((*qg_handle_f[Form("Gen-axis2-dR0p%03.0f",dR*1000)])[genjetRef]);
                    qglGenAxis1_dR[Form("0p%03.0f",dR*1000)]	->push_back((*qg_handle_f[Form("Gen-axis1-dR0p%03.0f",dR*1000)])[genjetRef]);
                }
            }else{
                for(const auto & dR : dRToProduce)
                {
                    qglGenMult_dR[Form("0p%03.0f",dR*1000)]	->push_back(-999);
                    qglGenPtD_dR[Form("0p%03.0f",dR*1000)]	->push_back(-999);
                    qglGenAxis2_dR[Form("0p%03.0f",dR*1000)]	->push_back(-999);
                    qglGenAxis1_dR[Form("0p%03.0f",dR*1000)]	->push_back(-999);
                }
            }
        }

        // Generator-level Info [Charged-H specific]
        int jetMatchedPartonPdgId_I = 0;
        int motherPdgId_I = 0;
        int grMotherPdgId_I = 0;
        int jetFlavour_I = 0;
        int hadFlavour_I = 0;

        if( not iEvent.isRealData() ){           
            const reco::Candidate * jetGen = j.genParton();
            const reco::Candidate * jetMother = getMother(jetGen);
            const reco::Candidate * jetGrMother = getMother(jetMother);
            if(!(jetGen == NULL)){jetMatchedPartonPdgId_I = jetGen->pdgId();}
            if(!(jetMother == 0)){motherPdgId_I = jetMother->pdgId();}
            if(!(jetGrMother == 0)){grMotherPdgId_I = jetGrMother->pdgId();}
            //            jetFlavour_I = j.jetFlavor();
            hadFlavour_I = j.hadronFlavour();
            
            jetFlavour_I = ReMatch( &j, &*mc->pruned_handle );
        }

        float charge =  0.;
        float charge_den =  0.;
        float charge_nopu = 0.;
        float charge_nopu_den = 0.;

        // compute jet charge
        for( size_t idx =0; idx < j.numberOfDaughters() ; ++idx)
        {
            // fromPV is available only in PackedCandidates and not in reco::PFCandidate
            pat::PackedCandidate *cand = ( pat::PackedCandidate* ) j.daughter(idx) ; 

            bool isFromOtherVtx= false;
            // 0 is the primary vertex
            for(size_t iVtx=0;iVtx < vtx->handle->size(); ++iVtx)
            {

                if ( int(iVtx) == vtx->firstGoodVertexIdx ) continue;

                if (cand->fromPV(iVtx)>1) isFromOtherVtx = true; // 0 noPV, 1 PVLoose, 2 PVTight
            }

            if (cand->charge() !=0 ) {  
                charge     += cand->charge() * ( j.px()*cand->px() + j.py()*cand->py() + j.pz()*cand->pz()  ) ;
                charge_den +=                  ( j.px()*cand->px() + j.py()*cand->py() + j.pz()*cand->pz()  ) ;
            }
            if (cand->charge() != 0 and not isFromOtherVtx)
            {
                charge_nopu     += cand->charge() * ( j.px()*cand->px() + j.py()*cand->py() + j.pz()*cand->pz()  ) ;
                charge_nopu_den +=                  ( j.px()*cand->px() + j.py()*cand->py() + j.pz()*cand->pz()  ) ;

            }
        }

        // Fill Output  --- part I
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
        refPt->push_back( j.pt() );

        //JES UNCERTAINTY
        jecUnc_->setJetEta(j.eta());
        jecUnc_->setJetPt(j.pt()) ;//https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections  corrected jet pt
        float jecunc = jecUnc_->getUncertainty(true);

        // JER
        if (not iEvent.isRealData())
        {
            // https://github.com/blinkseb/cmssw/blob/jer_fix_76x/JetMETCorrections/Modules/plugins/JetResolutionDemo.cc#L74
            /* this is from db
            JME::JetResolution resolution = JME::JetResolution::get(iSetup, "AK4PFchs_pt");
            JME::JetResolutionScaleFactor resolution_sf = JME::JetResolutionScaleFactor::get(iSetup, "AK4PFchs");
            */
            /* this is from text files Spring16_25nsV10_MC_PtResolution_AK8PFchs.txt  Spring16_25nsV10_MC_SF_AK4PFchs.txt */
            JME::JetResolution resolution = JME::JetResolution("jer/Spring16_25nsV10_MC_PtResolution_AK8PFchs.txt");
            JME::JetResolutionScaleFactor resolution_sf = JME::JetResolutionScaleFactor("jer/Spring16_25nsV10_MC_SF_AK4PFchs.txt");
            JME::JetParameters jpar;
            jpar.setJetPt( j.pt()).setJetEta( j.eta() ).setRho( evt->rho)  ;
            float res = resolution.getResolution(jpar);
            float sf = resolution_sf.getScaleFactor(jpar);
            float sf_up = resolution_sf.getScaleFactor(jpar, Variation::UP);
            float sf_down = resolution_sf.getScaleFactor(jpar, Variation::DOWN);

            // https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution#Smearing_procedures
            const reco::GenJet* genjet= j.genJet() ;
            float newpt = j.pt();
            float oldpt = j.pt();
            float newptUp = j.pt();
            float newptDown = j.pt();

            //
            // -- if (genjet==NULL) cout <<"DEBUG: NO GENJET"<<endl;
            // -- else cout <<"DEBUG: YES GENJET"<<"DR="<<reco::deltaR(*genjet,j) <<" DPT="<<fabs(j.pt()-genjet->pt())<< " 3Sig="<<3*res*genjet->pt()<<endl;

            // additional matching dR(reco jet, gen jet)<Rcone/2 and dpt=abs(pT-pTgen)<3*sigma_MC
            if (genjet!=NULL and reco::deltaR(*genjet,j) <0.2 and fabs(j.pt()-genjet->pt())<3*res*genjet->pt()) { //scaling
                float genpt=genjet->pt();
                newpt     = std::max(float(0.),float(genpt+sf*(j.pt()-genpt)      ));
                newptUp   = std::max(float(0.),float(genpt+sf_up*(j.pt()-genpt)   ));
                newptDown = std::max(float(0.),float(genpt+sf_down*(j.pt()-genpt) ));
            }
            else{ // smearing
                float sigma=TMath::Sqrt( sf*sf -1) * res;
                float sigmaUp=TMath::Sqrt( sf_up*sf_up -1) * res;
                float sigmaDown=TMath::Sqrt( sf_down*sf_down -1) * res;
                float s = rnd_->Gaus(0,1);
                newpt = s * sigma + oldpt;
                newptUp = s * sigmaUp + oldpt;
                newptDown = s * sigmaDown +oldpt;
            }
            ptResUncCentral->push_back(newpt);
            ptResUncUp->push_back(newptUp);
            ptResUncDown->push_back(newptDown) ;
        } // end is MC for JER

        // Fill output object	
        rawPt  -> push_back (j.pt()*j.jecFactor("Uncorrected"));
        puId   -> push_back (j.userFloat("pileupJetId:fullDiscriminant") );
        bDiscr -> push_back( j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") );
        bMva -> push_back( j.bDiscriminator("pfCombinedMVAV2BJetTags ") );
        //bDiscrLegacy -> push_back( j.bDiscriminator("combinedSecondaryVertexBJetTags") );
        qgl     -> push_back( qgLikelihood );

        //DEEP
        deepB -> push_back( j.bDiscriminator("pfDeepCSVJetTags:probb") );
        deepBB -> push_back( j.bDiscriminator("pfDeepCSVJetTags:probbb") );
        deepC -> push_back( j.bDiscriminator("pfDeepCSVJetTags:probc") );
        deepL -> push_back( j.bDiscriminator("pfDeepCSVJetTags:probudsg") );

        // if the token was not valid, this will simply not be filled
        if (qg_handle_Mult.isValid()) qglMult->push_back(  (*qg_handle_Mult)[jetRef] );
        if (qg_handle_PtD.isValid()) qglPtD->push_back(  (*qg_handle_PtD)[jetRef] );
        if (qg_handle_Axis2.isValid()) qglAxis2->push_back(  (*qg_handle_Axis2)[jetRef] );
        if (qg_handle_Axis1.isValid()) qglAxis1->push_back(  (*qg_handle_Axis1)[jetRef] );
        if (qg_handle_cmult.isValid()) qglCMult->push_back(  (*qg_handle_cmult)[jetRef] );
        if (qg_handle_nmult.isValid()) qglNMult->push_back(  (*qg_handle_nmult)[jetRef] );
        if (qg_handle_pt_dr_log.isValid()) qglPtDrLog->push_back(  (*qg_handle_pt_dr_log)[jetRef] );


        // saving the energy fractions
        chef -> push_back(j.chargedHadronEnergyFraction());
        nhef -> push_back(j.neutralHadronEnergyFraction());
        nemf -> push_back(j.neutralEmEnergyFraction());
        cemf -> push_back(j.chargedEmEnergyFraction());

        hadFlavour -> push_back( hadFlavour_I );
        flavour -> push_back( jetFlavour_I );
        matchedPartonPdgId -> push_back( jetMatchedPartonPdgId_I );
        motherPdgId -> push_back( motherPdgId_I );
        grMotherPdgId -> push_back( grMotherPdgId_I );
        unsigned bits=0;
        bits |=  (1 * JetBaseline);
        bits |= JetId(j,"monojet") * mjId;
        bits |= JetId(j,"monojetloose") * mjIdLoose;
        bits |= JetId(j,"monojet2015") * mjId2015;
        bits |= JetId(j,"loose") * JetLoose;
        bits |= JetId(j,"tight") * JetTight;

        selBits -> push_back( bits);
        Q      -> push_back( charge/charge_den);
        unc -> push_back( jecunc );
    }

    if ( int(rawPt -> size()) < mMinNjets ) return 1;

    return 0;
}

bool NeroJets::JetId(const pat::Jet &j, std::string id)
{
    // 11/09/2016 r95
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID

    bool jetid = false;

    float NHF    = j.neutralHadronEnergyFraction();
    float NEMF   = j.neutralEmEnergyFraction();
    float CHF    = j.chargedHadronEnergyFraction();
    float MUF    = j.muonEnergyFraction();
    float CEMF   = j.chargedEmEnergyFraction();
    int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
    int CHM      = j.chargedMultiplicity();
    int NumNeutralParticle =j.neutralMultiplicity(); 
    float eta = j.eta();

    if (id=="none") return true;

    // POG JetID loose, tight, tightLepVeto
    if (id=="loose")
    {
        jetid=(NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=2.7 ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>2 && abs(eta)>2.7 && abs(eta)<=3.0 ) ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 ) ;
    }

    else if (id=="tight")
    {
        jetid = (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=2.7 ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>2 && abs(eta)>2.7 && abs(eta)<=3.0 ) ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 ) ;
    }

    else if (id=="tightLepVeto")
    {
        jetid =  (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || abs(eta)>2.4) && abs(eta)<=2.7 ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>2 && abs(eta)>2.7 && abs(eta)<=3.0 ) ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 ) ;
    }

    else if ( id=="monojet" || id=="monojetloose" || id=="monojet2015")
    {
        // loose + monojet ids
        jetid=(NHF<0.99 && NEMF<0.99 && NumConst>1) && ((abs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || abs(eta)>2.4) && abs(eta)<=2.7 ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>2 && abs(eta)>2.7 && abs(eta)<=3.0 ) ;
        jetid = jetid or (NEMF<0.90 && NumNeutralParticle>10 && abs(eta)>3.0 ) ;
    }

    if (id=="monojet")
        jetid = jetid && (CHF > 0.2 && NHF < 0.7 && NEMF < 0.7);

    if (id=="monojetloose")
        jetid = jetid && (NHF < 0.7 && NEMF < 0.9);

    if (id=="monojet2015")
        jetid = jetid && (NHF < 0.8 && CHF > 0.1);

    return jetid;
}

// JES
void NeroJets::InitJes(const edm::EventSetup& iSetup){

    if (isJecUncSet_) return;

    string payload = "AK4PFchs";

    cout <<"[NeroJets]::[InitJes]::[INFO] Init Jes with Payload"<<payload<<endl;

    edm::ESHandle<JetCorrectorParametersCollection> JetCorParColl;
    iSetup.get<JetCorrectionsRecord>().get(payload ,JetCorParColl); 
    JetCorrectorParameters const& par = (*JetCorParColl)["Uncertainty"];
    jecUnc_ = new JetCorrectionUncertainty(par);

    isJecUncSet_ = true;
    return;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
