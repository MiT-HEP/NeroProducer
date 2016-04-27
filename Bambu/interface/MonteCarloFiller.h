#ifndef NeroProducer_Bambu_MonteCarloFiller_h
#define NeroProducer_Bambu_MonteCarloFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"

#include "MitAna/DataTree/interface/Names.h"

#include "TH1D.h"

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
      void AddPdfReweightGroupName(char const* _name) { pdfReweightGroupNames_.push_back(_name); }
      void AddPdfReweightGroupId(UInt_t _id) { pdfReweightGroupIds_.push_back(_id); }
      void AddPdfReweightIndex(UInt_t _idx) { pdfReweightIndices_.push_back(_idx); }
      void SetMinParticlePt(Double_t _min) { minParticlePt_ = _min; }
      void SetMinGenJetPt(Double_t _min) { minGenJetPt_ = _min; }

      // used by AllFiller
      std::vector<unsigned> const& getPdfReweightIndices() { return pdfReweightIndices_; }

    private:
      BareMonteCarlo out_{};

      TString mcParticlesName_ = mithep::Names::gkMCPartBrn;
      TString genJetsName_ = mithep::Names::gkGenJetBrn;
      
      std::vector<TString> pdfReweightGroupNames_{};
      std::vector<UInt_t> pdfReweightGroupIds_{};

      Double_t minParticlePt_ = 0.;
      Double_t minGenJetPt_ = 0.;

      std::vector<UInt_t> pdfReweightIndices_;

      ClassDef(MonteCarloFiller, 0)
    };

  }
}

#endif
