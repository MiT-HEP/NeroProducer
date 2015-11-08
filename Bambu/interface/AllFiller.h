#ifndef NeroProducer_Bambu_AllFiller_h
#define NeroProducer_Bambu_AllFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareAll.hpp"

#include "MitAna/DataTree/interface/EventHeaderFwd.h"

namespace mithep {
  namespace nero {

    class AllFiller : public BaseFiller {
    public:
      AllFiller() {}
      ~AllFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kAll; }

      void initialize() override;
      void fill() override;

    private:
      BareAll out_{};

      EventHeaderCol const* skippedEvents_ = 0;

      ClassDef(AllFiller, 0)
    };

  }
}

#endif
