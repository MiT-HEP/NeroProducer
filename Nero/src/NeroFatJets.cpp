#include "NeroProducer/Nero/interface/NeroFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp" // JetId
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

NeroFatJets::NeroFatJets(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
        BareFatJets(),
        mRunJEC(false),
        mMinId("loose"),
        jetRadius(0.8)
{
    token = cc.consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("AK8Jet"));
    rho_token = cc.consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
    vertex_token = cc.consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
    mMinPt = iConfig.getParameter<double>("minAK8Pt");
    mMaxEta = iConfig.getParameter<double>("minAK8Eta");
    mMinId = iConfig.getParameter<string>("minAK8Id");
    cachedPrefix = iConfig.getParameter<string>("AK8Name");
    jetRadius = 0.8;
    subjets_token = cc.mayConsume<reco::PFJetCollection>(edm::InputTag("PFJetsSoftDrop"+ cachedPrefix ,"SubJets"));
    btags_token = cc.mayConsume<reco::JetTagCollection>(edm::InputTag(cachedPrefix + "PFCombinedInclusiveSecondaryVertexV2BJetTags") ) ;
    jecBasePath= iConfig.getParameter<string>("AK8JEC");
    jecBasePathMC= iConfig.getParameter<string>("AK8JECMC");
}

NeroFatJets::~NeroFatJets(){
}

void NeroFatJets::init()
{
  BareFatJets::init();

}

int NeroFatJets::analyze(const edm::Event& iEvent){

    if ( mMinId == "none" ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    //Vertex
    iEvent.getByToken(vertex_token,vertex_handle);
    //Rho
    iEvent.getByToken(rho_token,rho_handle);

    // this jet collection is straight from miniAOD - skip all the fancy stuff
    if ( not handle.isValid() ) cout<<"[NeroFatJets]::[analyze]::[ERROR] handle is not valid"<<endl;

    int ijetRef = -1;
    int nsubjet = 0;
    for (const pat::Jet& j : *handle)
    {
        ijetRef++;
        //cout<<"[NeroFatJets]::[DEBUG] -> Doing FAT JET "<<ijetRef<<endl;
        if (j.pt() < mMinPt || fabs(j.eta()) > mMaxEta) continue;
        
        // JET ID
        if ( !j.isPFJet() ) continue;  // Jet Id breaks for non pf
        if ( !NeroJets::JetId(j,mMinId) ) continue;

        //cout<<"[NeroFatJets]::[DEBUG] -> FatJet Fill "<<ijetRef<<endl;
        // Fill output object	
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
        
        rawPt -> push_back (j.pt()*j.jecFactor("Uncorrected"));

        flavour -> push_back( j.partonFlavour() );

        tau1 -> push_back(j.userFloat("NjettinessAK8Puppi:tau1"));
        tau2 -> push_back(j.userFloat("NjettinessAK8Puppi:tau2"));
        tau3 -> push_back(j.userFloat("NjettinessAK8Puppi:tau3"));

        softdropMass->push_back(j.groomedMass("SoftDropPuppi"));

        TvsQCD->push_back(j.bDiscriminator("pfDeepBoostedDiscriminatorsJetTags:TvsQCD"));
        WvsQCD->push_back(j.bDiscriminator("pfDeepBoostedDiscriminatorsJetTags:WvsQCD"));
        ZvsQCD->push_back(j.bDiscriminator("pfDeepBoostedDiscriminatorsJetTags:ZvsQCD"));
        ZbbvsQCD->push_back(j.bDiscriminator("pfDeepBoostedDiscriminatorsJetTags:ZbbvsQCD"));

        TvsQCDMD->push_back(j.bDiscriminator("pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:TvsQCD"));
        WvsQCDMD->push_back(j.bDiscriminator("pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:WvsQCD"));
        ZHbbvsQCDMD->push_back(j.bDiscriminator("pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHbbvsQCD"));
        ZHccvsQCDMD->push_back(j.bDiscriminator("pfMassDecorrelatedDeepBoostedDiscriminatorsJetTags:ZHccvsQCD"));

        hbb -> push_back( j.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags") ) ;

        // Using SoftDropPuppi Subjets in 94X
        unsigned int nsubjetThisJet=0;
        firstSubjet->push_back(nsubjet);
        auto &Subjets = j.subjets("SoftDropPuppi");
        for ( auto const & i : Subjets ) {
            new ( (*subjet)[nsubjet]) TLorentzVector(i->px(), i->py(), i->pz(), i->energy());
            subjet_btag->push_back(i->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
            nsubjetThisJet++;
            nsubjet++;
        }
        nSubjets->push_back(nsubjetThisJet);
    }
        
    return 0;
}

NEROREGISTER(NeroFatJets);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
