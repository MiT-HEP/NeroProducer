#include "NeroProducer/Bambu/interface/AllFiller.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitAna/DataTree/interface/PileupInfoCol.h"

#include <stdexcept>

ClassImp(mithep::nero::AllFiller)

void
mithep::nero::AllFiller::fill()
{
  auto* eventHeader = getSource<mithep::EventHeader>(Names::gkEvtHeaderBrn);
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
        out_.puTrueInt += pu.GetPU_NumInteractions();
    }

    out_.mcWeight = mcInfo->Weight();
  }
}
