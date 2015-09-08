#include "NeroProducer/Core/interface/BareEvent.hpp"

BareEvent::BareEvent():BareCollection(){};
BareEvent::~BareEvent(){};

void BareEvent::clear()
{
    // This function clear all the internal storage and init it to an arbitrary value
    isRealData = -1;
    runNum = -1;
    lumiNum = -1;
    eventNum = 0;
    rho = -999;
    originalRun = -999;
    originalLumi = -999;
    originalEvent = -999;

}

void BareEvent::defineBranches(TTree *t){
    //
    t->Branch("isRealData"  ,&isRealData   ,"isRealData/I");
    t->Branch("runNum"      ,&runNum       ,"runNum/I");
    t->Branch("lumiNum"     ,&lumiNum      ,"lumiNum/I");
    t->Branch("eventNum"    ,&eventNum     ,"eventNum/l");
    t->Branch("rho"    ,&rho     ,"rho/F");
    if ( IsExtend() ) 
    {
        t->Branch("originalRun"      ,&originalRun       ,"originalRun/I");
        t->Branch("originalLumi"      ,&originalLumi       ,"originalLumi/I");
        t->Branch("originalEvent"      ,&originalEvent       ,"originalEvent/I");
    
    }
}

void BareEvent::setBranchAddresses(TTree *t){
    //
    if (t->GetBranchStatus("isRealData"))
        t->SetBranchAddress("isRealData"  ,&isRealData  );
    if (t->GetBranchStatus("runNum"))
        t->SetBranchAddress("runNum"      ,&runNum      );
    if (t->GetBranchStatus("lumiNum"))
        t->SetBranchAddress("lumiNum"     ,&lumiNum     );
    if (t->GetBranchStatus("eventNum"))
        t->SetBranchAddress("eventNum"    ,&eventNum    );
    if (t->GetBranchStatus("rho"))
        t->SetBranchAddress("rho"    	,&rho         );
    if (IsExtend() )
    {
        if (t->GetBranchStatus("originalRun"))
            t->SetBranchAddress("originalRun"      ,&originalRun      );
        if (t->GetBranchStatus("originalLumi"))
            t->SetBranchAddress("originalLumi"      ,&originalLumi      );
        if (t->GetBranchStatus("originalEvent"))
            t->SetBranchAddress("originalEvent"      ,&originalEvent      );
    }
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
