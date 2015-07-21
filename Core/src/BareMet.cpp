#include "NeroProducer/Core/interface/BareMet.hpp"


BareMet::BareMet(){
    p4 = NULL;
    ptJESUP = NULL;
    ptJESDOWN = NULL;
    genP4 = NULL;
    metNoMu = -1.0;
}

BareMet::~BareMet(){
}

void BareMet::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();

    ptJESUP -> clear();
    ptJESDOWN -> clear();
    genP4 -> Clear();
}

void BareMet::defineBranches(TTree *t){
    //
    BareP4::defineBranches(t, "met" );
    //
    ptJESUP = new vector<float>;
    t->Branch("metPtJESUP","vector<float>",&ptJESUP);
    //
    ptJESDOWN = new vector<float>;
    t->Branch("metPtJESDOWN","vector<float>",&ptJESDOWN);
    //	
    genP4 = new TClonesArray("TLorentzVector", 20);
    t->Branch("metP4_GEN","TClonesArray", &p4, 128000, 0);
    //
    t->Branch("metNoMu",&metNoMu,"metNoMu/F");

}

void BareMet::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"met");

    ptJESUP = new vector<float>;
    t->SetBranchAddress("metPtJESUP"	,&ptJESUP);
    ptJESDOWN = new vector<float>;
    t->SetBranchAddress("metPtJESDOWN",&ptJESDOWN);
    genP4 = new TClonesArray("TLorentzVector", 20);
    t->SetBranchAddress("metP4_GEN"	, &genP4 );

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
