#ifndef NeroProducer_Bambu_MetFiller_h
#define NeroProducer_Bambu_MetFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareMet.hpp"

namespace mithep {
  namespace nero {

    class MetFiller : public BaseFiller {
    public:
      MetFiller() {}
      ~MetFiller() {}

      BareCollection* getObject() override { return &out_; }
      mithep::nero::Collection collection() const override { return kMet; }

      void fill() override;

      void SetMetName(char const* _name) { metName_ = _name; }
      void SetMuonsName(char const* _name) { muonsName_ = _name; }
      void SetGenMetName(char const* _name) { genMetName_ = _name; }

    private:
      BareMet out_{};

      TString metName_ = "PFMet";
      TString muonsName_ = "Muons";
      TString genMetName_ = "GenMet";

      ClassDef(MetFiller, 0)
    };

  }
}

#endif
