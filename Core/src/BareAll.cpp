#include "NeroProducer/Core/interface/BareAll.hpp"

BareAll::BareAll(): BareCollection(){
};
BareAll::~BareAll(){};

void BareAll::clear()
{
    isRealData = -1;
    runNum = -1;
    lumiNum = -1;
    eventNum = 0;
    mcWeight = 1;
    puTrueInt = -1;
}

void BareAll::defineBranches(TTree *t){
    //
    t->Branch("isRealData"  ,&isRealData   ,"isRealData/I");
    t->Branch("runNum"      ,&runNum       ,"runNum/I");
    t->Branch("lumiNum"     ,&lumiNum      ,"lumiNum/I");
    t->Branch("eventNum"    ,&eventNum     ,"eventNum/l");
    t->Branch("mcWeight"    ,&mcWeight     ,"mcWeight/F");
    t->Branch("puTrueInt"     ,&puTrueInt      ,"puTrueInt/I");
}

void BareAll::setBranchAddresses(TTree *t){
    //
    t->SetBranchAddress("isRealData"  ,&isRealData   );
    t->SetBranchAddress("runNum"      ,&runNum       );
    t->SetBranchAddress("lumiNum"     ,&lumiNum      );
    t->SetBranchAddress("eventNum"    ,&eventNum     );
    t->SetBranchAddress("mcWeight"    ,&mcWeight     );
    t->SetBranchAddress("puTrueInt"     ,&puTrueInt  );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
