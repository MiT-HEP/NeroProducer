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
        mDataJetCorrector(0),
        mMCJetCorrectorPuppi(0),
        mDataJetCorrectorPuppi(0)
{
}

NeroFatJets::~NeroFatJets(){
  BareFunctions::Delete(mMCJetCorrector);
  BareFunctions::Delete(mDataJetCorrector);
  BareFunctions::Delete(mMCJetCorrectorPuppi);
  BareFunctions::Delete(mDataJetCorrectorPuppi);
}

void NeroFatJets::init()
{
  BareFatJets::init();
  // set up jet energy corrections
  string jecBasePathMC="jec/Summer16_23Sep2016V4";
  //
  cout<<"[NeroFatJets]::[init]::[INFO] Taking JEC for MC="<<jecBasePathMC<<"_MC_L2Relative_AK8PFchs.txt [..]"<<endl;
  cout<<"[NeroFatJets]::[init]::[INFO] Taking JEC for DATA="<<jecBasePath<<"_DATA_L2Relative_AK8PFchs.txt [..]"<<endl;
 
  //no L1 for mass  
  std::vector<JetCorrectorParameters> mcParams;
  mcParams.push_back(JetCorrectorParameters(jecBasePathMC+"_MC_L2Relative_AK8PFchs.txt"));
  mcParams.push_back(JetCorrectorParameters(jecBasePathMC+"_MC_L3Absolute_AK8PFchs.txt"));
  mMCJetCorrector = new FactorizedJetCorrector(mcParams);
  
  std::vector<JetCorrectorParameters> dataParams;
  dataParams.push_back(JetCorrectorParameters(jecBasePath + "_DATA_L2Relative_AK8PFchs.txt"));
  dataParams.push_back(JetCorrectorParameters(jecBasePath + "_DATA_L3Absolute_AK8PFchs.txt"));
  dataParams.push_back(JetCorrectorParameters(jecBasePath + "_DATA_L2L3Residual_AK8PFchs.txt"));
  mDataJetCorrector = new FactorizedJetCorrector(dataParams);

  //Puppi
  std::vector<JetCorrectorParameters> mcParamsPuppi;
  mcParamsPuppi.push_back(JetCorrectorParameters(jecBasePathMC+"_MC_L2Relative_AK8PFPuppi.txt"));
  mcParamsPuppi.push_back(JetCorrectorParameters(jecBasePathMC+"_MC_L3Absolute_AK8PFPuppi.txt"));
  mMCJetCorrectorPuppi = new FactorizedJetCorrector(mcParamsPuppi);
  
  std::vector<JetCorrectorParameters> dataParamsPuppi;
  dataParamsPuppi.push_back(JetCorrectorParameters(jecBasePath + "_DATA_L2Relative_AK8PFPuppi.txt"));
  dataParamsPuppi.push_back(JetCorrectorParameters(jecBasePath + "_DATA_L3Absolute_AK8PFPuppi.txt"));
  dataParamsPuppi.push_back(JetCorrectorParameters(jecBasePath + "_DATA_L2L3Residual_AK8PFPuppi.txt"));
  mDataJetCorrectorPuppi = new FactorizedJetCorrector(dataParamsPuppi);

}

int NeroFatJets::analyze(const edm::Event& iEvent){

    if ( mOnlyMc  ) return 0;
    
    if ( mMinId == "none" ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);
    iEvent.getByToken(sd_tau1_token,sd_tau1_handle);
    iEvent.getByToken(sd_tau2_token,sd_tau2_handle);
    iEvent.getByToken(sd_tau3_token,sd_tau3_handle);

    //Vertex
    iEvent.getByToken(vertex_token,vertex_handle);
    //Rho
    iEvent.getByToken(rho_token,rho_handle);

    // this jet collection is straight from miniAOD - skip all the fancy stuff

    if ( not handle.isValid() ) cout<<"[NeroFatJets]::[analyze]::[ERROR] handle is not valid"<<endl;
    if ( not sd_tau1_handle.isValid() ) cout<<"[NeroJets]::[analyze]::[ERROR] sd tau1_handle is not valid"<<endl;
    if ( not sd_tau2_handle.isValid() ) cout<<"[NeroJets]::[analyze]::[ERROR] sd tau2_handle is not valid"<<endl;
    if ( not sd_tau3_handle.isValid() ) cout<<"[NeroJets]::[analyze]::[ERROR] sd tau3_handle is not valid"<<endl;

    int ijetRef = -1;
    int nsubjet = 0;
    for (const pat::Jet& j : *handle)
    {
        ijetRef++;
        if (j.pt() < mMinPt || fabs(j.eta()) > mMaxEta) continue;
        
        // JET ID
        if ( !NeroJets::JetId(j,mMinId) ) continue;
        
        // GET  ValueMaps
        edm::RefToBase<pat::Jet> jetRef(edm::Ref<pat::JetCollection>(handle, ijetRef) );
        float sdtau1_ = (*sd_tau1_handle)[jetRef];
        sdtau1     -> push_back( sdtau1_);
        float sdtau2_ = (*sd_tau2_handle)[jetRef];
        sdtau2     -> push_back( sdtau2_);
        float sdtau3_ = (*sd_tau3_handle)[jetRef];
        sdtau3     -> push_back( sdtau3_);

        // Fill output object	
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
        
        rawPt -> push_back (j.pt()*j.jecFactor("Uncorrected"));
        
        //L2L3 Corrected Jet only for the pruned mass correction
        double corr=0.;              
        FactorizedJetCorrector *jecAK8_ = ( iEvent.isRealData() ) ? mDataJetCorrector : mMCJetCorrector;
        jecAK8_->setJetEta( j.eta()*j.jecFactor("Uncorrected") );
        jecAK8_->setJetPt ( j.pt()*j.jecFactor("Uncorrected") );
        jecAK8_->setJetE  ( j.energy()*j.jecFactor("Uncorrected") );
        jecAK8_->setJetA  ( j.jetArea() );
        jecAK8_->setRho   ( *rho_handle );
        jecAK8_->setNPV   ( vertex_handle->size() );
        corr = jecAK8_->getCorrection();        

        flavour -> push_back( j.partonFlavour() );

        tau1 -> push_back(j.userFloat("NjettinessAK8:tau1"));
        tau2 -> push_back(j.userFloat("NjettinessAK8:tau2"));
        tau3 -> push_back(j.userFloat("NjettinessAK8:tau3"));

        corrprunedMass  ->push_back(j.userFloat("ak8PFJetsCHSPrunedMass")*corr);
        prunedMass  ->push_back(j.userFloat("ak8PFJetsCHSPrunedMass"));
        softdropMass->push_back(j.userFloat("ak8PFJetsCHSSoftDropMass"));
        
        hbb -> push_back( j.bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags") ) ;
            
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

        /////////////////////////////////////
        // Fill Puppi
        /////////////////////////////////////

        TLorentzVector puppi_jet;
        puppi_jet.SetPtEtaPhiM(j.userFloat("ak8PFJetsPuppiValueMap:pt"),j.userFloat("ak8PFJetsPuppiValueMap:eta"),j.userFloat("ak8PFJetsPuppiValueMap:phi"),j.userFloat("ak8PFJetsPuppiValueMap:mass"));
        
        new ( (*puppiAK8)[puppiAK8->GetEntriesFast()]) TLorentzVector(puppi_jet);

        //puppiAK8   ->push_back(puppi_jet);
        puppi_tau1 ->push_back(j.userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau1"));
        puppi_tau2 ->push_back(j.userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau2"));

        TLorentzVector puppi_softdrop, puppi_softdrop_subjet;
        auto const & sdSubjetsPuppi = j.subjets("SoftDropPuppi");
        for ( auto const & it : sdSubjetsPuppi ) {
            puppi_softdrop_subjet.SetPtEtaPhiM(it->correctedP4(0).pt(),it->correctedP4(0).eta(),it->correctedP4(0).phi(),it->correctedP4(0).mass());
            puppi_softdrop+=puppi_softdrop_subjet;
        }
        
        puppi_softdrop_mass->push_back(puppi_softdrop.M());

        //L2L3 Corrected Jet only for the pruned mass correction
        double corrpuppi=0.;              
        FactorizedJetCorrector *jecAK8Puppi_ = ( iEvent.isRealData() ) ? mDataJetCorrectorPuppi : mMCJetCorrectorPuppi;
        
        jecAK8Puppi_->setJetEta( puppi_softdrop.Eta() );
        jecAK8Puppi_->setJetPt ( puppi_softdrop.Pt() );
        jecAK8Puppi_->setJetE  ( puppi_softdrop.Energy() );
        jecAK8Puppi_->setJetA  ( j.jetArea() );
        jecAK8Puppi_->setRho   ( *rho_handle );
        jecAK8Puppi_->setNPV   ( vertex_handle->size() );
        corrpuppi = jecAK8Puppi_->getCorrection();
        
        puppi_softdrop_masscorr->push_back(corrpuppi*puppi_softdrop.M());

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
