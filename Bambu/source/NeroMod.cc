#include "NeroProducer/Bambu/interface/NeroMod.h"
#include "NeroProducer/Bambu/interface/TriggerFiller.h"
#include "NeroProducer/Bambu/interface/AllFiller.h"

#include "MitAna/TreeMod/interface/HLTFwkMod.h"
#include "MitAna/DataTree/interface/EventHeaderCol.h"

#include <exception>

ClassImp(mithep::NeroMod)

mithep::NeroMod::NeroMod(char const* _name/* = "NeroMod"*/, char const* _title/* = "Nero-Bambu interface"*/) :
  BaseMod(_name, _title)
{
}

mithep::NeroMod::~NeroMod()
{
}

void
mithep::NeroMod::SlaveBegin()
{
  auto* outputFile = TFile::Open(fileName_, "recreate");
  auto* dir = outputFile->mkdir("nero");
  dir->cd();
  eventsTree_ = new TTree("events", "events");
  allTree_ = new TTree("all", "all");
  hXsec_ = new TH1F("xSec", "xSec", 20, -0.5, 19.5);
  hXsec_->Sumw2();

  tag_.Write();
  head_.Write();
  info_.Write();

  TString triggerNames;
  if (filler_[nero::kTrigger]) {
    auto& filler(*static_cast<nero::TriggerFiller*>(filler_[nero::kTrigger]));
    for (auto& n : filler.triggerNames())
      triggerNames += n + ",";
  }
  TNamed("triggerNames", triggerNames.Data()).Write();

  if (filler_[nero::kAll]) {
    auto* skippedEvents = GetPublicObj<mithep::EventHeaderCol>(mithep::Names::gkSkimmedHeaders, false);
    if (skippedEvents)
      static_cast<nero::AllFiller*>(filler_[nero::kAll])->setSkippedEvents(skippedEvents);
  }

  nero::BaseFiller::ProductGetter getter([this](char const* _name)->TObject* {
      return this->GetObject<TObject>(_name);
    });

  for (unsigned iC(0); iC != nero::nCollections; ++iC) {
    if (!filler_[iC])
      continue;

    filler_[iC]->setProductGetter(getter);

    if (printLevel_ > 0)
      std::cout << "creating branches for " << filler_[iC]->getObject()->name() << std::endl;

    if (iC < nero::nEventObjects)
      filler_[iC]->getObject()->defineBranches(eventsTree_);
    else
      filler_[iC]->getObject()->defineBranches(allTree_);

    if (printLevel_ > 0)
      std::cout << "initializing " << filler_[iC]->getObject()->name() << std::endl;

    filler_[iC]->setCrossRef(filler_);
    filler_[iC]->initialize();
  }
}

void
mithep::NeroMod::BeginRun()
{
  // if other fillers need similar switching, might make sense to define a conditions function on the filler side
  if (filler_[nero::kTrigger] && (!HasHLTInfo() || !GetHltFwkMod()->HasData()))
    filler_[nero::kTrigger]->disactivate();

  for (unsigned iC(0); iC != nero::nCollections; ++iC) {
    if (!filler_[iC])
      continue;

    if (printLevel_ > 0)
      std::cout << "begin run for " << filler_[iC]->getObject()->name() << std::endl;

    filler_[iC]->callBegin();
  }
}

void
mithep::NeroMod::SlaveTerminate()
{
  TFile* outputFile = eventsTree_->GetCurrentFile();
  outputFile->cd();
  outputFile->Write();
  delete outputFile;

  eventsTree_ = allTree_ = 0;
  hXsec_ = 0;
}

void
mithep::NeroMod::Process()
{
  // always fill allevents
  nero::AllFiller* allFiller = 0;

  if (filler_[nero::kAll]) {
    allFiller = static_cast<nero::AllFiller*>(filler_[nero::kAll]);
    if (printLevel_ > 1)
      std::cout << "filling " << allFiller->getObject()->name() << std::endl;

    allFiller->getObject()->clear();
    try {
      allFiller->callFill();
    }
    catch (std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      AbortAnalysis();
    }

    if (printLevel_ > 1)
      std::cout << "filled " << allFiller->getObject()->name() << std::endl;
  }

  // skip event if condition module is aborted
  if (condition_ && !condition_->IsActive())
    return;

  for (auto* filler : filler_) {
    if (filler == allFiller)
      continue;

    if (filler) {
      if (printLevel_ > 1)
        std::cout << "filling " << filler->getObject()->name() << std::endl;

      filler->getObject()->clear();
      try {
        filler->callFill();
      }
      catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        AbortAnalysis();
      }

      if (printLevel_ > 1)
        std::cout << "filled " << filler->getObject()->name() << std::endl;
    }
  }

  eventsTree_->Fill();
  allTree_->Fill();
}
