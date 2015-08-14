#include "NeroProducer/Bambu/interface/TriggerFiller.h"
#include "NeroProducer/Core/interface/BareLeptons.hpp"

#include "MitAna/DataTree/interface/TriggerTable.h"
#include "MitAna/DataTree/interface/TriggerMask.h"
#include "MitAna/DataTree/interface/TriggerObjectCol.h"
#include "MitAna/DataTree/interface/Names.h"

ClassImp(mithep::nero::TriggerFiller)

void
mithep::nero::TriggerFiller::initialize()
{
  out_.triggerFired->resize(triggerNames_.size(), 0);
  out_.triggerPrescale->resize(triggerNames_.size(), 0);
}

void
mithep::nero::TriggerFiller::setCrossRef(BaseFiller* _fillers[])
{
  // need dynamic cast here because BareP4 inherits virtually from BareCollection
  leptons_ = dynamic_cast<BareP4*>(_fillers[kLeptons]->getObject());
  taus_ = dynamic_cast<BareP4*>(_fillers[kTaus]->getObject());
  jets_ = dynamic_cast<BareP4*>(_fillers[kJets]->getObject());
  photons_ = dynamic_cast<BareP4*>(_fillers[kPhotons]->getObject());
}

void
mithep::nero::TriggerFiller::begin()
{
  // load new trigger table
  triggerIds_.clear();
  
  auto* hltTable = getSource<mithep::TriggerTable>(TString::Format("%sFwk", Names::gkHltTableBrn));
  for (auto trigger : triggerNames_) {
    if (trigger.EndsWith("*"))
      trigger.ReplaceAll("*", "");
    auto* triggerName = hltTable->GetWildcard(trigger);
    if (triggerName)
      triggerIds_.push_back(triggerName->Id());
    else
      triggerIds_.push_back(-1);
  }
}

void
mithep::nero::TriggerFiller::fill()
{
  // TODO: Is prescale data not available in BAMBU??

  out_.triggerFired->assign(triggerNames_.size(), 0);
  out_.triggerPrescale->assign(triggerNames_.size(), 0);

  out_.triggerLeps->assign(leptons_->p4->GetEntries(), 0);
  out_.triggerJets->assign(jets_->p4->GetEntries(), 0);
  out_.triggerTaus->assign(taus_->p4->GetEntries(), 0);
  out_.triggerPhotons->assign(photons_->p4->GetEntries(), 0);

  auto* triggerMask = getSource<mithep::TriggerMask>(Names::gkHltBitBrn);
  if (!triggerMask)
    return;

  for (unsigned iT(0); iT != triggerNames_.size(); ++iT) {
    if (!triggerMask->At(triggerIds_[iT]))
      continue;

    out_.triggerFired->at(iT) = 1;

    auto& objsName(triggerObjectsNames_[triggerNames_[iT]]);
    auto* triggerObjects = getSource<mithep::TriggerObjectCol>(objsName);

    for (unsigned iO(0); iO != triggerObjects->GetEntries(); ++iO) {
      auto& trigObj(*triggerObjects->At(iO));

      std::vector<int>* bitmasks = 0;
      BareP4* matchTarget = 0;
      unsigned pdgId(0);
      switch (trigObj.TriggerType()) {
      case mithep::TriggerObject::TriggerPhoton:
        bitmasks = out_.triggerPhotons;
        matchTarget = photons_;
        break;
      case mithep::TriggerObject::TriggerElectron:
        bitmasks = out_.triggerLeps;
        matchTarget = leptons_;
        pdgId = 11;
        break;
      case mithep::TriggerObject::TriggerMuon:
        bitmasks = out_.triggerLeps;
        matchTarget = leptons_;
        pdgId = 13;
        break;
      case mithep::TriggerObject::TriggerTau:
        bitmasks = out_.triggerTaus;
        matchTarget = taus_;
        break;
      case mithep::TriggerObject::TriggerJet:
      case mithep::TriggerObject::TriggerBJet:
        bitmasks = out_.triggerJets;
        matchTarget = jets_;
        break;
      default:
        continue;
      }

      TLorentzVector objP4(trigObj.Px(), trigObj.Py(), trigObj.Pz(), trigObj.E());

      if (bitmasks && matchTarget) {
        double minDR(std::numeric_limits<double>::max());
        int minIndex(-1);

        for (int iP(0); iP < matchTarget->p4->GetEntries(); ++iP) {
          if (pdgId != 0 && std::abs(dynamic_cast<BareLeptons*>(matchTarget)->pdgId->at(iP)) != pdgId)
            continue;

          double dR(static_cast<TLorentzVector*>(matchTarget->p4->At(iP))->DeltaR(objP4));
          if (dR < 0.2 && dR < minDR) {
            minIndex = iP;
            minDR = dR;
          }
        }
        
        if (minIndex >= 0)
          bitmasks->at(minIndex) |= (1 << iT);
      }
    }
  }
}

void
mithep::nero::TriggerFiller::AddTriggerName(char const* _name)
{
  triggerNames_.emplace_back(_name);
}
