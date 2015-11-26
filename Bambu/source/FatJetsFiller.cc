#include "NeroProducer/Bambu/interface/FatJetsFiller.h"

#include "MitAna/DataTree/interface/XlFatJetCol.h"
#include "MitAna/DataTree/interface/XlFatJet.h"

ClassImp(mithep::nero::FatJetsFiller)

void
mithep::nero::FatJetsFiller::defineBranches(TTree* _tree)
{
  switch (collection_) {
  case BaseFiller::kAK8Jets:
    out_.defineBranches(_tree, "ak8");
    break;
  case BaseFiller::kCA15Jets:
    out_.defineBranches(_tree, "ca15");
    break;
  default:
    break;
  }
}

void
mithep::nero::FatJetsFiller::initialize()
{
}

void
mithep::nero::FatJetsFiller::finalize()
{
}

void
mithep::nero::FatJetsFiller::fill()
{
  auto* jets = getSource<mithep::XlFatJetCol>(fatJetsName_);

  for (unsigned iJ(0); iJ != jets->GetEntries(); ++iJ) {
    if (jets->At(iJ)->ObjType() != kXlFatJet)
      throw std::runtime_error("non-fat jet passed to FatJetsFiller");
    
    auto& jet(*static_cast<mithep::XlFatJet const*>(jets->At(iJ)));

    if (jet.Pt() < 100.)
      continue;

    newP4(out_, jet);

    out_.rawPt->push_back(jet.RawMom().Pt());
    out_.flavour->push_back(jet.MatchedMCFlavor());
    out_.tau1->push_back(jet.Tau1());
    out_.tau2->push_back(jet.Tau2());
    out_.tau3->push_back(jet.Tau3());

    out_.trimmedMass->push_back(jet.MassTrimmed());
    out_.prunedMass->push_back(jet.MassPruned());
    out_.filteredMass->push_back(jet.MassFiltered());
    out_.softdropMass->push_back(jet.MassSDb0());
    auto& subjets(jet.GetSubJets(XlSubJet::kSoftDrop));
    out_.ak8jet_hasSubjet->push_back(subjets.GetEntries() == 0 ? 0 : 1);
    for (unsigned iS(0); iS != subjets.GetEntries(); ++iS) {
      auto& subjet(*subjets.At(iS));
      newP4(*out_.ak8_subjet, subjet);
    }

    // btags are stored in Bambu and ordered by decreasing subjet pT.
    // note that btag vector may be out of sync with other fatjet vectors
    // since different jet algorithms are being used
    std::vector<float> const &subjetBtags = jet.GetSubJetBtags();
    unsigned int nSJBtags = subjetBtags.size();
    for (unsigned iS=0; iS != nSJBtags; ++iS) 
      out_.ak8subjet_btag->push_back(subjetBtags[iS]);
  
  }
}

float
mithep::nero::FatJetsFiller::cleanInput(float x) {
	if (x>=0) // false for nan
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

