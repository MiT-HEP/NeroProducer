#ifndef NeroProducer_Bambu_FatJetsFiller_h
#define NeroProducer_Bambu_FatJetsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareFatJets.hpp"

#include "MitCommon/DataFormats/interface/Types.h"
#include "MitCommon/DataFormats/interface/Vect4M.h"
#include "MitPhysics/Utils/interface/NeuralNet.h"
#include "MitAna/DataTree/interface/FatJet.h"

namespace mithep {
  namespace nero {

    class FatJetsFiller : public BaseFiller {
    public:
      FatJetsFiller(BaseFiller::Collection col) : collection_(col), MVAOn(false) {}
      ~FatJetsFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return collection_; }

      void defineBranches(TTree*) override;

      void initialize() override;
      void finalize() override;
      void fill() override;
      
      float cleanInput(float x);
      float computePull(const FourVectorM &jetMom, const Vect4M &groomedMom );

      // must be corrected, loose-id jets
      void SetFatJetsName(char const* _name) { fatJetsName_ = _name; }

      void SetMVAOn(bool b) { MVAOn = b; }
      void SetMJIdOn(bool b) { applyMJId=b; }
      void SetExtendedOn(bool b) { extended = b; }

    private:
      bool fillFatJet(const FatJet &jet);
      BareFatJets out_{};

      BaseFiller::Collection collection_;

      TString fatJetsName_ = "AKt8PFJetsCHS";
      
      bool applyMJId=false;      
      bool MVAOn=false;
      bool extended=false;
      NeuralNet *topANN;
      float nn_mSD;
      float nn_QGTag;
      float nn_groomedIso;
      float nn_tau32;
      float nn_tau21;
      unsigned int subjetCounter;
      
      ClassDef(FatJetsFiller, 0)
    };

  }
}

#endif
