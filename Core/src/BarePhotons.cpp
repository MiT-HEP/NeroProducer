#include "NeroProducer/Core/interface/BarePhotons.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BarePhotons::BarePhotons():BareP4(){
}

BarePhotons::~BarePhotons(){
    BareFunctions::Delete(iso);
    BareFunctions::Delete(sieie);
    BareFunctions::Delete(selBits);
    BareFunctions::Delete(chIso);
    BareFunctions::Delete(chIsoRC);
    BareFunctions::Delete(nhIso);
    BareFunctions::Delete(nhIsoRC);
    BareFunctions::Delete(phoIso);
    BareFunctions::Delete(phoIsoRC);
    BareFunctions::Delete(puIso);
    BareFunctions::Delete(puIsoRC);
    BareFunctions::Delete(rawpt);
}

void BarePhotons::init(){
    BareP4::init();

    BareFunctions::New(iso);
    BareFunctions::New(sieie);
    BareFunctions::New(selBits);
    BareFunctions::New(chIso);
    BareFunctions::New(chIsoRC);
    BareFunctions::New(nhIso);
    BareFunctions::New(nhIsoRC);
    BareFunctions::New(phoIso);
    BareFunctions::New(phoIsoRC);
    BareFunctions::New(puIso);
    BareFunctions::New(puIsoRC);

    if (IsExtend()) {
        BareFunctions::New(rawpt);
    }
}

void BarePhotons::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
    iso -> clear();
    sieie -> clear();
    selBits -> clear();

    chIso -> clear();
    chIsoRC -> clear();
    nhIso -> clear();
    nhIsoRC -> clear();
    phoIso -> clear();
    phoIsoRC -> clear();
    puIso -> clear();
    puIsoRC -> clear();

    if (extend_) {
        rawpt->clear();
    }
}

void BarePhotons::defineBranches(TTree *t){
    //
    BareP4::defineBranches(t, "photon" );
    //
    t->Branch("photonIso","vector<float>",&iso);
    //
    t->Branch("photonSieie","vector<float>",&sieie);
    //
    t->Branch("photonSelBits","vector<unsigned>",&selBits);
    //

    t->Branch("photonChIso","vector<float>",&chIso);
    t->Branch("photonChIsoRC","vector<float>",&chIsoRC);
    t->Branch("photonNhIso","vector<float>",&nhIso);
    t->Branch("photonNhIsoRC","vector<float>",&nhIsoRC);
    t->Branch("photonPhoIso","vector<float>",&phoIso);
    t->Branch("photonPhoIsoRC","vector<float>",&phoIsoRC);
    t->Branch("photonPuIso","vector<float>",&puIso);
    t->Branch("photonPuIsoRC","vector<float>",&puIsoRC);

    if (IsExtend()) {
        t->Branch("photonRawPt", "vector<float>", &rawpt);
    }
}

void BarePhotons::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"photon");

    BareFunctions::SetBranchAddress(t,"photonIso"	,&iso);
    BareFunctions::SetBranchAddress(t,"photonSieie"	,&sieie);
    BareFunctions::SetBranchAddress(t,"photonSelBits"	,&selBits);

    BareFunctions::SetBranchAddress(t,"photonChIso",&chIso);
    BareFunctions::SetBranchAddress(t,"photonChIsoRC",&chIsoRC);
    BareFunctions::SetBranchAddress(t,"photonNhIso",&nhIso);
    BareFunctions::SetBranchAddress(t,"photonNhIsoRC",&nhIsoRC);
    BareFunctions::SetBranchAddress(t,"photonPhoIso",&phoIso);
    BareFunctions::SetBranchAddress(t,"photonPhoIsoRC",&phoIsoRC);
    BareFunctions::SetBranchAddress(t,"photonPuIso",&puIso);
    BareFunctions::SetBranchAddress(t,"photonPuIsoRC",&puIsoRC);

    if (IsExtend()) {
        BareFunctions::SetBranchAddress(t, "photonRawPt", &rawpt);
    }
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
