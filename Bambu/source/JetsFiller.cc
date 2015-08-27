#include "NeroProducer/Bambu/interface/JetsFiller.h"

#include "MitAna/DataTree/interface/JetCol.h"
#include "MitAna/DataTree/interface/PFJet.h"

#include "TDirectory.h"
#include "TROOT.h"

ClassImp(mithep::nero::JetsFiller)

void
mithep::nero::JetsFiller::initialize()
{
  if (!jetId_ && jetIdMVATrainingSet_ != mithep::JetIDMVA::nMVATypes) {
    auto* cwd = gDirectory;

    gROOT->cd();
    jetId_ = new JetIDMVA();
    jetId_->Initialize(JetIDMVA::CutType(jetIdCutWP_), JetIDMVA::MVAType(jetIdMVATrainingSet_), jetIdMVAWeightsFile_, jetIdCutsFile_);

    cwd->cd();

    ownJetId_ = true;
  }

  if (jetId_ && !jetId_->IsInitialized())
    throw std::runtime_error("JetID not initialized");
}

void
mithep::nero::JetsFiller::finalize()
{
  if (jetId_ && ownJetId_) {
    delete jetId_;
    jetId_ = 0;
  }
}

void
mithep::nero::JetsFiller::fill()
{
  auto* jets = getSource<mithep::JetCol>(jetsName_);
  if (!jets)
    return;

  auto* vertices = getSource<mithep::VertexCol>(verticesName_);

  for (unsigned iJ(0); iJ != jets->GetEntries(); ++iJ) {
    auto& jet(*jets->At(iJ));

    newP4(out_, jet);

    auto&& rawMom(jet.RawMom());

    out_.rawPt->push_back(rawMom.Pt());
    out_.bDiscr->push_back(jet.BJetTagsDisc(mithep::Jet::kCombinedInclusiveSecondaryVertexV2));
    // TODO
    out_.qgl->push_back(0.);

    out_.flavour->push_back(jet.MatchedMCFlavor());

    // TODO
    out_.matchedPartonPdgId->push_back(0);
    out_.motherPdgId->push_back(0);
    out_.grMotherPdgId->push_back(0);

    if (jet.ObjType() == mithep::kPFJet) {
      auto& pfJet(static_cast<mithep::PFJet&>(jet));
      double rawE(rawMom.E());
      double chFrac(pfJet.ChargedHadronEnergy() / rawE);
      double nhFrac(pfJet.NeutralHadronEnergy() / rawE);
      double neFrac(pfJet.NeutralEmEnergy() / rawE);

      out_.mjId->push_back(chFrac > 0.2 && nhFrac < 0.7 && neFrac < 0.7);
      out_.mjId_loose->push_back(nhFrac < 0.7 && neFrac < 0.9);

      if (jetId_ && vertices)
        out_.puId->push_back(jetId_->MVAValue(&pfJet, vertices->At(0), vertices));
      else
        out_.puId->push_back(-999.);
    }
    else {
      out_.mjId->push_back(false);
      out_.mjId_loose->push_back(false);
      out_.puId->push_back(-999.);
    }
  }
}
