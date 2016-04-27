#include "NeroProducer/Bambu/interface/EventFiller.h"

#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/PileupEnergyDensityCol.h"

ClassImp(mithep::nero::EventFiller)

void
mithep::nero::EventFiller::fill()
{
  auto* eventHeader = getSource<mithep::EventHeader>(Names::gkEvtHeaderBrn);

  out_.isRealData = !eventHeader->IsMC();
  out_.runNum = eventHeader->RunNum();
  out_.lumiNum = eventHeader->LumiSec();
  out_.eventNum = eventHeader->EvtNum();

  auto* rhoCol = getSource<mithep::PileupEnergyDensityCol>(Names::gkPileupEnergyDensityBrn);
  out_.rho = rhoCol->At(0)->Rho(rhoAlgo_);
}
