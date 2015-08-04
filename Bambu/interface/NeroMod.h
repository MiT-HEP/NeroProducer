#ifndef NeroProducer_Bambu_NeroMod_h
#define NeroProducer_Bambu_NeroMod_h

#include "MitAna/TreeMod/interface/BaseMod.h"
#include "NeroProducer/Bambu/interface/Collections.h"
#include "NeroProducer/Bambu/interface/BaseFiller.h"
#include "NeroProducer/Core/interface/BareCollection.hpp"

#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TNamed.h"
#include "TString.h"

namespace mithep {

  class NeroMod : public BaseMod {
  public:
    NeroMod(char const* name = "NeroMod", char const* title = "Nero-Bambu interface");
    ~NeroMod();

    void SetTag(char const* _tag) { tag_.SetTitle(_tag); }
    void SetHead(char const* _head) { head_.SetTitle(_head); }
    void SetInfo(char const* _info) { info_.SetTitle(_info); }
    void SetFileName(char const* _name) { fileName_ = _name; }
    void AddFiller(nero::BaseFiller* _filler) { filler_[_filler->collection()] = _filler; }
    void SetCondition(BaseMod* _mod) { condition_ = _mod; }

    void SetPrintLevel(unsigned _l) { printLevel_ = _l; }

  private:
    void SlaveBegin() override;
    void BeginRun() override;
    void SlaveTerminate() override;
    void Process() override;

    TString fileName_{"nero.root"};
    TTree* eventsTree_ = 0;
    TTree* allTree_ = 0;
    TH1F* hXsec_ = 0;
    
    TNamed tag_{"tag", ""};
    TNamed head_{"head", ""};
    TNamed info_{"info", "Nero"};

    nero::BaseFiller* filler_[nero::nCollections] = {};

    BaseMod* condition_ = 0;

    unsigned printLevel_ = 0;

    ClassDef(NeroMod, 0)
  };

}

#endif
