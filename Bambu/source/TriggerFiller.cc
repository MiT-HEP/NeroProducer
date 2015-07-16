#include "NeroProducer/Bambu/interface/TriggerFiller.h"

#include "MitAna/DataTree/interface/Names.h"

ClassImp(mithep::nero::TriggerFiller)

void
mithep::nero::TriggerFiller::setCrossRef(BaseFiller* _fillers[])
{
  // need dynamic cast here because BareP4 inherits virtually from BareCollection
  leptons_ = dynamic_cast<BareP4*>(_fillers[kLeptons]->getObject());
  taus_ = dynamic_cast<BareP4*>(_fillers[kTaus]->getObject());
  jets_ = dynamic_cast<BareP4*>(_fillers[kJets]->getObject());
  photons_ = dynamic_cast<BareP4*>(_fillers[kPhotons]->getObject());
}

void
mithep::nero::TriggerFiller::fill()
{
  // TODO
}
