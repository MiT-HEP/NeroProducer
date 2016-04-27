#include "NeroProducer/Bambu/interface/LeptonsFiller.h"

#include "MitAna/DataTree/interface/ElectronCol.h"
#include "MitAna/DataTree/interface/MuonCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataTree/interface/PileupEnergyDensity.h"
#include "MitAna/DataCont/interface/Types.h"

#include "MitPhysics/Utils/interface/IsolationTools.h"
#include "MitPhysics/Utils/interface/ElectronTools.h"
#include "MitPhysics/Mods/interface/IdMod.h"

#include "TTree.h"

#include <map>
#include <cstring>

ClassImp(mithep::nero::LeptonsFiller)

void
mithep::nero::LeptonsFiller::setCrossRef(BaseFiller* _fillers[])
{
  // need dynamic cast here because BareP4 inherits virtually from BareCollection
  event_ = dynamic_cast<BareEvent*>(_fillers[kEvent]->getObject());
}

void
mithep::nero::LeptonsFiller::setDefinedId_(Lepton _lep, BareLeptons::Selection _selection, char const* _name, Bool_t _saveAllPass)
{
  unsigned idx(0);
  while ((1 << idx) != _selection)
    ++idx;
  idName_[_lep][idx] = _name;

  savePassing_[_lep][idx] = _saveAllPass;
}

void
mithep::nero::LeptonsFiller::initialize()
{
  outputFile_->cd();
  auto* selBitsTree(new TTree("leptonSelBits", "Lepton selBits"));
  char eleBitsName[128];
  char muBitsName[128];
  selBitsTree->Branch("electron", eleBitsName, "electron/C");
  selBitsTree->Branch("muon", muBitsName, "muon/C");
  for (unsigned iB(0); iB != 32; ++iB) {
    std::strcpy(eleBitsName, idName_[kEl][iB].Data());
    std::strcpy(muBitsName, idName_[kMu][iB].Data());
    selBitsTree->Fill();
  }
  selBitsTree->Write();
  delete selBitsTree;

  if (electronMVAConfig_.second != ElectronIDMVA::kUninitialized) {
    electronDiscriminator_ = new ElectronIDMVA();
    electronDiscriminator_->Initialize(electronMVAConfig_.first, ElectronIDMVA::MVAType(electronMVAConfig_.second), true, electronMVAWeights_);
  }
}

void
  mithep::nero::LeptonsFiller::finalize()
{
  delete electronDiscriminator_;
  electronDiscriminator_ = 0;
}

void
mithep::nero::LeptonsFiller::fill()
{
  auto* electrons = getSource<mithep::ElectronCol>(electronsName_);
  auto* muons = getSource<mithep::MuonCol>(muonsName_);

  if (!electrons || !muons)
    return;

  mithep::NFArrBool const* ids[nLeptons][32]{};
  for (unsigned iL(0); iL != nLeptons; ++iL) {
    for (unsigned iSel(0); iSel != 32; ++iSel) {
      if (idName_[iL][iSel].Length() != 0)
        ids[iL][iSel] = getSource<mithep::NFArrBool>(idName_[iL][iSel]);
    }
  }

  auto* pfCands = getSource<mithep::PFCandidateCol>(pfCandsName_);
  auto* nopuPFCands = getSource<mithep::PFCandidateCol>(nopuPFCandsName_);
  auto* puPFCands = getSource<mithep::PFCandidateCol>(puPFCandsName_);
  auto* vertices = getSource<mithep::VertexCol>(verticesName_);

  unsigned iE(0);
  unsigned iM(0);

  while (true) {
    mithep::Electron const* ele = 0;
    if (iE != electrons->GetEntries())
      ele = electrons->At(iE);

    mithep::Muon const* mu = 0;
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
      // at least one standard lepton Id should be true
      unsigned iSel(0);
      for (; iSel != 32; ++iSel) {
        if (ids[kEl][iSel] && savePassing_[kEl][iSel] && ids[kEl][iSel]->At(iE))
          break;
      }

      if (iSel != 32) {
        newP4(out_, *ele);

        double mva(-1.);
        if (electronDiscriminator_)
          mva = electronDiscriminator_->MVAValue(ele, vertices->At(0), kFALSE);

        double chIso(ele->PFChargedHadronIso());
        double nhIso(ele->PFNeutralHadronIso());
        double phoIso(ele->PFPhotonIso());
        // dZ cut at 10000. -> vertex information is not used
        double puIso(IsolationTools::PFElectronIsolation(ele, puPFCands, vertices->At(0), 10000., 0., 0.3, 0., mithep::PFCandidate::eHadron));

        // iso type here is used only to determine the effective area and can be any 2015 flag
        double combIso(ElectronTools::CombinedIsoRhoCorr(ElectronTools::kSummer15LooseIso, chIso, nhIso + phoIso, event_->rho, ele->SCluster()->Eta()));

        out_.pdgId->push_back(-11 * ele->Charge());

        unsigned selBits(0);
        for (iSel = 0; iSel != 32; ++iSel) {
          if (ids[kEl][iSel] && ids[kEl][iSel]->At(iE))
            selBits |= (1 << iSel);
        }
        out_.selBits->push_back(selBits);

        out_.lepPfPt->push_back(0.);
        out_.mva->push_back(mva);
        out_.chIso->push_back(chIso);
        out_.nhIso->push_back(nhIso);
        out_.phoIso->push_back(phoIso);
        out_.puIso->push_back(puIso);
        out_.iso->push_back(combIso);
      }
      ++iE;
    }
    else {
      // at least one lepton Id should be true
      unsigned iSel(0);
      for (; iSel != 32; ++iSel) {
        if (ids[kMu][iSel] && savePassing_[kMu][iSel] && ids[kMu][iSel]->At(iM))
          break;
      }

      if (iSel != 32) {
        newP4(out_, *mu);

        double isoArr[4];
        double iso(IsolationTools::BetaMwithPUCorrection(nopuPFCands, puPFCands, mu, 0.4, isoArr));
      
        mithep::PFCandidate const* pf = 0;
        for (unsigned iPF(0); iPF != pfCands->GetEntries(); ++iPF) {
          if (pfCands->At(iPF)->Mu() == mu)
            pf = pfCands->At(iPF);
        }

        out_.pdgId->push_back(-13 * mu->Charge());
        out_.iso->push_back(iso);

        unsigned selBits(0);
        for (iSel = 0; iSel != 32; ++iSel) {
          if (ids[kMu][iSel] && ids[kMu][iSel]->At(iM))
            selBits |= (1 << iSel);
        }
        out_.selBits->push_back(selBits);

        if (pf)
          out_.lepPfPt->push_back(pf->Pt());
        else
          out_.lepPfPt->push_back(0.);

        // TODO
        out_.mva->push_back(0);
        out_.chIso->push_back(isoArr[0]);
        out_.nhIso->push_back(isoArr[1]);
        out_.phoIso->push_back(isoArr[2]);
        out_.puIso->push_back(isoArr[3]);
      }
      ++iM;
    }
  }
}
