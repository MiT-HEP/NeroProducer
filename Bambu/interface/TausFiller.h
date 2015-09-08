#ifndef NeroProducer_Bambu_TausFiller_h
#define NeroProducer_Bambu_TausFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareTaus.hpp"

namespace mithep {
  namespace nero {

    class TausFiller : public BaseFiller {
    public:
      TausFiller() {}
      ~TausFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kTaus; }

      void fill() override;

      void SetTausName(char const* _name) { tausName_ = _name; }
      void SetExtended(Bool_t _ex = kTRUE) { out_.SetExtend(_ex); }

    private:
      BareTaus out_{};

      TString tausName_ = "HPSTaus";

      ClassDef(TausFiller, 0)
    };

  }
}

#endif
