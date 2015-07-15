#include "NeroProducer/Bambu/interface/TriggerConfig.h"

ClassImp(mithep::nero::TriggerConfig)

std::pair<BareCollection*, mithep::nero::BaseFiller*>
mithep::nero::TriggerConfig::create() const
{
  return std::pair<BareCollection*, mithep::nero::BaseFiller*>(0, 0);
}
