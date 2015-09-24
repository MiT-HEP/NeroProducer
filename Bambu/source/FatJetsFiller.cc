#include "NeroProducer/Bambu/interface/FatJetsFiller.h"

#include "MitAna/DataTree/interface/JetCol.h"
#include "MitAna/DataTree/interface/XlFatJet.h"

#include "TLorentzVector.h"
#include "TMath.h"

ClassImp(mithep::nero::FatJetsFiller)

void 
mithep::nero::FatJetsFiller::initialize()
{
	topANN = new NeuralNet(5,2);
	#include "topTagger_simple.icc" // these are just weights - move to MIT_DATA?
	topANN->AllocateMemory();
	topANN->AddBranchAddress(&nn_mSD,69.14170513,70.41396876);
	topANN->AddBranchAddress(&nn_QGTag,0.24245312,0.31660758);
	topANN->AddBranchAddress(&nn_groomedIso,0.18981184,0.25050463);
	topANN->AddBranchAddress(&nn_tau32,0.79239905,0.10837058);
	topANN->AddBranchAddress(&nn_tau21,0.64983544,0.17112768);
}

void
mithep::nero::FatJetsFiller::finalize()
{
	delete topANN;
}

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
mithep::nero::FatJetsFiller::fill()
{
  auto* jets = getSource<mithep::JetCol>(fatJetsName_);

  for (unsigned iJ(0); iJ != jets->GetEntries(); ++iJ) {
    if (jets->At(iJ)->ObjType() != kXlFatJet)
      throw std::runtime_error("non-fat jet passed to FatJetsFiller");
    
    auto& jet(*static_cast<mithep::XlFatJet*>(jets->At(iJ)));

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
      out_.ak8subjet_btag->push_back(subjet.BTag());
    }

    nn_mSD = out_.softdropMass->back();
    nn_QGTag = cleanInput(jet.QGTag());
    nn_groomedIso = computePull(jet.Mom(),jet.SoftDropP());
    nn_tau32 = cleanInput(jet.Tau3()/jet.Tau2());
    nn_tau21 = cleanInput(jet.Tau2()/jet.Tau1());
    out_.topMVA->push_back(topANN->Evaluate()[1]);
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

