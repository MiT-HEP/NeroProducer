#ifndef NeroProducer_Bambu_PhotonsFiller_h
#define NeroProducer_Bambu_PhotonsFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BarePhotons.hpp"

namespace mithep {
  namespace nero {

    class PhotonsFiller : public BaseFiller {
    public:
      PhotonsFiller() {}
      ~PhotonsFiller() {}

      BareCollection* getObject() override { return &out_; }
      BaseFiller::Collection collection() const override { return BaseFiller::kPhotons; }

      void fill() override;

      void SetMaxIso(Double_t _max) { maxIso_ = _max; }
      void SetPhotonsName(char const* _name) { photonsName_ = _name; }
      void SetMediumIdName(char const* _name) { mediumIdName_ = _name; }
      void SetTightIdName(char const* _name) { tightIdName_ = _name; }
      void SetPFCandsName(char const* _name) { pfCandsName_ = _name; }
      void SetVerticesName(char const* _name) { verticesName_ = _name; }

    private:
      BarePhotons out_{};

      Double_t maxIso_ = -1.;
      TString photonsName_ = "Photons";
      TString mediumIdName_ = "PhotonsMediumId";
      TString tightIdName_ = "PhotonsTightId";
      TString pfCandsName_ = "PFCandidates";
      TString verticesName_ = "PrimaryVertexes";

      ClassDef(PhotonsFiller, 0)
    };

  }
}

#endif
