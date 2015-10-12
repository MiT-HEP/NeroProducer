#include "NeroProducer/Bambu/interface/LeptonsFiller.h"

#include "MitAna/DataTree/interface/ElectronCol.h"
#include "MitAna/DataTree/interface/MuonCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/PileupEnergyDensity.h"
#include "MitAna/DataCont/interface/Types.h"

#include "MitPhysics/Utils/interface/IsolationTools.h"
#include "MitPhysics/Utils/interface/ElectronTools.h"
#include "MitPhysics/Mods/interface/IdMod.h"

#include <map>

ClassImp(mithep::nero::LeptonsFiller)

void
mithep::nero::LeptonsFiller::setCrossRef(BaseFiller* _fillers[])
{
  // need dynamic cast here because BareP4 inherits virtually from BareCollection
  event_ = dynamic_cast<BareEvent*>(_fillers[kEvent]->getObject());
}

void
mithep::nero::LeptonsFiller::setDefinedId_(TString _idNames[], BareLeptons::Selection _selection, char const* _name)
{
  unsigned idx(0);
  while ((1 << idx) != _selection)
    ++idx;
  _idNames[idx] = _name;
}

void
mithep::nero::LeptonsFiller::fill()
{
  auto* electrons = getSource<mithep::ElectronCol>(electronsName_);
  auto* muons = getSource<mithep::MuonCol>(muonsName_);

  if (!electrons || !muons)
    return;

  mithep::NFArrBool* muIds[32]{};
  for (unsigned iSel(0); iSel != 32; ++iSel) {
    if (muonIdName_[iSel].Length() != 0)
      muIds[iSel] = getSource<mithep::NFArrBool>(muonIdName_[iSel]);
  }

  mithep::NFArrBool* eleIds[32]{};
  for (unsigned iSel(0); iSel != 32; ++iSel) {
    if (electronIdName_[iSel].Length() != 0)
      eleIds[iSel] = getSource<mithep::NFArrBool>(electronIdName_[iSel]);
  }

  auto* pfCands = getSource<mithep::PFCandidateCol>(pfCandsName_);
  auto* nopuPFCands = getSource<mithep::PFCandidateCol>(nopuPFCandsName_);
  auto* puPFCands = getSource<mithep::PFCandidateCol>(puPFCandsName_);
  auto* vertices = getSource<mithep::VertexCol>(verticesName_);

  unsigned iE(0);
  unsigned iM(0);

  while (true) {
    mithep::Electron* ele = 0;
    if (iE != electrons->GetEntries())
      ele = electrons->At(iE);

    mithep::Muon* mu = 0;
    if (iM != muons->GetEntries())
      mu = muons->At(iM);

    if (!ele && !mu)
      break;

    if (ele && mu) {
      if (ele->Pt() > mu->Pt())
        mu = 0;
      else
        ele = 0;
    }

    if (ele) {
      // at least one lepton Id should be true
      unsigned iSel(0);
      for (; iSel != 32; ++iSel) {
        if (eleIds[iSel] && eleIds[iSel]->At(iE))
          break;
      }

      if (iSel != 32) {
        newP4(out_, *ele);

        double chIso(ele->PFChargedHadronIso());
        double nhIso(ele->PFNeutralHadronIso());
        double phoIso(ele->PFPhotonIso());
        // dZ cut at 10000. -> vertex information is not used
        double puIso(IsolationTools::PFElectronIsolation(ele, puPFCands, vertices->At(0), 10000., 0., 0.3, 0., mithep::PFCandidate::eHadron));

        out_.pdgId->push_back(-11 * ele->Charge());
        out_.iso->push_back(IsolationTools::PFEleCombinedIsolationRhoCorr(ele, event_->rho, ElectronTools::kEleEASummer15));

        unsigned selBits(0);
        for (iSel = 0; iSel != 32; ++iSel) {
          if (eleIds[iSel] && eleIds[iSel]->At(iE))
            selBits |= (1 << iSel);
        }
        out_.selBits->push_back(selBits);

        out_.lepPfPt->push_back(0.);
        out_.chIso->push_back(chIso);
        out_.nhIso->push_back(nhIso);
        out_.phoIso->push_back(phoIso);
        out_.puIso->push_back(puIso);
      }
      ++iE;
    }
    else {
      // at least one lepton Id should be true
      unsigned iSel(0);
      for (; iSel != 32; ++iSel) {
        if (muIds[iSel] && muIds[iSel]->At(iM))
          break;
      }

      if (iSel != 32) {
        newP4(out_, *mu);

        double isoArr[4];
        double iso(IsolationTools::BetaMwithPUCorrection(nopuPFCands, puPFCands, mu, 0.4, isoArr));
      
        mithep::PFCandidate* pf = 0;
        for (unsigned iPF(0); iPF != pfCands->GetEntries(); ++iPF) {
          if (pfCands->At(iPF)->Mu() == mu)
            pf = pfCands->At(iPF);
        }

        out_.pdgId->push_back(-13 * mu->Charge());
        out_.iso->push_back(iso);

        unsigned selBits(0);
        for (iSel = 0; iSel != 32; ++iSel) {
          if (muIds[iSel] && muIds[iSel]->At(iM))
            selBits |= (1 << iSel);
        }
        out_.selBits->push_back(selBits);

        if (pf)
          out_.lepPfPt->push_back(pf->Pt());
        else
          out_.lepPfPt->push_back(0.);

        // TODO
        out_.chIso->push_back(isoArr[0]);
        out_.nhIso->push_back(isoArr[1]);
        out_.phoIso->push_back(isoArr[2]);
        out_.puIso->push_back(isoArr[3]);
      }
      ++iM;
    }
  }
}
