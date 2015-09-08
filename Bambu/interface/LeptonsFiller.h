#ifndef NeroProducer_Bambu_LeptonsFiller_h
#define NeroProducer_Bambu_LeptonsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareLeptons.hpp"

namespace mithep {
  namespace nero {

    class LeptonsFiller : public BaseFiller {
    public:
      LeptonsFiller() {}
      ~LeptonsFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kLeptons; }

      void fill() override;

      void SetElectronsName(char const* _name) { electronsName_ = _name; }
      void SetMuonsName(char const* _name) { muonsName_ = _name; }
      void SetMuonIdsAName(char const* _name) { muonIdsAName_ = _name; }
      void SetMuonIdsBName(char const* _name) { muonIdsBName_ = _name; }
      void SetMuonIdsCName(char const* _name) { muonIdsCName_ = _name; }
      void SetMuonIdsDName(char const* _name) { muonIdsDName_ = _name; }
      void SetElectronIdsAName(char const* _name) { electronIdsAName_ = _name; }
      void SetElectronIdsBName(char const* _name) { electronIdsBName_ = _name; }
      void SetElectronIdsCName(char const* _name) { electronIdsCName_ = _name; }
      void SetElectronIdsDName(char const* _name) { electronIdsDName_ = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetNoPUPFCandsName(char const* _name) { nopuPFCandsName_ = _name; }
      void SetPUPFCandsName(char const* _name) { puPFCandsName_ = _name; }

    private:
      BareLeptons out_{};

      TString electronsName_ = "Electrons";
      TString muonsName_ = "Muons";
      TString muonIdsAName_ = "MuonIdsA";
      TString muonIdsBName_ = "MuonIdsB";
      TString muonIdsCName_ = "MuonIdsC";
      TString muonIdsDName_ = "MuonIdsD";
      TString electronIdsAName_ = "ElectronIdsA";
      TString electronIdsBName_ = "ElectronIdsB";
      TString electronIdsCName_ = "ElectronIdsC";
      TString electronIdsDName_ = "ElectronIdsD";
      TString verticesName_ = "PrimaryVertexes";
      TString pfCandsName_ = "PFCandidates";
      TString nopuPFCandsName_ = "PFNoPileup";
      TString puPFCandsName_ = "PFPileup";

      ClassDef(LeptonsFiller, 0)
    };

  }
}

#endif
