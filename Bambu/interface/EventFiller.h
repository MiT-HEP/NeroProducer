#ifndef NeroProducer_Bambu_EventFiller_h
#define NeroProducer_Bambu_EventFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareEvent.hpp"

#include "MitAna/DataTree/interface/PileupEnergyDensity.h"

namespace mithep {
  namespace nero {

    class EventFiller : public BaseFiller {
    public:
      EventFiller() {}
      ~EventFiller() {}

      BareCollection* getObject() override { return &out_; }
      mithep::nero::Collection collection() const override { return kEvent; }

      void fill() override;

      void SetRhoAlgo(mithep::PileupEnergyDensity::Algo _algo) { rhoAlgo_ = _algo; }

    private:
      BareEvent out_{};

      mithep::PileupEnergyDensity::Algo rhoAlgo_ = mithep::PileupEnergyDensity::kFixedGridFastjetAll;

      ClassDef(EventFiller, 0)
    };

  }
}

#endif
