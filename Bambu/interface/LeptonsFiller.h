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
      void SetElectronIdsName(char const* _name) { electronIdsName_ = _name; }
      void SetMuonIdsName(char const* _name) { muonIdsName_ = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetNoPUPFCandsName(char const* _name) { nopuPFCandsName_ = _name; }
      void SetPUPFCandsName(char const* _name) { puPFCandsName_ = _name; }

    private:
      BareLeptons out_{};

      TString electronsName_ = "Electrons";
      TString muonsName_ = "Muons";
      TString electronIdsName_ = "ElectronIds";
      TString muonIdsName_ = "MuonIds";
      TString verticesName_ = "PrimaryVertexes";
      TString pfCandsName_ = "PFCandidates";
      TString nopuPFCandsName_ = "PFNoPileup";
      TString puPFCandsName_ = "PFPileup";

      ClassDef(LeptonsFiller, 0)
    };

  }
}

#endif
