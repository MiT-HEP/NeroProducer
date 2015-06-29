#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"


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

NeroJets::NeroJets() : BareJets()
{
    mMinPt = 20.;
    mMinNjets = 0;
    mMinEta = 2.5;
    mMinId = "loose";
}

NeroJets::~NeroJets(){
}

int NeroJets::analyze(const edm::Event& iEvent){

    if ( mOnlyMc  ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    iEvent.getByToken(qg_token,qg_handle);

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
        
        // Fill output object	
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
        rawPt  -> push_back (j.pt()*j.jecFactor("Uncorrected"));
        puId   -> push_back (j.userFloat("pileupJetId:fullDiscriminant") );
        bDiscr -> push_back( j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") );
        qgl     -> push_back( qgLikelihood );
        flavour -> push_back( jetFlavour_I );
        matchedPartonPdgId -> push_back( jetMatchedPartonPdgId_I );
        motherPdgId -> push_back( motherPdgId_I );
        grMotherPdgId -> push_back( grMotherPdgId_I );
        mjId       -> push_back( JetId(j,"monojet"));
        mjId_loose -> push_back( JetId(j,"monojetloose"));
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
    float MUF    = j.muonEnergyFraction();
    float CEMF   = j.chargedEmEnergyFraction();
    int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
    int CHM      = j.chargedMultiplicity();

    if (id=="loose")
        jetid = (NHF<0.99 && NEMF<0.99 && NumConst>1 && MUF<0.8) && ((fabs(j.eta())<=2.4 && CHF>0 && CHM>0 && CEMF<0.99) || fabs(j.eta())>2.4);

    if (id=="tight")
        jetid = (NHF<0.90 && NEMF<0.90 && NumConst>1 && MUF<0.8) && ((fabs(j.eta())<=2.4 && CHF>0 && CHM>0 && CEMF<0.90) || fabs(j.eta())>2.4);

    if (id=="monojet")
        jetid = (CHF > 0.2 && NHF < 0.7 && NEMF < 0.7);

    if (id=="monojetloose")
        jetid = (NHF < 0.7 && NEMF < 0.9);

    return jetid;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
