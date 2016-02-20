#ifndef NeroProducer_Bambu_PhotonsFiller_h
#define NeroProducer_Bambu_PhotonsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BarePhotons.hpp"

#include "MitPhysics/Utils/interface/PhotonTools.h"

namespace mithep {
  namespace nero {

    class PhotonsFiller : public BaseFiller {
    public:
      PhotonsFiller() { out_.SetExtend(); }
      ~PhotonsFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kPhotons; }

      void setCrossRef(BaseFiller* fillers[]) override;
      void fill() override;

      void SetMaxIso(Double_t _max) { maxIso_ = _max; }
      void SetPhotonsName(char const* _name) { photonsName_ = _name; }
      void SetLooseIdName(char const* _name) { looseIdName_ = _name; }
      void SetMediumIdName(char const* _name) { mediumIdName_ = _name; }
      void SetTightIdName(char const* _name) { tightIdName_ = _name; }
      void SetHighPtIdName(char const* _name) { highPtIdName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      void SetIsoType(UInt_t _type) { isoType_ = _type; }

    private:
      BarePhotons out_{};

      Double_t maxIso_ = -1.;
      TString photonsName_ = "Photons";
      TString looseIdName_ = "PhotonsLooseId";
      TString mediumIdName_ = "PhotonsMediumId";
      TString tightIdName_ = "PhotonsTightId";
      TString highPtIdName_ = "PhotonsHighPtId";
      TString pfCandsName_ = "PFCandidates";
      TString verticesName_ = "PrimaryVertexes";
      UInt_t isoType_ = mithep::PhotonTools::kIsoUndef;

      Float_t const* rho_ = 0;

      ClassDef(PhotonsFiller, 0)
    };

  }
}

#endif
