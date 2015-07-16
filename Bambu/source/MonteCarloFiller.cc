#include "NeroProducer/Bambu/interface/MonteCarloFiller.h"

#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitAna/DataTree/interface/PileupInfoCol.h"
#include "MitAna/DataTree/interface/MCParticleCol.h"
#include "MitAna/DataTree/interface/GenJetCol.h"

#include <vector>

ClassImp(mithep::nero::MonteCarloFiller)

void
mithep::nero::MonteCarloFiller::fill()
{
  auto* eventHeader = getSource<mithep::EventHeader>(Names::gkEvtHeaderBrn);
  if (!eventHeader->IsMC())
    return;

  auto* mcEventInfo = getSource<mithep::MCEventInfo>(Names::gkMCEvtInfoBrn);
  out_.mcWeight = mcEventInfo->Weight();
  out_.qScale = mcEventInfo->Scale();
  out_.alphaQED = mcEventInfo->AlphaQED();
  out_.alphaQCD = mcEventInfo->AlphaQCD();
  out_.x1 = mcEventInfo->X1();
  out_.x2 = mcEventInfo->X2();
  out_.pdf1Id = mcEventInfo->Pdf1();
  out_.pdf2Id = mcEventInfo->Pdf2();
  out_.scalePdf = mcEventInfo->ScalePdf();

  auto* puInfo = getSource<mithep::PileupInfoCol>("PileupInfo");
  out_.puTrueInt = 0;
  for (unsigned iP = 0; iP != puInfo->GetEntries(); ++iP) {
    auto& pu(*puInfo->At(iP));
    if (pu.GetBunchCrossing() == 0)
      out_.puTrueInt += pu.GetPU_NumInteractions();
  }

  double minPt(std::max(5., minParticlePt_));

  auto partIs = [](mithep::MCParticle const& _part, std::vector<unsigned> _idList)->bool {
    for (unsigned id : _idList) {
      if (_part.Is(id))
        return true;
    }
    return false;
  };

  auto* mcParticles = getSource<mithep::MCParticleCol>(mcParticlesName_);
  for (unsigned iP(0); iP != mcParticles->GetEntries(); ++iP) {
    auto& part(*mcParticles->At(iP));

    if (part.Pt() < minPt)
      continue;

    if ((part.Status() == 1 && partIs(part, {MCParticle::kEl, MCParticle::kMu, MCParticle::kGamma})) ||
        (part.Status() != 1 && part.StatusFlag(12) && partIs(part, {MCParticle::kTau, MCParticle::kZ, MCParticle::kW, MCParticle::kH, MCParticle::kHp}))) {
        newP4(out_, part);
        out_.pdgId->push_back(part.PdgId());
    }
  }

  minPt = std::max(20., minGenJetPt_);

  auto* genJets = getSource<mithep::GenJetCol>(genJetsName_);
  for (unsigned iJ(0); iJ != genJets->GetEntries(); ++iJ) {
    auto& jet(*genJets->At(iJ));
    if (jet.Pt() < minPt)
      continue;

    newP4(out_, jet);
  }
}
