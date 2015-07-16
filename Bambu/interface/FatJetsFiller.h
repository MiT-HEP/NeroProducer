#ifndef NeroProducer_Bambu_FatJetsFiller_h
#define NeroProducer_Bambu_FatJetsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareFatJets.hpp"

namespace mithep {
  namespace nero {

    class FatJetsFiller : public BaseFiller {
    public:
      FatJetsFiller() {}
      ~FatJetsFiller() {}

      BareCollection* getObject() override { return &out_; }
      mithep::nero::Collection collection() const override { return kFatJets; }

      void fill() override;

      // must be corrected, loose-id jets
      void SetFatJetsName(char const* _name) { fatJetsName_ = _name; }

    private:
      BareFatJets out_{};

      TString fatJetsName_ = "AKt8PFJetsCHS";

      ClassDef(FatJetsFiller, 0)
    };

  }
}

#endif
