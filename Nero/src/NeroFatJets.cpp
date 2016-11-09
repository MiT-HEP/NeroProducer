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
    
  std::vector<JetCorrectorParameters> mcParams;
  mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV8_MC_L1FastJet_AK8PFchs.txt"));
  mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV8_MC_L2Relative_AK8PFchs.txt"));
  mcParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV8_MC_L3Absolute_AK8PFchs.txt"));
  mMCJetCorrector = new FactorizedJetCorrector(mcParams);
  
  std::vector<JetCorrectorParameters> dataParams;
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV8_DATA_L1FastJet_AK8PFchs.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV8_DATA_L2Relative_AK8PFchs.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV8_DATA_L3Absolute_AK8PFchs.txt"));
  dataParams.push_back(JetCorrectorParameters(jecDir + "Spring16_25nsV8_DATA_L2L3Residual_AK8PFchs.txt"));
  mDataJetCorrector = new FactorizedJetCorrector(dataParams);

}

int NeroFatJets::analyze(const edm::Event& iEvent){

    if ( mOnlyMc  ) return 0;
    
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
        if (j.pt() < mMinPt || fabs(j.eta()) > mMaxEta) continue;
        
        // JET ID
        if ( !NeroJets::JetId(j,mMinId) ) continue;
        
        // GET  ValueMaps
        
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
