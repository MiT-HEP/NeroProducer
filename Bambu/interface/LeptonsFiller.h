#ifndef NeroProducer_Bambu_LeptonsFiller_h
#define NeroProducer_Bambu_LeptonsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareEvent.hpp"
#include "NeroProducer/Core/interface/BareLeptons.hpp"

#include "MitPhysics/Utils/interface/ElectronIDMVA.h"

#include <utility>
#include <vector>
#include <string>

namespace mithep {
  namespace nero {

    class LeptonsFiller : public BaseFiller {
      enum Lepton {
        kEl,
        kMu,
        nLeptons
      };

    public:
      LeptonsFiller() {}
      ~LeptonsFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kLeptons; }
      void setCrossRef(BaseFiller* _fillers[]);

      void initialize() override;
      void finalize() override;
      void fill() override;

      void SetElectronsName(char const* _name) { electronsName_ = _name; }
      void SetMuonsName(char const* _name) { muonsName_ = _name; }
      void SetBaselineMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepBaseline, _name, _saveAllPass); }
      void SetVetoMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepVeto, _name, _saveAllPass); }
      void SetFakeMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepFake, _name, _saveAllPass); }
      void SetSoftMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepSoft, _name, _saveAllPass); }
      void SetLooseMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepLoose, _name, _saveAllPass); }
      void SetMediumMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepMedium, _name, _saveAllPass); }
      void SetTightMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepTight, _name, _saveAllPass); }
      void SetLooseMuonIsoName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepLooseIso, _name, _saveAllPass); }
      void SetMediumMuonIsoName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepMediumIso, _name, _saveAllPass); }
      void SetTightMuonIsoName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepTightIso, _name, _saveAllPass); }
      void SetMediumIPMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepMediumIP, _name, _saveAllPass); }
      void SetTightIPMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepTightIP, _name, _saveAllPass); }
      void SetSoftIPMuonIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kMu, BareLeptons::LepSoftIP, _name, _saveAllPass); }
      void SetMuonIdName(UInt_t _bit, char const* _name, Bool_t _saveAllPass = kFALSE) { idName_[kMu][_bit] = _name; savePassing_[kMu][_bit] = _saveAllPass; }
      void SetBaselineElectronIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepBaseline, _name, _saveAllPass); }
      void SetVetoElectronIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepVeto, _name, _saveAllPass); }
      void SetFakeElectronIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepFake, _name, _saveAllPass); }
      void SetSoftElectronIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepSoft, _name, _saveAllPass); }
      void SetLooseElectronIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepLoose, _name, _saveAllPass); }
      void SetMediumElectronIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepMedium, _name, _saveAllPass); }
      void SetTightElectronIdName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepTight, _name, _saveAllPass); }
      void SetLooseElectronIsoName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepLooseIso, _name, _saveAllPass); }
      void SetMediumElectronIsoName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepMediumIso, _name, _saveAllPass); }
      void SetTightElectronIsoName(char const* _name, Bool_t _saveAllPass = kFALSE) { setDefinedId_(kEl, BareLeptons::LepTightIso, _name, _saveAllPass); }
      void SetElectronIdName(UInt_t _bit, char const* _name, Bool_t _saveAllPass = kFALSE) { idName_[kEl][_bit] = _name; savePassing_[kEl][_bit] = _saveAllPass; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetNoPUPFCandsName(char const* _name) { nopuPFCandsName_ = _name; }
      void SetPUPFCandsName(char const* _name) { puPFCandsName_ = _name; }
      void SetElectronMVA(char const* _methodName, UInt_t _type) { electronMVAConfig_.first = _methodName; electronMVAConfig_.second = _type; }
      void AddElectronMVAWeights(char const* _path) { electronMVAWeights_.push_back(_path); }

      char const* GetMuonIdName(UInt_t _bit) { return idName_[kMu][_bit]; }
      char const* GetElectronIdName(UInt_t _bit) { return idName_[kEl][_bit]; }

    private:
      BareLeptons out_{};
      void setDefinedId_(Lepton, BareLeptons::Selection, char const* name, Bool_t saveAllPass);

      TString electronsName_ = "Electrons";
      TString muonsName_ = "Muons";
      TString idName_[nLeptons][32]{};
      TString verticesName_ = "PrimaryVertexes";
      TString pfCandsName_ = "PFCandidates";
      TString nopuPFCandsName_ = "PFNoPileup";
      TString puPFCandsName_ = "PFPileup";
      Bool_t savePassing_[nLeptons][32]{};
      // method name, MVA type (enum)
      std::pair<std::string, UInt_t> electronMVAConfig_{"", ElectronIDMVA::kUninitialized};
      std::vector<std::string> electronMVAWeights_{};
      ElectronIDMVA* electronDiscriminator_{0};
      BareEvent* event_{0};

      ClassDef(LeptonsFiller, 0)
    };

  }
}

#endif
