#include "NeroProducer/Core/interface/BareAll.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareAll::BareAll(): BareCollection(){}
BareAll::~BareAll(){}

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
    BareFunctions::SetBranchAddress(t,"isRealData"  ,&isRealData   );
    BareFunctions::SetBranchAddress(t,"runNum"      ,&runNum       );
    BareFunctions::SetBranchAddress(t,"lumiNum"     ,&lumiNum      );
    BareFunctions::SetBranchAddress(t,"eventNum"    ,&eventNum     );
    BareFunctions::SetBranchAddress(t,"mcWeight"    ,&mcWeight     );
    BareFunctions::SetBranchAddress(t,"puTrueInt"     ,&puTrueInt  );
}

BAREREGISTER(BareAll);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
