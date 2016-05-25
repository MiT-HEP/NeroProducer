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

    newP4(out_, tau);

    unsigned selBits(BareTaus::TauBaseline);
    if (tau.PFTauIdentifier(mithep::PFTau::iDecayModeFindingNewDMs))
      selBits |= BareTaus::TauDecayModeFindingNewDMs;
    if (tau.PFTauIdentifier(mithep::PFTau::iDecayModeFinding))
      selBits |= BareTaus::TauDecayModeFinding;
    if (tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByLooseElectronRejection) > 0.5)
      selBits |= BareTaus::AgainstEleLoose;
    if (tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByMediumElectronRejection) > 0.5)
      selBits |= BareTaus::AgainstEleMedium;
    if (tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByLooseMuonRejection) > 0.5)
      selBits |= BareTaus::AgainstMuLoose;
    if (tau.PFTauDiscriminator(mithep::PFTau::kDiscriminationByTightMuonRejection) > 0.5)
      selBits |= BareTaus::AgainstMuTight;
    out_.selBits->push_back(selBits);

    out_.Q->push_back(tau.Charge());
    out_.M->push_back(tau.Mass());
    // TODO
    out_.iso->push_back(0.);

    if (out_.IsExtend()) {
      out_.chargedIsoPtSum->push_back(tau.PFTauDiscriminator(mithep::PFTau::kMVA3IsolationChargedIsoPtSum));
      out_.neutralIsoPtSum->push_back(tau.PFTauDiscriminator(mithep::PFTau::kMVA3IsolationNeutralIsoPtSum));
      out_.isoDeltaBetaCorr->push_back(tau.PFTauDiscriminator(mithep::PFTau::dByCombinedIsolationDeltaBetaCorrRaw3Hits));
    }
  }
}
