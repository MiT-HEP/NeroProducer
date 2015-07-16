#ifndef NeroProducer_Bambu_JetsFiller_h
#define NeroProducer_Bambu_JetsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareJets.hpp"

namespace mithep {
  namespace nero {

    class JetsFiller : public BaseFiller {
    public:
      JetsFiller() {}
      ~JetsFiller() {}

      BareCollection* getObject() override { return &out_; }
      mithep::nero::Collection collection() const override { return kJets; }

      void fill() override;

      // must be corrected, loose-id jets
      void SetJetsName(char const* _name) { jetsName_ = _name; }

    private:
      BareJets out_{};

      TString jetsName_ = "AKt4PFJetsCHS";

      ClassDef(JetsFiller, 0)
    };

  }
}

#endif
