#include "NeroProducer/Nero/interface/NeroPuppiFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroPuppiJets.hpp" // JetId
#include "NeroProducer/Nero/interface/Nero.hpp"
#include <cstdlib>
#include <string>


NeroPuppiFatJets::NeroPuppiFatJets() : 
        NeroCollection(),
        BarePuppiFatJets(),
        mUseCA15(false),
        mMCJetCorrector(0),
        mDataJetCorrector(0)
{
}

NeroPuppiFatJets::~NeroPuppiFatJets()
{
  if (mMCJetCorrector)   delete mMCJetCorrector;
  if (mDataJetCorrector) delete mDataJetCorrector;
}

void NeroPuppiFatJets::init()
{
  BarePuppiFatJets::init();
  // set up jet energy corrections
  std::string jecDir(getenv("CMSSW_BASE"));
  jecDir += "/src/NeroProducer/Nero/test/jec/";
  std::vector<JetCorrectorParameters> mcParams;
  std::vector<JetCorrectorParameters> dataParams;
  mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L1FastJet_AK8PFPuppi.txt"));
  mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L2Relative_AK8PFPuppi.txt"));
  mcParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_MC_L3Absolute_AK8PFPuppi.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L1FastJet_AK8PFPuppi.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L2Relative_AK8PFPuppi.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L3Absolute_AK8PFPuppi.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Summer15_25nsV6_DATA_L2L3Residual_AK8PFPuppi.txt"));
  mMCJetCorrector = new FactorizedJetCorrector(mcParams);
  mDataJetCorrector = new FactorizedJetCorrector(dataParams);
}

int NeroPuppiFatJets::analyze(const edm::Event& iEvent)
{

    if ( mOnlyMc  ) return 0;

    if ( mMinId == "none" ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    iEvent.getByToken(rho_token,rho_handle);

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

        if (!mUseCA15) {
          // this is an AK8 jet straight from mini AOD
          
          if (j.pt() < mMinPt ) continue;
          rawPt -> push_back (j.pt()*j.jecFactor("Uncorrected"));

          // JET ID
          
          new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());

          edm::Handle<reco::JetTagCollection> pfBoostedDoubleSecondaryVertex;  //HBB 74X
          iEvent.getByLabel("pfBoostedDoubleSecondaryVertexAK8BJetTags",pfBoostedDoubleSecondaryVertex); //HBB 74X

          if ( not handle.isValid() ) cout<<"[NeroPuppiFatJets]::[analyze]::[ERROR] handle is not valid"<<endl;
          if ( not pfBoostedDoubleSecondaryVertex.isValid() )  cout<<"[NeroPuppiFatJets]::[analyze]::[ERROR] pfBoosted.. handle is not valid"<<endl;

          flavour -> push_back( j.partonFlavour() );
          tau1 -> push_back(j.userFloat("NjettinessAK8Puppi:tau1"));
          tau2 -> push_back(j.userFloat("NjettinessAK8Puppi:tau2"));
          tau3 -> push_back(j.userFloat("NjettinessAK8Puppi:tau3"));
  
          trimmedMass ->push_back(j.userFloat("ak8PuppiJetsTrimmedMass"));
          prunedMass  ->push_back(j.userFloat("ak8PuppiJetsPrunedMass"));
          filteredMass->push_back(j.userFloat("ak8PuppiJetsFilteredMass"));
          softdropMass->push_back(j.userFloat("ak8PuppiJetsSoftDropMass"));
          ak8_hasSubjet->push_back(j.hasSubjets("SoftDrop"));
    
          // --float hbb= j.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags"); // HBB 75X
          // --cout <<"Hbb tagger="<<hbb<<endl;
          // --if(hbb>10) cout<<endl;
          //float hbb =  (*pfBoostedDoubleSecondaryVertex).value(ijetRef) ;//HBB 74X
          hbb -> push_back( (*pfBoostedDoubleSecondaryVertex).value(ijetRef) ) ;
  
          unsigned int nsubjetThisJet=0;
          auto Subjets = j.subjets("SoftDrop");
          for ( auto const & i : Subjets ) {
              new ( (*ak8_subjet)[nsubjet]) TLorentzVector(i->px(), i->py(), i->pz(), i->energy());
              ak8subjet_btag->push_back(i->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
              nsubjet++;
              nsubjetThisJet++;
          }
          ak8_nSubjets->push_back(nsubjetThisJet);
        } else {
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

          tau1 -> push_back(j.userFloat("Njettiness15:tau1"));
          tau2 -> push_back(j.userFloat("Njettiness15:tau2"));
          tau3 -> push_back(j.userFloat("Njettiness15:tau3"));
  
          softdropMass->push_back(j.userFloat("SoftDrop15:Mass"));
          ak8_hasSubjet->push_back(j.hasSubjets("SoftDrop"));
    
          unsigned int nsubjetThisJet=0;
          auto Subjets = j.subjets("SoftDrop");
          for ( auto const & i : Subjets ) {
              new ( (*ak8_subjet)[nsubjet]) TLorentzVector(i->px(), i->py(), i->pz(), i->energy());
              ak8subjet_btag->push_back(i->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags"));
              nsubjet++;
              nsubjetThisJet++;
          }
          ak8_nSubjets->push_back(nsubjetThisJet);
        }

    }
    return 0;
}
