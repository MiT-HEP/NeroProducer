#include "NeroProducer/Bambu/interface/AllFiller.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EventHeaderCol.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitAna/DataTree/interface/PileupInfoCol.h"

#include <stdexcept>

ClassImp(mithep::nero::AllFiller)

void
mithep::nero::AllFiller::initialize()
{
  skippedEvents_ = getSource<mithep::EventHeaderCol>(mithep::Names::gkSkimmedHeaders, false);
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
  }
}
