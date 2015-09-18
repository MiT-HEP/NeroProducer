#include "NeroProducer/Bambu/interface/PhotonsFiller.h"
#include "NeroProducer/Core/interface/BareEvent.hpp"

#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataCont/interface/Types.h"

#include "MitPhysics/Utils/interface/PhotonTools.h"
#include "MitPhysics/Utils/interface/IsolationTools.h"

#include <map>

ClassImp(mithep::nero::PhotonsFiller)

void
mithep::nero::PhotonsFiller::setCrossRef(BaseFiller* _fillers[])
{
  auto* event = dynamic_cast<BareEvent const*>(_fillers[kEvent]->getObject());
  rho_ = &event->rho;
}

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
    double scEta(photon.SCluster()->AbsEta());

    double chIso(0.);
    double nhIso(0.);
    double phIso(0.);

    IsolationTools::PFEGIsoFootprintRemoved(&photon, pv, pfCands, 0.3, chIso, nhIso, phIso);

    chIso = IsolationTools::PFPhotonIsolationRhoCorr(scEta, chIso, *rho_, PhotonTools::kPhoEAPhys14, PhotonTools::kPhoChargedHadron03);
    nhIso = IsolationTools::PFPhotonIsolationRhoCorr(scEta, nhIso, *rho_, PhotonTools::kPhoEAPhys14, PhotonTools::kPhoNeutralHadron03);
    phIso = IsolationTools::PFPhotonIsolationRhoCorr(scEta, phIso, *rho_, PhotonTools::kPhoEAPhys14, PhotonTools::kPhoPhoton03);

    double iso(chIso + nhIso + phIso);
    if (maxIso_ > 0. && iso > maxIso_)
      continue;

    newP4(out_, photon);

    out_.iso->push_back(iso);
    out_.sieie->push_back(photon.CoviEtaiEta5x5());
    out_.looseid->push_back(1);
    out_.tightid->push_back(tightId->At(iP));
    out_.mediumid->push_back(mediumId->At(iP));

    out_.chIso->push_back(chIso);
    out_.nhIso->push_back(nhIso);
    out_.phoIso->push_back(phIso);
  }
}
