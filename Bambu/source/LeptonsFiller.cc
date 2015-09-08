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

  auto* muAId = getSource<mithep::NFArrBool>(muonIdsAName_);
  auto* muBId = getSource<mithep::NFArrBool>(muonIdsBName_);
  auto* muCId = getSource<mithep::NFArrBool>(muonIdsCName_);
  auto* muDId = getSource<mithep::NFArrBool>(muonIdsDName_);

  auto* eleAId = getSource<mithep::NFArrBool>(electronIdsAName_);
  auto* eleBId = getSource<mithep::NFArrBool>(electronIdsBName_);
  auto* eleCId = getSource<mithep::NFArrBool>(electronIdsCName_);
  auto* eleDId = getSource<mithep::NFArrBool>(electronIdsDName_);

  auto* pfCands = getSource<mithep::PFCandidateCol>(pfCandsName_);
  auto* nopuPFCands = getSource<mithep::PFCandidateCol>(nopuPFCandsName_);
  auto* puPFCands = getSource<mithep::PFCandidateCol>(puPFCandsName_);
  auto* vertices = getSource<mithep::VertexCol>(verticesName_);

  if (!electrons || !muons)
    return;

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
      if (eleAId->At(iE) || eleBId->At(iE) || eleCId->At(iE) || eleDId->At(iE)){
        newP4(out_, *ele);

        double chIso(ele->PFChargedHadronIso());
        double nhIso(ele->PFNeutralHadronIso());
        double phoIso(ele->PFPhotonIso());
        // dZ cut at 10000. -> vertex information is not used
        double puIso(IsolationTools::PFElectronIsolation(ele, puPFCands, vertices->At(0), 10000., 0., 0.3, 0., mithep::PFCandidate::eHadron));

        out_.pdgId->push_back(-11 * ele->Charge());
        out_.iso->push_back(chIso + nhIso + phoIso);

        unsigned selBits = BareLeptons::LepLoose;
        if (eleAId->At(iE))
          selBits |= BareLeptons::LepVeto;
        if (eleBId->At(iE))
          selBits |= BareLeptons::LepFake;
        if (eleCId->At(iE))
          selBits |= BareLeptons::LepMedium;
        if (eleDId->At(iE))
          selBits |= BareLeptons::LepTight;
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
      if (muAId->At(iM) || muBId->At(iM) || muCId->At(iM) || muDId->At(iM)){
        newP4(out_, *mu);

        double iso(IsolationTools::BetaMwithPUCorrection(nopuPFCands, puPFCands, mu, 0.4));
      
        mithep::PFCandidate* pf = 0;
        for (unsigned iPF(0); iPF != pfCands->GetEntries(); ++iPF) {
          if (pfCands->At(iPF)->Mu() == mu)
            pf = pfCands->At(iPF);
        }

        out_.pdgId->push_back(-13 * mu->Charge());
        out_.iso->push_back(iso);

        // careful, different treatment for muons and electrons
        unsigned selBits = BareLeptons::LepLoose;
        if (muAId->At(iM))
          selBits |= BareLeptons::LepVeto;
        if (muBId->At(iM))
          selBits |= BareLeptons::LepFake;
        if (muCId->At(iM))
          selBits |= BareLeptons::LepSoft;
        if (muDId->At(iM))
          selBits |= BareLeptons::LepTight;
        out_.selBits->push_back(selBits);

        if (pf)
          out_.lepPfPt->push_back(pf->Pt());
        else
          out_.lepPfPt->push_back(0.);

        // TODO
        out_.chIso->push_back(0.);
        out_.nhIso->push_back(0.);
        out_.phoIso->push_back(0.);
        out_.puIso->push_back(0.);
      }
      ++iM;
    }
  }
}
