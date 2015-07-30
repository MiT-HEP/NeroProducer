#include "NeroProducer/Core/interface/BarePhotons.hpp"


BarePhotons::BarePhotons(){
    p4 = NULL;
    sieie = NULL;
    iso = NULL;
    tightid = NULL;
    mediumid = NULL;
    looseid = NULL;

    chIso = NULL;
    chIsoRC = NULL;
    nhIso = NULL;
    nhIsoRC = NULL;
    phoIso = NULL;
    phoIsoRC = NULL;
    puIso = NULL;
    puIsoRC = NULL;
}

BarePhotons::~BarePhotons(){}

void BarePhotons::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
    iso -> clear();
    sieie -> clear();
    tightid -> clear();
    mediumid -> clear();
    looseid -> clear();

    chIso -> clear();
    chIsoRC -> clear();
    nhIso -> clear();
    nhIsoRC -> clear();
    phoIso -> clear();
    phoIsoRC -> clear();
    puIso -> clear();
    puIsoRC -> clear();
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

    looseid = new vector<int>;
    t->Branch("photonLooseId","vector<int>",&looseid);

    chIso = new vector<float>;
    t->Branch("photonChIso","vector<float>",&chIso);
    chIsoRC = new vector<float>;
    t->Branch("photonChIsoRC","vector<float>",&chIsoRC);
    nhIso = new vector<float>;
    t->Branch("photonNhIso","vector<float>",&nhIso);
    nhIsoRC = new vector<float>;
    t->Branch("photonNhIsoRC","vector<float>",&nhIsoRC);
    phoIso = new vector<float>;
    t->Branch("photonPhoIso","vector<float>",&phoIso);
    phoIsoRC = new vector<float>;
    t->Branch("photonPhoIsoRC","vector<float>",&phoIsoRC);
    puIso = new vector<float>;
    t->Branch("photonPuIso","vector<float>",&puIso);
    puIsoRC = new vector<float>;
    t->Branch("photonPuIsoRC","vector<float>",&puIsoRC);

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
    looseid = new vector<int>;
    t->Branch("photonLooseId"	,&looseid);

    chIso = new vector<float>;
    t->SetBranchAddress("photonChIso",&chIso);
    chIsoRC = new vector<float>;
    t->SetBranchAddress("photonChIsoRC",&chIsoRC);
    nhIso = new vector<float>;
    t->SetBranchAddress("photonNhIso",&nhIso);
    nhIsoRC = new vector<float>;
    t->SetBranchAddress("photonNhIsoRC",&nhIsoRC);
    phoIso = new vector<float>;
    t->SetBranchAddress("photonPhoIso",&phoIso);
    phoIsoRC = new vector<float>;
    t->SetBranchAddress("photonPhoIsoRC",&phoIsoRC);
    puIso = new vector<float>;
    t->SetBranchAddress("photonPuIso",&puIso);
    puIsoRC = new vector<float>;
    t->SetBranchAddress("photonPuIsoRC",&puIsoRC);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
