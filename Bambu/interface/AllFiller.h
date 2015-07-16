#ifndef NeroProducer_Bambu_AllFiller_h
#define NeroProducer_Bambu_AllFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareAll.hpp"

namespace mithep {
  namespace nero {

    class AllFiller : public BaseFiller {
    public:
      AllFiller() {}
      ~AllFiller() {}

      BareCollection* getObject() override { return &out_; }
      mithep::nero::Collection collection() const override { return kAll; }

      void fill() override;

    private:
      BareAll out_{};

      ClassDef(AllFiller, 0)
    };

  }
}

#endif
