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

      void initialize() override;
      void finalize() override;
      void fill() override;

      // must be corrected, loose-id jets
      void SetJetsName(char const* _name) { jetsName_ = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetJetIdCutWP(unsigned _p) { jetIdCutWP_ = _p; }
      void SetJetIdMVATrainingSet(unsigned _p) { jetIdMVATrainingSet_ = _p; }
      void SetJetIdMVAWeightsFile(char const* _path) { jetIdMVAWeightsFile_ = _path; }
      void SetJetIdCutsFile(char const* _path) { jetIdCutsFile_ = _path; }
      void SetJetIDMVA(mithep::JetIDMVA* _mva) { jetId_ = _mva; }

    private:
      BareJets out_{};

      TString jetsName_ = "AKt4PFJetsCHS";
      TString verticesName_ = ModNames::gkGoodVertexesName;
      JetIDMVA* jetId_ = 0;
      unsigned jetIdCutWP_ = JetIDMVA::kLoose;
      unsigned jetIdMVATrainingSet_ = JetIDMVA::nMVATypes;
      TString jetIdMVAWeightsFile_ = "";
      TString jetIdCutsFile_ = "";
      bool ownJetId_ = false;

      ClassDef(JetsFiller, 0)
    };

  }
}

#endif
