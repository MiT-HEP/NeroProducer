#include "NeroProducer/Bambu/interface/TriggerFiller.h"
#include "NeroProducer/Core/interface/BareLeptons.hpp"

#include "MitAna/DataTree/interface/TriggerTable.h"
#include "MitAna/DataTree/interface/TriggerMask.h"
#include "MitAna/DataTree/interface/TriggerObjectsTable.h"
#include "MitAna/DataTree/interface/TriggerObject.h"
#include "MitAna/DataTree/interface/Names.h"

#include <algorithm>

ClassImp(mithep::nero::TriggerFiller)

void
mithep::nero::TriggerFiller::initialize()
{
  TString triggerNames;
  for (auto& n : triggerNames_)
    triggerNames += n + ",";
  outputFile_->cd();
  TNamed("triggerNames", triggerNames.Data()).Write();

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
    bool isWildcard(false);
    if (trigger.EndsWith("*")) {
      trigger.ReplaceAll("*", "");
      isWildcard = true;
    }

    TriggerName const* triggerName(0);
    if (isWildcard)
      triggerName = hltTable->GetWildcard(trigger);
    else
      triggerName = hltTable->Get(trigger);

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
  auto* triggerObjs = getSource<mithep::TriggerObjectsTable>(TString(Names::gkHltObjBrn) + "Fwk");
  if (!triggerMask || !triggerObjs)
    return;

  std::vector<int>* bitmaskArrs[4] = {
    out_.triggerPhotons,
    out_.triggerLeps,
    out_.triggerTaus,
    out_.triggerJets
  };
  BareP4* matchTargets[4] = {
    photons_,
    leptons_,
    taus_,
    jets_
  };

  for (unsigned iT(0); iT != triggerNames_.size(); ++iT) {
    if (!triggerMask->At(triggerIds_[iT]))
      continue;

    out_.triggerFired->at(iT) = 1;

    auto& objsNames(triggerObjectsNames_[iT]);

    if (objsNames.size() != 0) {
      TList const* filterList(triggerObjs->GetList(triggerIds_[iT]));
      for (TObject* obj : *filterList) {
        auto& trigObj(*static_cast<mithep::TriggerObject*>(obj));

        // not very efficient but not worth putting too much time improving this..
        if (std::find(objsNames.begin(), objsNames.end(), trigObj.ModuleName()) == objsNames.end())
          continue;

        TLorentzVector objP4(trigObj.Px(), trigObj.Py(), trigObj.Pz(), trigObj.E());

        for (unsigned iTarg(0); iTarg != 4; ++iTarg) {
          auto* bitmaskArr(bitmaskArrs[iTarg]);
          auto* matchTarget(matchTargets[iTarg]);
        
          double minDR(std::numeric_limits<double>::max());
          int minIndex(-1);

          for (int iP(0); iP < matchTarget->p4->GetEntries(); ++iP) {
            double dR(static_cast<TLorentzVector*>(matchTarget->p4->At(iP))->DeltaR(objP4));
            if (dR < 0.2 && dR < minDR) {
              minIndex = iP;
              minDR = dR;
            }
          }
        
          if (minIndex >= 0)
            bitmaskArr->at(minIndex) |= (1 << iT);
        }
      }
    }
  }
}

void
mithep::nero::TriggerFiller::AddTriggerName(char const* _path)
{
  triggerNames_.emplace_back(_path);
  triggerObjectsNames_.resize(triggerNames_.size());
}

void
mithep::nero::TriggerFiller::AddFilterName(char const* _path, char const* _filter)
{
  auto pItr(std::find(triggerNames_.begin(), triggerNames_.end(), _path));
  if (pItr == triggerNames_.end())
    throw std::runtime_error((TString("AddFilterName could not find a trigger path named ") + _path).Data());

  triggerObjectsNames_[pItr - triggerNames_.begin()].emplace_back(_filter);
}
