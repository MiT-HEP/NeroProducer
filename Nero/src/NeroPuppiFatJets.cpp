#include "NeroProducer/Nero/interface/NeroPuppiFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroPuppiJets.hpp" // JetId
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"
#include <cstdlib>
#include <string>


NeroPuppiFatJets::NeroPuppiFatJets() : 
    NeroCollection(),
    BarePuppiFatJets(),
    mMinId("loose"),
    jetRadius(0.8),
    mMCJetCorrector(0),
    mDataJetCorrector(0)
{
}

NeroPuppiFatJets::~NeroPuppiFatJets()
{
    BareFunctions::Delete(mMCJetCorrector);
    BareFunctions::Delete(mDataJetCorrector);
}

void NeroPuppiFatJets::init()
{
    BarePuppiFatJets::init();
    // set up jet energy corrections
    std::string jecDir = "jec/";

    std::vector<JetCorrectorParameters> mcParams;
    mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L1FastJet_AK8PFPuppi.txt"));
    mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L2Relative_AK8PFPuppi.txt"));
    mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L3Absolute_AK8PFPuppi.txt"));
    mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L2L3Residual_AK8PFPuppi.txt"));
    mMCJetCorrector = new FactorizedJetCorrector(mcParams);

    std::vector<JetCorrectorParameters> dataParams;
    dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L1FastJet_AK8PFPuppi.txt"));
    dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L2Relative_AK8PFPuppi.txt"));
    dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L3Absolute_AK8PFPuppi.txt"));
    dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L2L3Residual_AK8PFPuppi.txt"));
    mDataJetCorrector = new FactorizedJetCorrector(dataParams);
}

int NeroPuppiFatJets::analyze(const edm::Event& iEvent)
{

    if ( mOnlyMc  ) return 0;

    if ( mMinId == "none" ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    iEvent.getByToken(rho_token,rho_handle);

    TString tPrefix(cachedPrefix);

    edm::Handle<reco::PFJetCollection> subjets_handle;
    edm::InputTag subjetLabel("PFJetsSoftDrop"+tPrefix,"SubJets");
    //iEvent.getByLabel(subjetLabel,subjets_handle);
    iEvent.getByToken(subjets_token,subjets_handle);
    const reco::PFJetCollection *subjetCol = subjets_handle.product();
    assert(subjets_handle.isValid());

    edm::Handle<reco::JetTagCollection> btags_handle;
    //iEvent.getByLabel(edm::InputTag((tPrefix+"PFCombinedInclusiveSecondaryVertexV2BJetTags").Data()),btags_handle);
    iEvent.getByToken(btags_token,btags_handle);
    assert((btags_handle.isValid()));

    FactorizedJetCorrector *corrector = ( iEvent.isRealData() ) ? mDataJetCorrector : mMCJetCorrector;

    int ijetRef = -1;
    int nsubjet = 0;


    for (const pat::Jet& j : *handle)
    {
        ijetRef++;

        if (fabs(j.eta() ) > mMaxEta)  continue; 
        if ( !NeroPuppiJets::JetId(j,mMinId) ) continue;
        // pT cut applied after applying JEC if necessary

        // GET  ValueMaps

        // Fill output object   

        // this was reclustered from mini AOD, so we have to apply JEC, etc

        edm::RefToBase<pat::Jet> jetRef(edm::Ref<pat::JetCollection>(handle,ijetRef));

        double jecFactor=0;
        if (fabs(j.eta())<5.191) {
            corrector->setJetPt(j.pt());
            corrector->setJetEta(j.eta());
            corrector->setJetPhi(j.phi());
            corrector->setJetE(j.energy());
            corrector->setRho(*rho_handle);
            corrector->setJetA(j.jetArea());
            corrector->setJetEMF(-99.0);
            jecFactor = corrector->getCorrection();
        }

        if (j.pt()*jecFactor < mMinPt)  continue;
        rawPt -> push_back (j.pt());
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px()*jecFactor, j.py()*jecFactor, j.pz()*jecFactor, j.energy()*jecFactor);

        tau1 -> push_back(j.userFloat(tPrefix+"Njettiness:tau1"));
        tau2 -> push_back(j.userFloat(tPrefix+"Njettiness:tau2"));
        tau3 -> push_back(j.userFloat(tPrefix+"Njettiness:tau3"));

        softdropMass->push_back(j.userFloat(tPrefix+"SDKinematics:Mass")*jecFactor);

        unsigned int nsubjetThisJet=0;
        firstSubjet->push_back(nsubjet);

        for (reco::PFJetCollection::const_iterator i = subjetCol->begin(); i!=subjetCol->end(); ++i) {
            if (reco::deltaR(i->eta(),i->phi(),j.eta(),j.phi())>jetRadius) continue;
            nsubjetThisJet++;

            new ( (*subjet)[nsubjet]) TLorentzVector(i->px(), i->py(), i->pz(), i->energy());
            nsubjet++;

            reco::JetBaseRef sjBaseRef(reco::PFJetRef(subjets_handle,i-subjetCol->begin()));
            subjet_btag->push_back((float)(*(btags_handle.product()))[sjBaseRef]);
        }

        nSubjets->push_back(nsubjetThisJet);

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
