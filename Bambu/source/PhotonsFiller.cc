#include "NeroProducer/Bambu/interface/PhotonsFiller.h"
#include "NeroProducer/Core/interface/BareEvent.hpp"

#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/ElectronCol.h"
#include "MitAna/DataTree/interface/DecayParticleCol.h"
#include "MitAna/DataTree/interface/BeamSpotCol.h"
#include "MitAna/DataTree/interface/Names.h"
#include "MitAna/DataCont/interface/Types.h"

#include "MitPhysics/Utils/interface/PhotonTools.h"
#include "MitPhysics/Utils/interface/IsolationTools.h"

#include <map>
#include <cmath>

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
  // loose, medium, tight has no electron veto applied yet
  auto* looseId = getSource<mithep::NFArrBool>(looseIdName_);
  auto* mediumId = getSource<mithep::NFArrBool>(mediumIdName_);
  auto* tightId = getSource<mithep::NFArrBool>(tightIdName_);
  auto* highPtId = getSource<mithep::NFArrBool>(highPtIdName_);

  auto* pfCands = getSource<mithep::PFCandidateCol>(pfCandsName_);
  auto* vertices = getSource<mithep::VertexCol>(verticesName_);
  auto* beamspot = getSource<mithep::BeamSpotCol>(Names::gkBeamSpotBrn)->At(0);
  auto* electrons = getSource<mithep::ElectronCol>(Names::gkElectronBrn);
  auto* conversions = getSource<mithep::DecayParticleCol>("Conversions");
  auto* pv = vertices->At(0);

  for (unsigned iP(0); iP != photons->GetEntries(); ++iP) {
    if (!looseId->At(iP) && !highPtId->At(iP))
      continue;

    auto& photon(*photons->At(iP));
    double scEta(photon.SCluster()->AbsEta());

    double chIso(0.);
    double nhIso(0.);
    double phIso(0.);

    IsolationTools::PFEGIsoFootprintRemoved(&photon, pv, pfCands, 0.3, chIso, nhIso, phIso);
    PhotonTools::IsoRhoCorrection(&photon, PhotonTools::EPhIsoType(isoType_), *rho_, chIso, nhIso, phIso);

    double iso(chIso + nhIso + phIso);
    if (maxIso_ > 0. && iso > maxIso_)
      continue;

    newP4(out_, photon);

    out_.iso->push_back(iso);
    out_.sieie->push_back(photon.CoviEtaiEta5x5());
    out_.rawpt->push_back(photon.SCluster()->RawEnergy() / std::cosh(scEta));

    out_.r9->push_back(photon.R9());

    bool csafeveto(PhotonTools::PassElectronVetoConvRecovery(&photon, electrons, conversions, beamspot));

    unsigned selBits(BarePhotons::PhoBaseline);
    if (csafeveto)
      selBits |= BarePhotons::PhoElectronVeto;

    if (looseId->At(iP)) {
      selBits |= BarePhotons::PhoLooseNoEVeto;
      if (csafeveto)
        selBits |= BarePhotons::PhoLoose;
    }
    if (mediumId->At(iP)) {
      selBits |= BarePhotons::PhoMediumNoEVeto;
      if (csafeveto)
        selBits |= BarePhotons::PhoMedium;
    }
    if (tightId->At(iP)) {
      selBits |= BarePhotons::PhoTightNoEVeto;
      if (csafeveto)
        selBits |= BarePhotons::PhoTight;
    }
    if (highPtId->At(iP))
      selBits |= BarePhotons::PhoHighPt;
    out_.selBits->push_back(selBits);

    out_.chIso->push_back(chIso);
    out_.nhIso->push_back(nhIso);
    out_.phoIso->push_back(phIso);
  }
}
