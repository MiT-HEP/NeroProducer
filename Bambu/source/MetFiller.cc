#include "NeroProducer/Bambu/interface/MetFiller.h"

#include "MitAna/DataTree/interface/MetCol.h"
#include "MitAna/DataTree/interface/GenMetCol.h"
#include "MitAna/DataTree/interface/EventHeader.h"
#include "MitAna/DataTree/interface/Names.h"

ClassImp(mithep::nero::MetFiller)

void
mithep::nero::MetFiller::fill()
{
  auto* metCol = getSource<mithep::MetCol>(metName_);
  auto& met(*metCol->At(0));

  newP4(out_, met.Px(), met.Py(), met.Pz(), met.E());

  // TODO
  out_.ptJESUP->push_back(0.);
  out_.ptJESDOWN->push_back(0.);

  if (getSource<mithep::EventHeader>(Names::gkEvtHeaderBrn)->IsMC()) {
    auto* genMetCol = getSource<mithep::GenMetCol>(genMetName_);
    auto& genMet(*genMetCol->At(0));

    newP4(*out_.genP4, genMet.Px(), genMet.Py(), genMet.Pz(), genMet.E());
  }
}
