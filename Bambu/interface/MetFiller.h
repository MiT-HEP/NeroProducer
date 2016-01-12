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
      void SetPuppiMetName(char const* _name) { puppiMetName_ = _name; }
      void SetPuppiRawMetName(char const* _name) { puppiRawMetName_ = _name; }
      void SetRawMetName(char const* _name) { rawMetName_ = _name; }
      void SetCaloMetName(char const* _name) { caloMetName_ = _name; }
      void SetGenMetName(char const* _name) { genMetName_ = _name; }

    private:
      BareMet out_{};

      TString metName_ = "PFMet";
      TString jesUpMetName_ = "";
      TString jesDownMetName_ = "";
      TString muonsName_ = "Muons";
      TString pfCandidatesName_ = "PFCandidates";
      TString puppiMetName_ = "";
      TString puppiRawMetName_ = "";
      TString caloMetName_ = "CaloMet";
      TString rawMetName_ = "PFMet";
      TString genMetName_ = "";

      ClassDef(MetFiller, 0)
    };

  }
}

#endif
