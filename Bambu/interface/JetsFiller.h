#ifndef NeroProducer_Bambu_JetsFiller_h
#define NeroProducer_Bambu_JetsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareJets.hpp"
#include "MitPhysics/Init/interface/ModNames.h"
#include "MitPhysics/Utils/interface/JetIDMVA.h"

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
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetJetIDMVA(mithep::JetIDMVA* _mva) { jetId_ = _mva; }

    private:
      BareJets out_{};

      TString jetsName_ = "AKt4PFJetsCHS";
      TString verticesName_ = ModNames::gkGoodVertexesName;
      mithep::JetIDMVA* jetId_ = 0; // not owned

      ClassDef(JetsFiller, 0)
    };

  }
}

#endif
