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

  auto* eleTightId = getSource<mithep::NFArrBool>(electronIdsName_);
  auto* muTightId = getSource<mithep::NFArrBool>(muonIdsName_);

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
      newP4(out_, ele->Px(), ele->Py(), ele->Pz(), ele->E());

      double chIso(ele->PFChargedHadronIso());
      double nhIso(ele->PFNeutralHadronIso());
      double phoIso(ele->PFPhotonIso());
      // dZ cut at 10000. -> vertex information is not used
      double puIso(IsolationTools::PFElectronIsolation(ele, puPFCands, vertices->At(0), 10000., 0., 0.3, 0., mithep::PFCandidate::eHadron));

      out_.pdgId->push_back(11 * ele->Charge());
      out_.iso->push_back(chIso + nhIso + phoIso);
      out_.tightId->push_back(eleTightId->At(iE));
      out_.lepPfPt->push_back(0.);
      out_.chIso->push_back(chIso);
      out_.nhIso->push_back(nhIso);
      out_.phoIso->push_back(phoIso);
      out_.puIso->push_back(puIso);

      ++iE;
    }
    else {
      newP4(out_, mu->Px(), mu->Py(), mu->Pz(), mu->E());

      double iso(IsolationTools::BetaMwithPUCorrection(nopuPFCands, puPFCands, mu, 0.4));
      
      mithep::PFCandidate* pf = 0;
      for (unsigned iPF(0); iPF != pfCands->GetEntries(); ++iPF) {
        if (pfCands->At(iPF)->Mu() == mu)
          pf = pfCands->At(iPF);
      }

      out_.pdgId->push_back(13 * mu->Charge());
      out_.iso->push_back(iso);
      out_.tightId->push_back(muTightId->At(iM));
      if (pf)
        out_.lepPfPt->push_back(pf->Pt());
      else
        out_.lepPfPt->push_back(0.);

      // TODO
      out_.chIso->push_back(0.);
      out_.nhIso->push_back(0.);
      out_.phoIso->push_back(0.);
      out_.puIso->push_back(0.);

      ++iM;
    }
  }
}
