#include "NeroProducer/Bambu/interface/PhotonsFiller.h"
#include "NeroProducer/Core/interface/BareEvent.hpp"

#include "MitAna/DataTree/interface/PhotonCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/ElectronCol.h"
#include "MitAna/DataTree/interface/DecayParticleCol.h"
#include "MitAna/DataTree/interface/BeamSpotCol.h"
#include "MitAna/DataTree/interface/MCParticleCol.h"
#include "MitAna/DataTree/interface/MCEventInfo.h"
#include "MitAna/DataTree/interface/EventHeader.h"
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

  mithep::MCEventInfo const* mcEvent = 0;
  TVector3 genVertex;
  std::vector<mithep::MCParticle const*> mcFinalStates;
  std::vector<mithep::MCParticle const*> promptPhotons;
  std::vector<unsigned> photonAncestors;
  auto* eventHeader = getSource<mithep::EventHeader>(mithep::Names::gkEvtHeaderBrn);
  if (eventHeader->IsMC()) {
    auto* mcParts = getSource<mithep::MCParticleCol>(mithep::Names::gkMCPartBrn);
    mcEvent = getSource<mithep::MCEventInfo>(mithep::Names::gkMCEvtInfoBrn);

    if (mcParts->GetEntries() != 0) {
      auto&& decayVtx(mcParts->At(0)->DecayVertex());
      genVertex.SetXYZ(decayVtx.X(), decayVtx.Y(), decayVtx.Z());

      for (unsigned iMC(0); iMC != mcParts->GetEntries(); ++iMC) {
        auto& part(*mcParts->At(iMC));
        if (part.Status() != 1)
          continue;

        mcFinalStates.push_back(&part);

        if (part.PdgId() != 22)
          continue;

        auto const* mother(&part);

        while (mother) {
          unsigned mId(mother->AbsPdgId());
          if ((mother->Status() == 2 || (mother->Status() > 70 && mother->Status() < 100)) && ((mId / 100) % 10 != 0 || mId < 7 || mId == 21 || mId == 13 || mId == 15)) {
            // mother is a decaying lepton or hadron
            auto* copy = mother;
            while (true) {
              auto* daughter = copy->FindDaughter(copy->PdgId(), true);
              if (daughter)
                copy = daughter;
              else
                break;
            }
            if (copy == mother) {
              // mother is the last copy
              break;
            }
          }

          mother = mother->Mother();
        }

        if (mother) // is a decay product
          continue;
        
        promptPhotons.push_back(&part);
        photonAncestors.push_back(-1);

        for (unsigned iP(0); iP != mcEvent->NPartons(); ++iP) {
          if (mcEvent->PartonStatus(iP) != 1)
            continue;

          if (mithep::MathUtils::DeltaR(part, *mcEvent->PartonMom(iP)) < 0.0225) {
            photonAncestors.back() = iP;
            break;
          }
        }
      }
    }
    else
      mcEvent = 0;
  }

  for (unsigned iP(0); iP != photons->GetEntries(); ++iP) {
    if (!looseId->At(iP) && !highPtId->At(iP))
      continue;

    auto& photon(*photons->At(iP));
    double scEta(photon.SCluster()->AbsEta());

    double chIso(0.);
    double nhIso(0.);
    double phIso(0.);

    IsolationTools::PFEGIsoFootprintRemoved(&photon, pv, pfCands, 0.3, chIso, nhIso, phIso);
    PhotonTools::IsoLeakageCorrection(&photon, PhotonTools::EPhIsoType(isoType_), chIso, nhIso, phIso);
    
    double chIsoMax(chIso);

    PhotonTools::IsoRhoCorrection(&photon, PhotonTools::EPhIsoType(isoType_), *rho_, chIso, nhIso, phIso);

    double iso(chIso + nhIso + phIso);
    if (maxIso_ > 0. && iso > maxIso_)
      continue;

    newP4(out_, photon);

    out_.sieie->push_back(photon.CoviEtaiEta5x5());

    out_.iso->push_back(iso);

    bool csafeveto(PhotonTools::PassElectronVetoConvRecovery(&photon, electrons, conversions, beamspot));

    unsigned selBits(BarePhotons::PhoBaseline);
    if (csafeveto)
      selBits |= BarePhotons::PhoElectronVeto;

    if (!photon.HasPixelSeed())
      selBits |= BarePhotons::PhoPixelSeedVeto;

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

    if (out_.IsExtend()) {
      out_.rawpt->push_back(photon.SCluster()->RawEnergy() / std::cosh(scEta));
      out_.e55->push_back(photon.E55());

      out_.hOverE->push_back(photon.HadOverEmTow());
      out_.chWorstIso->push_back(IsolationTools::PFChargedIsolation(&photon, 0, 0.3, 0., pfCands, 0, vertices));

      // compare to the chIso without rho correction using other vertices
      for (unsigned iV(1); iV < vertices->GetEntries(); ++iV) {
        IsolationTools::PFEGIsoFootprintRemoved(&photon, vertices->At(iV), pfCands, 0.3, chIso, nhIso, phIso);
        PhotonTools::IsoLeakageCorrection(&photon, PhotonTools::EPhIsoType(isoType_), chIso, nhIso, phIso);
        if (chIso > chIsoMax)
          chIsoMax = chIso;
      }
      out_.chIsoMax->push_back(chIsoMax);

      out_.sipip->push_back(photon.CoviPhiiPhi());
      out_.sieip->push_back(photon.CoviEtaiPhi());
      out_.r9->push_back(photon.R9());
      out_.s4->push_back(photon.S4Ratio());

      out_.mipEnergy->push_back(photon.MipTotEnergy());

      out_.time->push_back(photon.SCluster()->SeedTime());
      out_.timeSpan->push_back(photon.SCluster()->LeadTimeSpan());
      
      if (mcEvent) {
        // find matched gen
        // 22 -> gen photon with no matching parton
        // -22 -> gen photon with matching isolated parton
        // 11/-11 -> electrons
        // 111 -> everything else

        short genMatch = 0;

        auto&& caloPos(photon.CaloPos());
        TVector3 caloDir(caloPos.X(), caloPos.Y(), caloPos.Z());          
        caloDir -= genVertex;

        mithep::MCParticle const* matched(0);

        for (unsigned iPP(0); iPP != promptPhotons.size(); ++iPP) {
          if (photonAncestors[iPP] >= mcEvent->NPartons())
            continue;

          auto&& partP4(*mcEvent->PartonMom(photonAncestors[iPP]));
          if (caloDir.DeltaR(TVector3(partP4.X(), partP4.Y(), partP4.Z())) < 0.1) {
            matched = promptPhotons[iPP];
            genMatch = -22;
            break;
          }
        }

        if (!matched) {
          double minDR(-1.);
          bool leptonMatched(false);

          for (auto* part : mcFinalStates) {
            double dR(caloDir.DeltaR(TVector3(part->Px(), part->Py(), part->Pz())));
            if (dR > 0.1)
              continue;

            bool isLepton(part->AbsPdgId() == 11 || part->AbsPdgId() == 13);

            if (minDR < 0. || isLepton || (dR < minDR && !leptonMatched)) {
              genMatch = part->PdgId();

              minDR = dR;
              if (isLepton)
                leptonMatched = true;

              matched = part;
            }
          }

          if (genMatch == 22 && matched) {
            // Matched to a final state photon. Is this from a hadron decay?
            auto* mother(matched->DistinctMother());
            if (!(mother->PdgId() == 2212 && !mother->HasMother()) && std::abs(mother->PdgId()) > 25)
              genMatch = 111;
          }
        }

        out_.genMatched->push_back(genMatch);
      }
    }
  }
}
