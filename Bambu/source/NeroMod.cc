#include "NeroProducer/Bambu/interface/NeroMod.h"
#include "NeroProducer/Bambu/interface/TriggerConfig.h"

#include "MitAna/DataCont/interface/BaseCollection.h"

#include "TString.h"

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
  if (config_[nero::kTrigger]) {
    auto& cfg(*static_cast<nero::TriggerConfig*>(config_[nero::kTrigger]));
    for (auto& n : cfg.triggerNames)
      triggerNames += n + ",";
  }
  TNamed("triggerNames", triggerNames.Data()).Write();

  nero::ProductGetter getter([this](char const* _name)->mithep::BaseCollection* {
    return this->GetObject<BaseCollection>(_name);
    });

  for (unsigned iC(0); iC != nero::nCollections; ++iC) {
    if (!config_[iC])
      continue;

    auto&& prod(config_[iC]->create());
    collection_[iC] = prod.first;
    filler_[iC] = prod.second;
    filler_[iC]->setGetter(getter);

    if (iC < nero::nEventObjects)
      collection_[iC]->defineBranches(eventsTree_);
    else
      collection_[iC]->defineBranches(allTree_);
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

  for (unsigned iC(0); iC != nero::nCollections; ++iC) {
    delete collection_[iC];
    delete filler_[iC];
    collection_[iC] = 0;
    filler_[iC] = 0;
  }
}

void
mithep::NeroMod::Process()
{
  for (auto* filler : filler_) {
    if (filler)
      filler->fill();
  }
}
