#include "NeroProducer/Core/interface/BareEvent.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareEvent::BareEvent():
    BareCollection(),
    metfilterNames(new vector<string>)
{
};

BareEvent::~BareEvent(){
    BareFunctions::Delete(metfilterNames);
};

void BareEvent::clear()
{
    // This function clear all the internal storage and init it to an arbitrary value
    isRealData = -1;
    runNum = -1;
    lumiNum = -1;
    eventNum = 0;
    rho = -999;
    
    //not sure if this is a good idea..
    selBits = 0;

}

void BareEvent::defineBranches(TTree *t){
    //
    t->Branch("isRealData"  ,&isRealData   ,"isRealData/I");
    t->Branch("runNum"      ,&runNum       ,"runNum/I");
    t->Branch("lumiNum"     ,&lumiNum      ,"lumiNum/I");
    t->Branch("eventNum"    ,&eventNum     ,"eventNum/l");
    t->Branch("rho"    ,&rho     ,"rho/F");
    t->Branch("filterSelBits", &selBits, "selBits/i");
    if ( IsExtend() ) 
    {
    
    }
}

void BareEvent::setBranchAddresses(TTree *t){
    //
    BareFunctions::SetBranchAddress(t,"isRealData", &isRealData);
    BareFunctions::SetBranchAddress(t,"runNum", &runNum);
    BareFunctions::SetBranchAddress(t,"lumiNum", &lumiNum);
    BareFunctions::SetBranchAddress(t,"eventNum", &eventNum);
    BareFunctions::SetBranchAddress(t,"rho", &rho);
    BareFunctions::SetBranchAddress(t,"filterSelBits",&selBits);

    if (IsExtend() )
    {
    }
}

BAREREGISTER(BareEvent);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
