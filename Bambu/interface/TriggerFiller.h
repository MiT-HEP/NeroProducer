#ifndef NeroProducer_Bambu_TriggerFiller_h
#define NeroProducer_Bambu_TriggerFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareTrigger.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

namespace mithep {
  namespace nero {

    class TriggerFiller : public BaseFiller {
    public:
      TriggerFiller() {}
      ~TriggerFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kTrigger; }

      void initialize() override;
      void setCrossRef(BaseFiller* fillers[]) override;
      void begin() override;
      void fill() override;

      std::vector<TString> const& triggerNames() const { return triggerNames_; }

      void AddTriggerName(char const* path);
      void AddFilterName(char const* path, char const* filter);

    private:
      BareTrigger out_{};

      std::vector<TString> triggerNames_{};
      std::vector<unsigned> triggerIds_{};

      std::vector<std::vector<TString>> triggerObjectsNames_{};

      BareP4* leptons_ = 0;
      BareP4* taus_ = 0;
      BareP4* jets_ = 0;
      BareP4* photons_ = 0;

      ClassDef(TriggerFiller, 0)
    };

  }
}

#endif
