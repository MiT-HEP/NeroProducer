#ifndef NeroProducer_Bambu_MetFiller_h
#define NeroProducer_Bambu_MetFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareMet.hpp"

namespace mithep {
  namespace nero {

    class MetFiller : public BaseFiller {
    public:
      MetFiller() { out_.SetExtend(); }
      ~MetFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kMet; }

      void fill() override;

      void SetMetName(char const* _name) { metName_ = _name; }
      void SetJESUpMetName(char const* _name) { jesUpMetName_ = _name; }
      void SetJESDownMetName(char const* _name) { jesDownMetName_ = _name; }
      void SetMuonsName(char const* _name) { muonsName_ = _name; }
      void SetPFCandidatesName(char const* _name) { pfCandidatesName_ = _name; }
      void SetGenMetName(char const* _name) { genMetName_ = _name; }

    private:
      BareMet out_{};

      TString metName_ = "PFMet";
      TString jesUpMetName_ = "";
      TString jesDownMetName_ = "";
      TString muonsName_ = "Muons";
      TString pfCandidatesName_ = "PFCandidates";
      TString genMetName_ = "GenMet";

      ClassDef(MetFiller, 0)
    };

  }
}

#endif
