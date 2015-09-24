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
      void SetVetoMuonIdName(char const* _name) { setDefinedId_(muonIdName_, BareLeptons::LepVeto, _name); }
      void SetFakeMuonIdName(char const* _name) { setDefinedId_(muonIdName_, BareLeptons::LepFake, _name); }
      void SetSoftMuonIdName(char const* _name) { setDefinedId_(muonIdName_, BareLeptons::LepSoft, _name); }
      void SetLooseMuonIdName(char const* _name) { setDefinedId_(muonIdName_, BareLeptons::LepLoose, _name); }
      void SetMediumMuonIdName(char const* _name) { setDefinedId_(muonIdName_, BareLeptons::LepMedium, _name); }
      void SetTightMuonIdName(char const* _name) { setDefinedId_(muonIdName_, BareLeptons::LepTight, _name); }
      void SetMuonIdName(UInt_t _bit, char const* _name) { muonIdName_[_bit] = _name; }
      void SetVetoElectronIdName(char const* _name) { setDefinedId_(electronIdName_, BareLeptons::LepVeto, _name); }
      void SetFakeElectronIdName(char const* _name) { setDefinedId_(electronIdName_, BareLeptons::LepFake, _name); }
      void SetSoftElectronIdName(char const* _name) { setDefinedId_(electronIdName_, BareLeptons::LepSoft, _name); }
      void SetLooseElectronIdName(char const* _name) { setDefinedId_(electronIdName_, BareLeptons::LepLoose, _name); }
      void SetMediumElectronIdName(char const* _name) { setDefinedId_(electronIdName_, BareLeptons::LepMedium, _name); }
      void SetTightElectronIdName(char const* _name) { setDefinedId_(electronIdName_, BareLeptons::LepTight, _name); }
      void SetElectronIdName(UInt_t _bit, char const* _name) { electronIdName_[_bit] = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetNoPUPFCandsName(char const* _name) { nopuPFCandsName_ = _name; }
      void SetPUPFCandsName(char const* _name) { puPFCandsName_ = _name; }

    private:
      BareLeptons out_{};
      void setDefinedId_(TString [], BareLeptons::Selection, char const* _name);

      TString electronsName_ = "Electrons";
      TString muonsName_ = "Muons";
      TString muonIdName_[32]{};
      TString electronIdName_[32]{};
      TString verticesName_ = "PrimaryVertexes";
      TString pfCandsName_ = "PFCandidates";
      TString nopuPFCandsName_ = "PFNoPileup";
      TString puPFCandsName_ = "PFPileup";

      ClassDef(LeptonsFiller, 0)
    };

  }
}

#endif
