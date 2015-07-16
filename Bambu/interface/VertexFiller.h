#ifndef NeroProducer_Bambu_VertexFiller_h
#define NeroProducer_Bambu_VertexFiller_h

#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareVertex.hpp"

#include "MitAna/DataTree/interface/Vertex.h"

namespace mithep {
  namespace nero {

    class VertexFiller : public BaseFiller {
    public:
      VertexFiller() {}
      ~VertexFiller() {}

      BareCollection* getObject() override { return &out_; }
      mithep::nero::Collection collection() const override { return kVertex; }

      void fill() override;

      mithep::Vertex const* pv() const { return pv_; }

      void SetVerticesName(char const* _name) { verticesName_ = _name; }
      
    private:
      BareVertex out_{};

      TString verticesName_ = "PrimaryVertexes";

      mithep::Vertex const* pv_ = 0;

      ClassDef(VertexFiller, 0)
    };

  }
}

#endif
