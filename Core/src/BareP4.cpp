#include "NeroProducer/Core/interface/BareP4.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

#include <iostream>
using namespace std;

BareP4::BareP4() : BareCollection(){
}

BareP4::~BareP4(){
    BareFunctions::Delete(p4);
}

void BareP4::init(){
    BareFunctions::New(p4);
}

void BareP4::clear(){ 

    p4->Clear() ; 

}

void BareP4::defineBranches(TTree * t, string prefix)
{
    init();

    t->Branch( (prefix + "P4").c_str(),"TClonesArray", &p4, 128000, 0);

}

void BareP4::setBranchAddresses(TTree* t, string prefix)
{
    init();

    BareFunctions::SetBranchAddress(t, (prefix+"P4").c_str(), &p4	);

}

void BareP4::compress(){
    for(int i=0;i<p4->GetEntries();++i)
        BareFunctions::Compress( * (TLorentzVector*) p4->At(i)  );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
