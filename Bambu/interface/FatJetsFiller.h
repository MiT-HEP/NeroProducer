#ifndef NeroProducer_Bambu_FatJetsFiller_h
#define NeroProducer_Bambu_FatJetsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareFatJets.hpp"

namespace mithep {
  namespace nero {

    class FatJetsFiller : public BaseFiller {
    public:
      FatJetsFiller(BaseFiller::Collection col) : collection_(col) {}
      ~FatJetsFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return collection_; }

      void defineBranches(TTree*) override;

      void fill() override;

      // must be corrected, loose-id jets
      void SetFatJetsName(char const* _name) { fatJetsName_ = _name; }

    private:
      BareFatJets out_{};

      BaseFiller::Collection collection_;

      TString fatJetsName_ = "AKt8PFJetsCHS";

      ClassDef(FatJetsFiller, 0)
    };

  }
}

#endif
