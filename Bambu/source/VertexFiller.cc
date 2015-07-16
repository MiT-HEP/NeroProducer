#include "NeroProducer/Bambu/interface/VertexFiller.h"

#include "MitAna/DataTree/interface/VertexCol.h"

#include <map>

ClassImp(mithep::nero::VertexFiller)

void
mithep::nero::VertexFiller::fill()
{
  auto* vertices = getSource<mithep::VertexCol>(verticesName_);
  pv_ = vertices->At(0);
}
