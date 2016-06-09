#include "NeroProducer/Bambu/interface/MetFiller.h"

#include "MitAna/DataTree/interface/PFMetCol.h"
#include "MitAna/DataTree/interface/CaloMetCol.h"
#include "MitAna/DataTree/interface/MetCol.h"
#include "MitAna/DataTree/interface/ParticleCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/Names.h"

#include "TVector2.h"
#include "TMath.h"

ClassImp(mithep::nero::MetFiller)

void
mithep::nero::MetFiller::fill()
{
  auto& met(*getSource<mithep::PFMetCol>(metName_)->At(0));
  newP4(out_, met);

  if (jesUpMetName_.Length() != 0) {
    auto& jesUpMet(*getSource<mithep::PFMetCol>(jesUpMetName_)->At(0));
    out_.ptJESUP->push_back(jesUpMet.Pt());
  }
  if (jesDownMetName_.Length() != 0) {
    auto& jesDownMet(*getSource<mithep::PFMetCol>(jesDownMetName_)->At(0));
    out_.ptJESDOWN->push_back(jesDownMet.Pt());
  }

  if (muonsName_.Length() != 0) {
    out_.metNoMu->SetXYZT(met.Px(), met.Py(), met.Pz(), met.E());
    auto& muons(*getSource<mithep::ParticleCol>(muonsName_));
    for (unsigned iM(0); iM != muons.GetEntries(); ++iM) {
      auto& muon(*muons.At(iM));
      *out_.metNoMu += TLorentzVector(muon.Px(), muon.Py(), muon.Pz(), muon.E());
    }
  }

  if (pfCandidatesName_.Length() != 0) {
    *out_.pfMet_e3p0 *= 0.;
    out_.sumEtRawNoHF = 0.;
    *out_.trackMet *= 0.;
    auto& pfCandidates(*getSource<mithep::PFCandidateCol>(pfCandidatesName_));
    for (unsigned iP(0); iP != pfCandidates.GetEntries(); ++iP) {
      auto& cand(*pfCandidates.At(iP));

      if (cand.AbsEta() < 3.) {
        *out_.pfMet_e3p0 -= TLorentzVector(cand.Px(), cand.Py(), cand.Pz(), cand.E());
        out_.sumEtRawNoHF += cand.Pt();
      }

      switch (cand.PFType()) {
      case mithep::PFCandidate::eHadron:
      case mithep::PFCandidate::eElectron:
      case mithep::PFCandidate::eMuon:
        *out_.trackMet -= TLorentzVector(cand.Px(), cand.Py(), cand.Pz(), cand.E());
        break;
      default:
        break;
      }
    }
  }

  if (caloMetName_.Length() != 0) {
    auto& caloMet(*getSource<mithep::CaloMetCol>(caloMetName_)->At(0));
    out_.caloMet_Pt = caloMet.Pt();
    out_.caloMet_Phi = caloMet.Phi();
    out_.caloMet_SumEt = caloMet.SumEt();
  }

  if (rawMetName_.Length() != 0) {
    auto& rawMet(*getSource<mithep::PFMetCol>(rawMetName_)->At(0));
    out_.sumEtRaw = rawMet.SumEt();
    out_.rawMet_Pt = rawMet.Pt();
    out_.rawMet_Phi = rawMet.Phi();
  }

  if (puppiMetName_.Length() != 0) {
    auto& puppiMet(*getSource<mithep::PFMetCol>(puppiMetName_)->At(0));
    out_.metPuppi->SetXYZT(puppiMet.Px(), puppiMet.Py(), puppiMet.Elongitudinal(), puppiMet.SumEt());
  }

  if (puppiRawMetName_.Length() != 0) {
    auto& puppiRawMet(*getSource<mithep::PFMetCol>(puppiRawMetName_)->At(0));
    out_.sumEtRawPuppi = puppiRawMet.SumEt();
  }

  if (genMetName_.Length() != 0 && getSource<mithep::EventHeader>(Names::gkEvtHeaderBrn)->IsMC()) {
    auto& genMet(*getSource<mithep::MetCol>(genMetName_)->At(0));
    newP4(*out_.genP4, genMet);
  }
}
