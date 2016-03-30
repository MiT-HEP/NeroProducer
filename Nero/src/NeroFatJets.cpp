#include "NeroProducer/Nero/interface/NeroFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp" // JetId
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

NeroFatJets::NeroFatJets() : 
        NeroCollection(),
        BareFatJets(),
        mRunJEC(false),
        mMinId("loose"),
        jetRadius(0.8),
        mMCJetCorrector(0),
        mDataJetCorrector(0)
{
}

NeroFatJets::~NeroFatJets(){
  BareFunctions::Delete(mMCJetCorrector);
  BareFunctions::Delete(mDataJetCorrector);
}

void NeroFatJets::init()
{
  BareFatJets::init();
  // set up jet energy corrections
  std::string jecDir = "jec/";
    
  if (mRunJEC) {
      std::vector<JetCorrectorParameters> mcParams;
      mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L1FastJet_AK8PFchs.txt"));
      mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L2Relative_AK8PFchs.txt"));
      mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L3Absolute_AK8PFchs.txt"));
      mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L2L3Residual_AK8PFchs.txt"));
      mMCJetCorrector = new FactorizedJetCorrector(mcParams);
     
      std::vector<JetCorrectorParameters> dataParams;
      dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L1FastJet_AK8PFchs.txt"));
      dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L2Relative_AK8PFchs.txt"));
      dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L3Absolute_AK8PFchs.txt"));
      dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L2L3Residual_AK8PFchs.txt"));
      mDataJetCorrector = new FactorizedJetCorrector(dataParams);
  }
}

int NeroFatJets::analyze(const edm::Event& iEvent){

    if ( mOnlyMc  ) return 0;
    
    if ( mMinId == "none" ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);

    if (!mRunJEC) {
        // this jet collection is straight from miniAOD - skip all the fancy stuff

        edm::Handle<reco::JetTagCollection> pfBoostedDoubleSecondaryVertex;  //HBB 74X
        iEvent.getByLabel("pfBoostedDoubleSecondaryVertexAK8BJetTags",pfBoostedDoubleSecondaryVertex); //HBB 74X

        if ( not handle.isValid() ) cout<<"[NeroFatJets]::[analyze]::[ERROR] handle is not valid"<<endl;
        if ( not pfBoostedDoubleSecondaryVertex.isValid() )  cout<<"[NeroFatJets]::[analyze]::[ERROR] pfBoosted.. handle is not valid"<<endl;

        int ijetRef = -1;
        int nsubjet = 0;
        for (const pat::Jet& j : *handle)
        {
            ijetRef++;
            if (j.pt() < mMinPt || fabs(j.eta()) > mMaxEta) continue;

            // JET ID
            if ( !NeroJets::JetId(j,mMinId) ) continue;

            // GET  ValueMaps

            // Fill output object	
            //p4 -> AddLast(new TLorentzVector(j.px(), j.py(), j.pz(), j.energy())  );
            new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());

            rawPt -> push_back (j.pt()*j.jecFactor("Uncorrected"));
            flavour -> push_back( j.partonFlavour() );
            tau1 -> push_back(j.userFloat("NjettinessAK8:tau1"));
            tau2 -> push_back(j.userFloat("NjettinessAK8:tau2"));
            tau3 -> push_back(j.userFloat("NjettinessAK8:tau3"));

            trimmedMass ->push_back(j.userFloat("ak8PFJetsCHSTrimmedMass"));
            prunedMass  ->push_back(j.userFloat("ak8PFJetsCHSPrunedMass"));
            filteredMass->push_back(j.userFloat("ak8PFJetsCHSFilteredMass"));
            softdropMass->push_back(j.userFloat("ak8PFJetsCHSSoftDropMass"));

            // --float hbb= j.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags"); // HBB 75X
            // --cout <<"Hbb tagger="<<hbb<<endl;
            // --if(hbb>10) cout<<endl;
            //float hbb =  (*pfBoostedDoubleSecondaryVertex).value(ijetRef) ;//HBB 74X
            hbb -> push_back( (*pfBoostedDoubleSecondaryVertex).value(ijetRef) ) ;
            
            unsigned int nsubjetThisJet=0;
            firstSubjet->push_back(nsubjet);
            auto &Subjets = j.subjets("SoftDrop");
            for ( auto const & i : Subjets ) {
                new ( (*subjet)[nsubjet]) TLorentzVector(i->px(), i->py(), i->pz(), i->energy());
                subjet_btag->push_back(i->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
                nsubjetThisJet++;
                nsubjet++;
            }
            nSubjets->push_back(nsubjetThisJet);

        }
    } else {
        // this collection was reclustered and needs a different interface

        iEvent.getByToken(rho_token,rho_handle);
        
        TString tPrefix(cachedPrefix);

        edm::Handle<vector<pat::Jet>> subjets_handle;
        edm::InputTag subjetLabel("slimmedJetsAK8PFCHSSoftDropPacked","SubJets");

        iEvent.getByLabel(subjetLabel,subjets_handle);
        const vector<pat::Jet> *subjetCol = subjets_handle.product();
        assert(subjets_handle.isValid());

        FactorizedJetCorrector *corrector = ( iEvent.isRealData() ) ? mDataJetCorrector : mMCJetCorrector;

        int ijetRef = -1;
        int nsubjet = 0;
        for (const pat::Jet& j : *handle)
        {
            ijetRef++;

            if (fabs(j.eta() ) > mMaxEta)  continue; 
            if ( !NeroJets::JetId(j,mMinId) ) continue;
            // pT cut applied after applying JEC if necessary

            // GET  ValueMaps

            // Fill output object   

              edm::RefToBase<pat::Jet> jetRef(edm::Ref<pat::JetCollection>(handle,ijetRef));

              double jecFactor=0;
              if (fabs(j.eta())<5.191) {
                corrector->setJetPt(j.pt() * j.jecFactor("Uncorrected"));
                corrector->setJetEta(j.eta());
                corrector->setJetPhi(j.phi());
                corrector->setJetE(j.energy());
                corrector->setRho(*rho_handle);
                corrector->setJetA(j.jetArea());
                corrector->setJetEMF(-99.0);
                jecFactor = corrector->getCorrection();
              }

              if (j.pt() * j.jecFactor("Uncorrected")*jecFactor < mMinPt)  continue;
              rawPt -> push_back (j.pt() * j.jecFactor("Uncorrected"));
              new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px()*jecFactor * j.jecFactor("Uncorrected"), j.py()*jecFactor * j.jecFactor("Uncorrected"), 
                                                                j.pz()*jecFactor * j.jecFactor("Uncorrected"), j.energy()*jecFactor * j.jecFactor("Uncorrected"));

              tau1 -> push_back(j.userFloat(tPrefix+"Njettiness:tau1"));
              tau2 -> push_back(j.userFloat(tPrefix+"Njettiness:tau2"));
              tau3 -> push_back(j.userFloat(tPrefix+"Njettiness:tau3"));
      
              softdropMass->push_back(j.userFloat(tPrefix+"SDKinematics:Mass")*jecFactor * j.jecFactor("Uncorrected"));
        
              unsigned int nsubjetThisJet=0;
              firstSubjet->push_back(nsubjet);

              for (vector<pat::Jet>::const_iterator i = subjetCol->begin(); i!=subjetCol->end(); ++i) {
                if (reco::deltaR(i->eta(),i->phi(),j.eta(),j.phi())>jetRadius) continue;
                nsubjetThisJet++;
               
                new ( (*subjet)[nsubjet]) TLorentzVector(i->px(), i->py(), i->pz(), i->energy());
                nsubjet++;

                subjet_btag->push_back(j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
              }

              nSubjets->push_back(nsubjetThisJet);
        }
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
