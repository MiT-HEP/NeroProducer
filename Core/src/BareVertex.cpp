#include "NeroProducer/Core/interface/BareVertex.hpp"


BareVertex::BareVertex(){
}

BareVertex::~BareVertex(){
}

void BareVertex::clear(){
    npv= -1;
}

void BareVertex::defineBranches(TTree *t){
    // nothing to be done
    t->Branch("npv"     ,&npv      ,"npv/I");
}
void BareVertex::setBranchAddresses(TTree *t){
    // nothing to be done
    t->SetBranchAddress("npv"     ,&npv );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
