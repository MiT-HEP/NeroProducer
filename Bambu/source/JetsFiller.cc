#include "NeroProducer/Bambu/interface/JetsFiller.h"

#include "MitAna/DataTree/interface/JetCol.h"
#include "MitAna/DataTree/interface/Vertex.h"
#include "MitAna/DataTree/interface/PFJet.h"
#include "MitAna/DataCont/interface/Types.h"

#include "TDirectory.h"
#include "TROOT.h"

ClassImp(mithep::nero::JetsFiller)

void
mithep::nero::JetsFiller::defineBranches(TTree* _tree)
{
  switch (collection_) {
  case BaseFiller::kJets:
    out_.cachedPrefix="";
    break;
  case BaseFiller::kPuppiJets:
    out_.cachedPrefix="puppi";
    break;
  default:
    break;
  }
  out_.defineBranches(_tree);
}

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

  mithep::NFArrBool const* tightId(0);
  if (tightIdName_.Length() != 0)
    tightId = getSource<mithep::NFArrBool>(tightIdName_);

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
      auto& pfJet(static_cast<mithep::PFJet const&>(jet));
      double rawE(rawMom.E());
      double chFrac(pfJet.ChargedHadronEnergy() / rawE);
      double nhFrac(pfJet.NeutralHadronEnergy() / rawE);
      double neFrac(pfJet.NeutralEmEnergy() / rawE);

      unsigned selBits(BareJets::JetBaseline | BareJets::JetLoose);
      if (tightId && tightId->At(iJ))
        selBits |= BareJets::JetTight;
      if (chFrac > 0.2 && nhFrac < 0.7 && neFrac < 0.7)
        selBits |= BareJets::mjId;
      if (nhFrac < 0.7 && neFrac < 0.9)
        selBits |= BareJets::mjIdLoose;
      if (nhFrac < 0.8 && chFrac > 0.1)
        selBits |= BareJets::mjId2015;
      out_.selBits->push_back(selBits);

      if (jetId_ && vertices)
        out_.puId->push_back(jetId_->MVAValue(&pfJet, vertices->At(0), vertices));
      else
        out_.puId->push_back(-999.);

      double sumQW(0.);
      double sumW(0.);
      double sumQWPV(0.);
      double sumWPV(0.);
      auto&& jetP3(pfJet.Mom().Vect());
      for (unsigned iP(0); iP != pfJet.NPFCands(); ++iP) {
        auto& cand(*pfJet.PFCand(iP));

        if (cand.Charge() != 0.) {
          double w(jetP3.Dot(cand.Mom().Vect()));
          sumQW += cand.Charge() * w;
          sumW += w;
          
          if (cand.Trk() && vertices->At(0)->HasTrack(cand.Trk())) {
            sumQWPV += cand.Charge() * w;
            sumWPV += w;
          }
        }
      }

      out_.Q->push_back(sumQW / sumW);
      out_.QnoPU->push_back(sumQWPV / sumWPV);
    }
    else {
      out_.selBits->push_back(0);
      out_.puId->push_back(-999.);

      out_.Q->push_back(0.);
      out_.QnoPU->push_back(0.);
    }
  }
}

void
mithep::nero::JetsFiller::SetJetIdMVAWeightsFile(char const* _path, unsigned _idx/* = 0*/)
{
  if (_idx >= jetIdMVAWeightsFile_.size())
    jetIdMVAWeightsFile_.resize(_idx + 1, "");

  jetIdMVAWeightsFile_[_idx] = _path;
}
