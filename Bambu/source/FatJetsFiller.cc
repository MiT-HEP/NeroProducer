#include "NeroProducer/Bambu/interface/FatJetsFiller.h"

#include "MitAna/DataTree/interface/JetCol.h"

ClassImp(mithep::nero::FatJetsFiller)

void
mithep::nero::FatJetsFiller::fill()
{
  auto* jets = getSource<mithep::JetCol>(fatJetsName_);
  if (!jets)
    return;

  for (unsigned iJ(0); iJ != jets->GetEntries(); ++iJ) {
    auto& jet(*jets->At(iJ));

    if (jet.Pt() < 100.)
      continue;

    newP4(out_, jet.Px(), jet.Py(), jet.Pz(), jet.E());

    out_.rawPt->push_back(jet.RawMom().Pt());
    out_.flavour->push_back(jet.MatchedMCFlavor());
    out_.tau1->push_back(0.);
    out_.tau2->push_back(0.);
    out_.tau3->push_back(0.);

    out_.trimmedMass->push_back(0.);
    out_.prunedMass->push_back(0.);
    out_.filteredMass->push_back(0.);
    out_.softdropMass->push_back(0.);
    out_.ak8jet_hasSubjet->push_back(false);
  }
}
