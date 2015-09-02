#include "NeroProducer/Core/interface/BarePhotons.hpp"


BarePhotons::BarePhotons():BareP4(){
}

BarePhotons::~BarePhotons(){
    delete iso;
    delete sieie;
    delete tightid;
    delete mediumid;
    delete looseid;
    delete chIso;
    delete chIsoRC;
    delete nhIso;
    delete nhIsoRC;
    delete phoIso;
    delete phoIsoRC;
    delete puIso;
    delete puIsoRC;
}

void BarePhotons::init(){
    BareP4::init();

    if (!iso)
        iso = new vector<float>;
    //
    if (!sieie)
        sieie = new vector<float>;
    //
    if (!tightid)
        tightid = new vector<int>;
    //
    if (!mediumid)
        mediumid = new vector<int>;

    if (!looseid)
        looseid = new vector<int>;

    if (!chIso)
        chIso = new vector<float>;
    if (!chIsoRC)
        chIsoRC = new vector<float>;
    if (!nhIso)
        nhIso = new vector<float>;
    if (!nhIsoRC)
        nhIsoRC = new vector<float>;
    if (!phoIso)
        phoIso = new vector<float>;
    if (!phoIsoRC)
        phoIsoRC = new vector<float>;
    if (!puIso)
        puIso = new vector<float>;
    if (!puIsoRC)
        puIsoRC = new vector<float>;
}

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
    t->Branch("photonIso","vector<float>",&iso);
    //
    t->Branch("photonSieie","vector<float>",&sieie);
    //
    t->Branch("photonTightId","vector<int>",&tightid);
    //
    t->Branch("photonMediumId","vector<int>",&mediumid);

    t->Branch("photonLooseId","vector<int>",&looseid);

    t->Branch("photonChIso","vector<float>",&chIso);
    t->Branch("photonChIsoRC","vector<float>",&chIsoRC);
    t->Branch("photonNhIso","vector<float>",&nhIso);
    t->Branch("photonNhIsoRC","vector<float>",&nhIsoRC);
    t->Branch("photonPhoIso","vector<float>",&phoIso);
    t->Branch("photonPhoIsoRC","vector<float>",&phoIsoRC);
    t->Branch("photonPuIso","vector<float>",&puIso);
    t->Branch("photonPuIsoRC","vector<float>",&puIsoRC);
}

void BarePhotons::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"photon");

    if (t->GetBranchStatus("photonIso"))
        t->SetBranchAddress("photonIso"	,&iso);
    if (t->GetBranchStatus("photonSieie"))
        t->SetBranchAddress("photonSieie"	,&sieie);
    if (t->GetBranchStatus("photonTightId"))
        t->SetBranchAddress("photonTightId"	,&tightid);
    if (t->GetBranchStatus("photonMediumId"))
        t->SetBranchAddress("photonMediumId"	,&mediumid);
    if (t->GetBranchStatus("photonLooseId"))
        t->SetBranchAddress("photonLooseId"	,&looseid);

    if (t->GetBranchStatus("photonChIso"))
        t->SetBranchAddress("photonChIso",&chIso);
    if (t->GetBranchStatus("photonChIsoRC"))
        t->SetBranchAddress("photonChIsoRC",&chIsoRC);
    if (t->GetBranchStatus("photonNhIso"))
        t->SetBranchAddress("photonNhIso",&nhIso);
    if (t->GetBranchStatus("photonNhIsoRC"))
        t->SetBranchAddress("photonNhIsoRC",&nhIsoRC);
    if (t->GetBranchStatus("photonPhoIso"))
        t->SetBranchAddress("photonPhoIso",&phoIso);
    if (t->GetBranchStatus("photonPhoIsoRC"))
        t->SetBranchAddress("photonPhoIsoRC",&phoIsoRC);
    if (t->GetBranchStatus("photonPuIso"))
        t->SetBranchAddress("photonPuIso",&puIso);
    if (t->GetBranchStatus("photonPuIsoRC"))
        t->SetBranchAddress("photonPuIsoRC",&puIsoRC);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
