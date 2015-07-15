#ifndef NeroProducer_Bambu_TriggerConfig_h
#define NeroProducer_Bambu_TriggerConfig_h

#include "NeroProducer/Bambu/interface/BaseConfig.h"

#include "TString.h"

#include <vector>

namespace mithep {
  namespace nero {
    
    class TriggerConfig : public BaseConfig {
    public:
      TriggerConfig() {}
      ~TriggerConfig() {}

      mithep::nero::Collection collection() const override { return mithep::nero::kTrigger; }
      std::pair<BareCollection*, BaseFiller*> create() const override;

      std::vector<TString> triggerNames;

      ClassDef(TriggerConfig, 0)
    };

  }
}

#endif
