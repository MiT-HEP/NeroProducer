#ifndef NeroProducer_Bambu_MonteCarloFiller_h
#define NeroProducer_Bambu_MonteCarloFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"

namespace mithep {
  namespace nero {

    class MonteCarloFiller : public BaseFiller {
    public:
      MonteCarloFiller() {}
      ~MonteCarloFiller() {}

      BareCollection* getObject() override { return &out_; }
      mithep::nero::Collection collection() const override { return kMonteCarlo; }

      void fill() override;

    private:
      BareMonteCarlo out_{};

      ClassDef(MonteCarloFiller, 0)
    };

  }
}

#endif
