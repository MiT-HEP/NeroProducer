#include "NeroProducer/Bambu/interface/TausFiller.h"

#include "MitAna/DataTree/interface/PFTauCol.h"

#include <map>

ClassImp(mithep::nero::TausFiller)

void
mithep::nero::TausFiller::fill()
{
  auto* taus = getSource<mithep::PFTauCol>(tausName_);

  for (unsigned iT(0); iT != taus->GetEntries(); ++iT) {
    auto& tau(*taus->At(iT));

    newP4(out_, tau.Px(), tau.Py(), tau.Pz(), tau.E());

    out_.id->push_back(tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByDecayModeFinding));
    out_.Q->push_back(tau.Charge());
    out_.M->push_back(tau.Mass());
    // TODO
    out_.iso->push_back(0.);

    if (out_.IsExtend()) {
      out_.chargedIsoPtSum->push_back(tau.PFTauDiscriminator(mithep::PFTau::kMVA3IsolationChargedIsoPtSum));
      out_.neutralIsoPtSum->push_back(tau.PFTauDiscriminator(mithep::PFTau::kMVA3IsolationNeutralIsoPtSum));
      out_.isoDeltaBetaCorr->push_back(tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits));
      out_.againstEleLoose->push_back(tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByMVA5LooseElectronRejection));
      out_.againstEleMedium->push_back(tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByMVA5MediumElectronRejection));
      out_.againstMuLoose->push_back(tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByLooseMuonRejection3));
      out_.againstMuTight->push_back(tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByTightMuonRejection3));
    }
  }
}
