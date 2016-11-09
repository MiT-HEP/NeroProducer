#include "NeroProducer/Bambu/interface/FatJetsFiller.h"

#include "MitAna/DataTree/interface/FatJetCol.h"
#include "MitAna/DataTree/interface/FatJet.h"
#include "MitAna/DataTree/interface/XlFatJetCol.h"
#include "MitAna/DataTree/interface/XlFatJet.h"
#include "MitAna/DataTree/interface/JetCol.h"
#include "MitAna/DataTree/interface/Jet.h"
#include "MitAna/DataTree/interface/XlSubJet.h"
#include <algorithm>

ClassImp(mithep::nero::FatJetsFiller)

void
mithep::nero::FatJetsFiller::defineBranches(TTree* _tree)
{
  switch (collection_) {
  case BaseFiller::kAK8Jets:
    out_.cachedPrefix = "AK8CHS";
    break;
  case BaseFiller::kCA15Jets:
    out_.cachedPrefix = "CA15CHS";
    break;
  case BaseFiller::kAK8PuppiJets:
    out_.cachedPrefix="AK8Puppi";
    break;
  case BaseFiller::kCA15PuppiJets:
    out_.cachedPrefix="CA15Puppi";
    break;
  default:
    break;
  }
  out_.defineBranches(_tree);
}

void
mithep::nero::FatJetsFiller::initialize()
{
}

void
mithep::nero::FatJetsFiller::finalize()
{
  if (MVAOn)
    delete topANN;
}

bool
mithep::nero::FatJetsFiller::fillFatJet(const mithep::FatJet &jet)
{
    if (jet.Pt() < 100.)
      return false;

    double rawE = jet.RawMom().E();
    double chf = jet.ChargedHadronEnergy()/rawE;
    double nhf = jet.NeutralHadronEnergy()/rawE;

    if (applyMJId && (nhf>0.8 || chf<0.1))
      return false;

    newP4(out_, jet);


    out_.rawPt->push_back(jet.RawMom().Pt());
    out_.tau1->push_back(jet.Tau1());
    out_.tau2->push_back(jet.Tau2());
    out_.tau3->push_back(jet.Tau3());

    out_.softdropMass->push_back(jet.MassSoftDrop());
    
    std::vector<float> subjetBtags = jet.GetSubJetBtags();

    unsigned int nSubjets = subjetBtags.size();
    out_.nSubjets->push_back(nSubjets);
    out_.firstSubjet->push_back(subjetCounter);

    for (unsigned int iB=0; iB!=nSubjets; ++iB) {
      out_.subjet_btag->push_back(subjetBtags[iB]);
    }
    subjetCounter += nSubjets;

    return true;
}

void
mithep::nero::FatJetsFiller::fill()
{
  subjetCounter=0;
  if (extended) {
    auto* jets = getSource<mithep::XlFatJetCol>(fatJetsName_);

    for (unsigned iJ(0); iJ != jets->GetEntries(); ++iJ) {
      if (jets->At(iJ)->ObjType() != kXlFatJet)
        throw std::runtime_error("non-xlfat jet passed to FatJetsFiller with extended=true");
      auto& jet(*static_cast<mithep::XlFatJet const*>(jets->At(iJ)));

      bool filled = fillFatJet(jet);    
      
      if (filled) {
        out_.trimmedMass->push_back(jet.MassTrimmed());
        out_.prunedMass->push_back(jet.MassPruned());
        out_.softdropMass->push_back(jet.MassSoftDrop());
      }
    }
  } else {
    auto* jets = getSource<mithep::JetCol>(fatJetsName_);

    for (unsigned iJ(0); iJ != jets->GetEntries(); ++iJ) {
      if (jets->At(iJ)->ObjType() != kFatJet)
        throw std::runtime_error("non-fat jet passed to FatJetsFiller");
      auto& jet(*static_cast<mithep::FatJet const*>(jets->At(iJ)));

      bool filled = fillFatJet(jet);    
      
      if (filled) {
        out_.trimmedMass->push_back(-1);
        out_.prunedMass->push_back(-1);
        out_.softdropMass->push_back(jet.MassSoftDrop());
      }
    }
  }

}

float
mithep::nero::FatJetsFiller::cleanInput(float x) {
	if (x==x) // false for nan or inf
		return x;
	else
		return -1;
}

float 
mithep::nero::FatJetsFiller::computePull(const FourVectorM &jetMom, const Vect4M &groomedMom ) {

  TLorentzVector v1;
  TLorentzVector v2;

  v1.SetPtEtaPhiM(jetMom.Pt(),jetMom.Eta(),jetMom.Phi(),jetMom.M());
  v2.SetPtEtaPhiM(groomedMom.Pt(),groomedMom.Eta(),groomedMom.Phi(),groomedMom.M());

  float pull = TMath::Sqrt( TMath::Power(v1.Px()-v2.Px(),2)+
				               TMath::Power(v1.Py()-v2.Py(),2)+
				               TMath::Power(v1.Pz()-v2.Pz(),2)  )/v1.Pt();

  if (pull>=0)
    return pull;
  else {
    return -1;
  }
}

