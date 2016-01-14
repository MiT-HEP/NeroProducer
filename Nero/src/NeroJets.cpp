#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

//JES
#include "FWCore/Framework/interface/ESHandle.h"


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
    mMinPt = 20.;
    mMinNjets = 0;
    mMinEta = 2.5;
    mMinId = "loose";
    pf = NULL;
    // JES
    isJecUncSet_= false;
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

    // -- need to init JES here, where there is the iSetup
    InitJes(iSetup);

    int ijetRef = -1;
    for (const pat::Jet& j : *handle)
    {
        ijetRef++;

        if (j.pt() < mMinPt ) continue;
        if ( fabs(j.eta()) > mMinEta ) continue;
        if ( !JetId(j,mMinId) ) continue;

        //0 < |eta| < 2.5: PUID > -0.63
        if ( !(j.userFloat("pileupJetId:fullDiscriminant") > -0.63) ) continue;

        // GET  ValueMaps
        edm::RefToBase<pat::Jet> jetRef(edm::Ref<pat::JetCollection>(handle, ijetRef) );
        float qgLikelihood = (*qg_handle)[jetRef];
    
    
        // Generator-level Info [Charged-H specific]
        int jetMatchedPartonPdgId_I = 0;
        int motherPdgId_I = 0;
        int grMotherPdgId_I = 0;
        int jetFlavour_I = 0;

        if( not iEvent.isRealData() ){           
            const reco::Candidate * jetGen = j.genParton();
            const reco::Candidate * jetMother = getMother(jetGen);
            const reco::Candidate * jetGrMother = getMother(jetMother);
            if(!(jetGen == NULL)){jetMatchedPartonPdgId_I = jetGen->pdgId();}
            if(!(jetMother == 0)){motherPdgId_I = jetMother->pdgId();}
            if(!(jetGrMother == 0)){grMotherPdgId_I = jetGrMother->pdgId();}
            jetFlavour_I = j.partonFlavour();
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
        
        //JES UNCERTAINTY
        jecUnc_->setJetEta(j.eta());
        jecUnc_->setJetPt(j.pt()) ;//https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections  corrected jet pt
        float jecunc = jecUnc_->getUncertainty(true);


        // Fill output object	
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
        rawPt  -> push_back (j.pt()*j.jecFactor("Uncorrected"));
        puId   -> push_back (j.userFloat("pileupJetId:fullDiscriminant") );
        bDiscr -> push_back( j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") );
        bDiscrLegacy -> push_back( j.bDiscriminator("combinedSecondaryVertexBJetTags") );
        qgl     -> push_back( qgLikelihood );
        flavour -> push_back( jetFlavour_I );
        matchedPartonPdgId -> push_back( jetMatchedPartonPdgId_I );
        motherPdgId -> push_back( motherPdgId_I );
        grMotherPdgId -> push_back( grMotherPdgId_I );
        unsigned bits=0;
        bits |= JetId(j,"monojet") * mjId;
        bits |= JetId(j,"monojetloose") * mjIdLoose;
        bits |= JetId(j,"loose") * JetLoose;
        bits |= JetId(j,"tight") * JetTight;
        bits |= JetId(j,"monojet2015") * mjId2015;

        selBits -> push_back( bits);
        Q      -> push_back( charge/charge_den);
        QnoPU  -> push_back( charge_nopu/charge_nopu_den);
        unc -> push_back( jecunc );
    }

    if ( int(rawPt -> size()) < mMinNjets ) return 1;

    return 0;
}

bool NeroJets::JetId(const pat::Jet &j, std::string id)
{
    // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID
    //                              Loose -- Tight Jet ID
    // --- Number of Constituents   > 1     > 1
    // --- Neutral Hadron Fraction  < 0.99  < 0.90
    // --- Neutral EM Fraction      < 0.99  < 0.90
    // --- Muon Fraction    < 0.8   < 0.8
    // --- And for -2.4 <= eta <= 2.4 in addition apply
    // --- Charged Hadron Fraction  > 0     > 0
    // --- Charged Multiplicity     > 0     > 0
    // --- Charged EM Fraction      < 0.99  < 0.90 

    bool jetid = false;

    float NHF    = j.neutralHadronEnergyFraction();
    float NEMF   = j.neutralEmEnergyFraction();
    float CHF    = j.chargedHadronEnergyFraction();
    //float MUF    = j.muonEnergyFraction();
    float CEMF   = j.chargedEmEnergyFraction();
    int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
    int CHM      = j.chargedMultiplicity();
    int NumNeutralParticle =j.neutralMultiplicity(); 
    float eta = j.eta();

    //tightLepVetoJetID = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || fabs(eta)>2.4) && fabs(eta)<=3.0 

    if (id=="loose" || id=="monojet" || id=="monojetloose")
    {
        //jetid = (NHF<0.99 && NEMF<0.99 && NumConst>1 && MUF<0.8) && ((fabs(j.eta())<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(j.eta())>2.4);
        jetid = (NHF<0.99 && NEMF<0.99 && NumConst>1) && ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(eta)>2.4) && fabs(eta)<=3.0;
        jetid = jetid || (NEMF<0.90 && NumNeutralParticle>10 && fabs(eta)>3.0);
    }

    if (id=="tight")
    {
        //jetid = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((fabs(j.eta())<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || fabs(j.eta())>2.4);
        jetid = (NHF<0.90 && NEMF<0.90 && NumConst>1) && ((fabs(eta)<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(eta)>2.4) && fabs(eta)<=3.0;
        jetid = jetid || (NEMF<0.90 && NumNeutralParticle>10 && fabs(eta)>3.0 );
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
