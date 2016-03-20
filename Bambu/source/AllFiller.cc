#include "NeroProducer/Bambu/interface/AllFiller.h"
#include "NeroProducer/Bambu/interface/MonteCarloFiller.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EventHeaderCol.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitAna/DataTree/interface/PileupInfoCol.h"

#include <stdexcept>

ClassImp(mithep::nero::AllFiller)

void
mithep::nero::AllFiller::setCrossRef(BaseFiller* _fillers[])
{
  if (_fillers[kMonteCarlo])
    pdfReweightIndices_ = &static_cast<MonteCarloFiller*>(_fillers[kMonteCarlo])->getPdfReweightIndices();
}

void
mithep::nero::AllFiller::initialize()
{
  skippedEvents_ = getSource<mithep::EventHeaderCol>(mithep::Names::gkSkimmedHeaders, false);
}

void
mithep::nero::AllFiller::finalize()
{
  if (pdfReweightSums_) {
    outputFile_->cd();
    pdfReweightSums_->Write();
  }
}

void
mithep::nero::AllFiller::begin()
{
  if (pdfReweightIndices_) { // MonteCarloFiller is set up
    if (!scaleReweightSums_) {
      scaleReweightSums_ = new TH1D("scaleReweightSums", "", 6, 0., 6.);
      scaleReweightSums_->Sumw2();
      int iBin = 0;
      for (TString label : {"r1f2", "r1f5", "r2f1", "r2f2", "r5f1", "r5f5"})
        scaleReweightSums_->GetXaxis()->SetBinLabel(++iBin, label);
    }
  
    // AllFiller::begin is called after MonteCarloFiller::begin
    if (!pdfReweightSums_ && pdfReweightIndices_->size() != 0) {
      outputFile_->cd();
      pdfReweightSums_ = new TH1D("pdfReweightSums", "", pdfReweightIndices_->size(), 0., pdfReweightIndices_->size());
      pdfReweightSums_->Sumw2();
    }
  }
}

void
mithep::nero::AllFiller::fill()
{
  if (skippedEvents_) {
    for (unsigned iS(0); iS != skippedEvents_->GetEntries(); ++iS) {
      auto& header(*skippedEvents_->At(iS));

      out_.isRealData = !header.IsMC();
      out_.runNum = header.RunNum();
      out_.lumiNum = header.LumiSec();
      out_.eventNum = header.EvtNum();
      out_.puTrueInt = 0;
      out_.mcWeight = header.Weight();
    }
  }

  auto* eventHeader = getSource<mithep::EventHeader>(mithep::Names::gkEvtHeaderBrn);
  if (!eventHeader)
    throw runtime_error("EventHeader not found");

  out_.isRealData = !eventHeader->IsMC();
  out_.runNum = eventHeader->RunNum();
  out_.lumiNum = eventHeader->LumiSec();
  out_.eventNum = eventHeader->EvtNum();
  out_.puTrueInt = 0;
  out_.mcWeight = 1.;

  if (eventHeader->IsMC()) {
    auto* puInfo = getSource<mithep::PileupInfoCol>("PileupInfo");
    auto* mcInfo = getSource<mithep::MCEventInfo>(Names::gkMCEvtInfoBrn);

    if (!puInfo)
      throw runtime_error("PUInfo not found");

    if (!mcInfo)
      throw runtime_error("MCInfo not found");

    for (unsigned iP = 0; iP != puInfo->GetEntries(); ++iP) {
      auto& pu(*puInfo->At(iP));
      if (pu.GetBunchCrossing() == 0)
        out_.puTrueInt += pu.GetPU_NumMean();
    }

    out_.mcWeight = mcInfo->Weight();

    if (pdfReweightIndices_) {
      if (mcInfo->NReweightScaleFactors() > 8) {
        int iBin = 0;
        for (int iF : {1, 2, 3, 4, 6, 8})
          scaleReweightSums_->Fill((++iBin) - 0.5, mcInfo->ReweightScaleFactor(iF));
      }

      if (pdfReweightSums_) {
        for (unsigned iP(0); iP != pdfReweightIndices_->size(); ++iP) {
          unsigned idx(pdfReweightIndices_->at(iP));
          if (idx < mcInfo->NReweightScaleFactors())
            pdfReweightSums_->Fill(iP + 0.5, mcInfo->ReweightScaleFactor(idx));
        }
      }
    }
  }
}
