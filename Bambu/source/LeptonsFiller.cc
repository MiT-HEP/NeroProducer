#include "NeroProducer/Bambu/interface/LeptonsFiller.h"

#include "MitAna/DataTree/interface/ElectronCol.h"
#include "MitAna/DataTree/interface/MuonCol.h"
#include "MitAna/DataTree/interface/PFCandidateCol.h"
#include "MitAna/DataCont/interface/Types.h"

#include "MitPhysics/Utils/interface/IsolationTools.h"

#include <map>

ClassImp(mithep::nero::LeptonsFiller)

void
mithep::nero::LeptonsFiller::fill()
{
  auto* electrons = getSource<mithep::ElectronCol>(electronsName_);
  auto* muons = getSource<mithep::MuonCol>(muonsName_);

  if (!electrons || !muons)
    return;

  auto* vetoMuId = getSource<mithep::NFArrBool>(vetoMuonIdName_);
  auto* fakeMuId = getSource<mithep::NFArrBool>(fakeMuonIdName_);
  auto* softMuId = getSource<mithep::NFArrBool>(softMuonIdName_);
  auto* tightMuId = getSource<mithep::NFArrBool>(tightMuonIdName_);
  mithep::NFArrBool* customMuId[24]{};
  for (unsigned iB(0); iB != 24; ++iB) {
    if (customMuonIdName_[iB].Length() != 0)
      customMuId[iB] = getSource<mithep::NFArrBool>(customMuonIdName_[iB]);
  }

  auto* vetoEleId = getSource<mithep::NFArrBool>(vetoElectronIdName_);
  auto* fakeEleId = getSource<mithep::NFArrBool>(fakeElectronIdName_);
  auto* mediumEleId = getSource<mithep::NFArrBool>(mediumElectronIdName_);
  auto* tightEleId = getSource<mithep::NFArrBool>(tightElectronIdName_);
  mithep::NFArrBool* customEleId[24]{};
  for (unsigned iB(0); iB != 24; ++iB) {
    if (customElectronIdName_[iB].Length() != 0)
      customEleId[iB] = getSource<mithep::NFArrBool>(customElectronIdName_[iB]);
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
      if (vetoEleId->At(iE) || fakeEleId->At(iE) || mediumEleId->At(iE) || tightEleId->At(iE)){
        newP4(out_, *ele);

        double chIso(ele->PFChargedHadronIso());
        double nhIso(ele->PFNeutralHadronIso());
        double phoIso(ele->PFPhotonIso());
        // dZ cut at 10000. -> vertex information is not used
        double puIso(IsolationTools::PFElectronIsolation(ele, puPFCands, vertices->At(0), 10000., 0., 0.3, 0., mithep::PFCandidate::eHadron));

        out_.pdgId->push_back(-11 * ele->Charge());
        out_.iso->push_back(chIso + nhIso + phoIso);

        unsigned selBits(BareLeptons::LepLoose);
        if (vetoEleId->At(iE))
          selBits |= BareLeptons::LepVeto;
        if (fakeEleId->At(iE))
          selBits |= BareLeptons::LepFake;
        if (mediumEleId->At(iE))
          selBits |= BareLeptons::LepMedium;
        if (tightEleId->At(iE))
          selBits |= BareLeptons::LepTight;
        for (unsigned iB(8); iB != 32; ++iB) {
          if (customEleId[iB - 8] && customEleId[iB - 8]->At(iE))
            selBits |= (1 << iB);
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
      if (vetoMuId->At(iM) || fakeMuId->At(iM) || softMuId->At(iM) || tightMuId->At(iM)){
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

        // careful, different treatment for muons and electrons
        unsigned selBits(BareLeptons::LepLoose);
        if (vetoMuId->At(iM))
          selBits |= BareLeptons::LepVeto;
        if (fakeMuId->At(iM))
          selBits |= BareLeptons::LepFake;
        if (softMuId->At(iM))
          selBits |= BareLeptons::LepSoft;
        if (tightMuId->At(iM))
          selBits |= BareLeptons::LepTight;
        for (unsigned iB(8); iB != 32; ++iB) {
          if (customMuId[iB - 8] && customMuId[iB - 8]->At(iM))
            selBits |= (1 << iB);
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
