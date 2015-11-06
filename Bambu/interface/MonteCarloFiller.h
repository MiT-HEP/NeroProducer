#ifndef NeroProducer_Bambu_MonteCarloFiller_h
#define NeroProducer_Bambu_MonteCarloFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"

#include "MitAna/DataTree/interface/Names.h"

namespace mithep {
  namespace nero {

    class MonteCarloFiller : public BaseFiller {
    public:
      MonteCarloFiller() {}
      ~MonteCarloFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kMonteCarlo; }

      void begin() override;
      void fill() override;

      void SetMCParticlesName(char const* _name) { mcParticlesName_ = _name; }
      void SetGenJetsName(char const* _name) { genJetsName_ = _name; }
      void SetPdfReweightName(char const* _name) { pdfReweightGroup_ = _name; }
      void SetMinParticlePt(Double_t _min) { minParticlePt_ = _min; }
      void SetMinGenJetPt(Double_t _min) { minGenJetPt_ = _min; }

    private:
      BareMonteCarlo out_{};

      TString mcParticlesName_ = mithep::Names::gkMCPartBrn;
      TString genJetsName_ = mithep::Names::gkGenJetBrn;
      TString pdfReweightGroup_ = "";

      Double_t minParticlePt_ = 0.;
      Double_t minGenJetPt_ = 0.;

      std::vector<unsigned> pdfReweightId_;

      ClassDef(MonteCarloFiller, 0)
    };

  }
}

#endif
