#include "NeroProducer/Core/interface/BarePhotons.hpp"


BarePhotons::BarePhotons(){
    p4 = NULL;
    sieie = NULL;
    iso = NULL;
    tightid = NULL;
    mediumid = NULL;
}

BarePhotons::~BarePhotons(){}

void BarePhotons::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
    iso -> clear();
    sieie -> clear();
    tightid -> clear();
    mediumid -> clear();
}

void BarePhotons::defineBranches(TTree *t){
    //
    BareP4::defineBranches(t, "photon" );
    //
    iso = new vector<float>;
    t->Branch("photonIso","vector<float>",&iso);
    //
    sieie = new vector<float>;
    t->Branch("photonSieie","vector<float>",&sieie);
    //
    tightid = new vector<int>;
    t->Branch("photonTightId","vector<int>",&tightid);
    //
    mediumid = new vector<int>;
    t->Branch("photonMediumId","vector<int>",&mediumid);

}

void BarePhotons::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"photon");

    iso = new vector<float>;
    t->Branch("photonIso"	,&iso);
    sieie = new vector<float>;
    t->Branch("photonSieie"	,&sieie);
    tightid = new vector<int>;
    t->Branch("photonTightId"	,&tightid);
    mediumid = new vector<int>;
    t->Branch("photonMediumId"	,&mediumid);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
