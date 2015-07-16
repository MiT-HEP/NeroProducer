#include "NeroProducer/Bambu/interface/PhotonsFiller.h"

#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataCont/interface/Types.h"

#include "MitPhysics/Utils/interface/IsolationTools.h"

#include <map>

ClassImp(mithep::nero::PhotonsFiller)

void
mithep::nero::PhotonsFiller::fill()
{
  auto* photons = getSource<mithep::PhotonCol>(photonsName_);
  auto* mediumId = getSource<mithep::NFArrBool>(mediumIdName_);
  auto* tightId = getSource<mithep::NFArrBool>(tightIdName_);

  auto* pfCands = getSource<mithep::PFCandidateCol>(pfCandsName_);
  auto* vertices = getSource<mithep::VertexCol>(verticesName_);
  auto* pv = vertices->At(0);

  for (unsigned iP(0); iP != photons->GetEntries(); ++iP) {
    auto& photon(*photons->At(iP));

    double chIso(0.);
    double nhIso(0.);
    double phIso(0.);

    IsolationTools::PFPhotonIsoFootprintRemoved(&photon, pv, pfCands, 0.3, chIso, nhIso, phIso);

    double iso(chIso + nhIso + phIso);
    if (iso > maxIso_)
      continue;

    newP4(out_, photon);

    out_.iso->push_back(iso);
    out_.sieie->push_back(photon.CoviEtaiEta5x5());
    out_.tightid->push_back(tightId->At(iP));
    out_.mediumid->push_back(mediumId->At(iP));
  }
}
