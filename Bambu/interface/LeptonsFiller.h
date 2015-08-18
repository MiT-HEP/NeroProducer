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
      mithep::nero::Collection collection() const override { return kLeptons; }

      void fill() override;

      void SetElectronsName(char const* _name) { electronsName_ = _name; }
      void SetMuonsName(char const* _name) { muonsName_ = _name; }
      void SetElectronIdsAName(char const* _name) { electronIdsAName_ = _name; }
      void SetMuonIdsAName(char const* _name) { muonIdsAName_ = _name; }
      void SetElectronIdsBName(char const* _name) { electronIdsBName_ = _name; }
      void SetMuonIdsBName(char const* _name) { muonIdsBName_ = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetNoPUPFCandsName(char const* _name) { nopuPFCandsName_ = _name; }
      void SetPUPFCandsName(char const* _name) { puPFCandsName_ = _name; }

    private:
      BareLeptons out_{};

      TString electronsName_ = "Electrons";
      TString muonsName_ = "Muons";
      TString electronIdsAName_ = "ElectronIdsA";
      TString muonIdsAName_ = "MuonIdsA";
      TString electronIdsBName_ = "ElectronIdsB";
      TString muonIdsBName_ = "MuonIdsB";
      TString verticesName_ = "PrimaryVertexes";
      TString pfCandsName_ = "PFCandidates";
      TString nopuPFCandsName_ = "PFNoPileup";
      TString puPFCandsName_ = "PFPileup";

      ClassDef(LeptonsFiller, 0)
    };

  }
}

#endif
