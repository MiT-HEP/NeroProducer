#include "NeroProducer/Bambu/interface/FatJetsFiller.h"

#include "MitAna/DataTree/interface/JetCol.h"
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
  }
}
