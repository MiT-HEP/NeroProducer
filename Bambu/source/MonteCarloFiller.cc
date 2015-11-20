#include "NeroProducer/Bambu/interface/MonteCarloFiller.h"

#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitAna/DataTree/interface/MCRunInfo.h"
#include "MitAna/DataTree/interface/PileupInfoCol.h"
#include "MitAna/DataTree/interface/MCParticleCol.h"
#include "MitAna/DataTree/interface/GenJetCol.h"

#include "TTree.h"

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstring>

ClassImp(mithep::nero::MonteCarloFiller)

void
mithep::nero::MonteCarloFiller::begin()
{
  auto* mcRunInfo = getSource<mithep::MCRunInfo>(Names::gkMCRunInfoBrn);

  if (pdfReweightGroupNames_.size() != 0 || pdfReweightGroupIds_.size() != 0) {
    // first convert group names to ids
    for (auto& name : pdfReweightGroupNames_) {
      unsigned iG = 0;
      for (; iG != mcRunInfo->NWeightGroups(); ++iG) {
        if (name == mcRunInfo->WeightGroupType(iG))
          break;
      }
      if (iG == mcRunInfo->NWeightGroups())
        throw std::runtime_error((name + " not found").Data());

      if (std::find(pdfReweightGroupIds_.begin(), pdfReweightGroupIds_.end(), iG) == pdfReweightGroupIds_.end())
        pdfReweightGroupIds_.push_back(iG);
    }

    // we don't need this info any more
    pdfReweightGroupNames_.clear();

    outputFile_->cd();
    auto* namesTree = new TTree("pdfReweight", "PDF set names");
    char weightDef[256];
    namesTree->Branch("weightDef", weightDef, "weightDef/C");

    for (unsigned gid : pdfReweightGroupIds_) {
      for (unsigned iC = 0; iC != mcRunInfo->NWeights(); ++iC) {
        if (mcRunInfo->WeightGroup(iC) != gid)
          continue;

        pdfReweightId_.push_back(mcRunInfo->WeightPositionInEvent(iC));

        strcpy(weightDef, mcRunInfo->WeightDefinition(iC));
        namesTree->Fill();
      }
    }

    // we don't need this info any more
    pdfReweightGroupIds_.clear();

    outputFile_->cd();
    namesTree->Write();
    delete namesTree;
  }

  if (pdfReweightId_.size() != 0) {
    unsigned maxId(0);
    for (unsigned id : pdfReweightId_) {
      if (id > maxId)
        maxId = id;
    }

    if (mcRunInfo->NWeights() <= maxId)
      std::cerr << "Current file has fewer PDF reweight ids than is expected!!";
  }
}

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

  if (mcEventInfo->NReweightScaleFactors() > 8) {
    out_.r1f2 = mcEventInfo->ReweightScaleFactor(1);
    out_.r1f5 = mcEventInfo->ReweightScaleFactor(2);
    out_.r2f1 = mcEventInfo->ReweightScaleFactor(3);
    out_.r2f2 = mcEventInfo->ReweightScaleFactor(4);
    out_.r5f1 = mcEventInfo->ReweightScaleFactor(6);
    out_.r5f5 = mcEventInfo->ReweightScaleFactor(8);
  }
  for (unsigned id : pdfReweightId_) {
    if (id < mcEventInfo->NReweightScaleFactors())
      out_.pdfRwgt->push_back(mcEventInfo->ReweightScaleFactor(id));
  }

  auto* puInfo = getSource<mithep::PileupInfoCol>("PileupInfo");
  out_.puTrueInt = 0;
  for (unsigned iP = 0; iP != puInfo->GetEntries(); ++iP) {
    auto& pu(*puInfo->At(iP));
    if (pu.GetBunchCrossing() == 0)
      out_.puTrueInt += pu.GetPU_NumMean();
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

    if ((part.Status() == 1 && partIs(part, {MCParticle::kEl, MCParticle::kMu, MCParticle::kGamma, MCParticle::kElNu, MCParticle::kMuNu, MCParticle::kTauNu, 1000022})) ||
        (part.Status() != 1 && part.StatusFlag(12) && partIs(part, {MCParticle::kTau, MCParticle::kZ, MCParticle::kW, MCParticle::kH, MCParticle::kHp, MCParticle::kUp, MCParticle::kDown, MCParticle::kStrange, MCParticle::kCharm, MCParticle::kBottom, MCParticle::kTop}))) { // 12 = kIsFirstCopy
        newP4(out_, part);
        out_.pdgId->push_back(part.PdgId());
    }
  }

  auto* genJets = getSource<mithep::GenJetCol>(genJetsName_, false);
  if (genJets) {
    minPt = std::max(20., minGenJetPt_);

    for (unsigned iJ(0); iJ != genJets->GetEntries(); ++iJ) {
      auto& jet(*genJets->At(iJ));
      if (jet.Pt() < minPt)
        continue;

      newP4(*out_.jetP4, jet);
    }
  }
}
