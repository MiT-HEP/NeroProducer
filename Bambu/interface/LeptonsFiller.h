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
      void SetVetoMuonIdName(char const* _name) { vetoMuonIdName_ = _name; }
      void SetFakeMuonIdName(char const* _name) { fakeMuonIdName_ = _name; }
      void SetSoftMuonIdName(char const* _name) { softMuonIdName_ = _name; }
      void SetTightMuonIdName(char const* _name) { tightMuonIdName_ = _name; }
      void SetVetoElectronIdName(char const* _name) { vetoElectronIdName_ = _name; }
      void SetFakeElectronIdName(char const* _name) { fakeElectronIdName_ = _name; }
      void SetMediumElectronIdName(char const* _name) { mediumElectronIdName_ = _name; }
      void SetTightElectronIdName(char const* _name) { tightElectronIdName_ = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetNoPUPFCandsName(char const* _name) { nopuPFCandsName_ = _name; }
      void SetPUPFCandsName(char const* _name) { puPFCandsName_ = _name; }

    private:
      BareLeptons out_{};

      TString electronsName_ = "Electrons";
      TString muonsName_ = "Muons";
      TString vetoMuonIdName_ = "MuonIdsA";
      TString fakeMuonIdName_ = "MuonIdsB";
      TString softMuonIdName_ = "MuonIdsC";
      TString tightMuonIdName_ = "MuonIdsD";
      TString vetoElectronIdName_ = "ElectronIdsA";
      TString fakeElectronIdName_ = "ElectronIdsB";
      TString mediumElectronIdName_ = "ElectronIdsC";
      TString tightElectronIdName_ = "ElectronIdsD";
      TString verticesName_ = "PrimaryVertexes";
      TString pfCandsName_ = "PFCandidates";
      TString nopuPFCandsName_ = "PFNoPileup";
      TString puPFCandsName_ = "PFPileup";

      ClassDef(LeptonsFiller, 0)
    };

  }
}

#endif
