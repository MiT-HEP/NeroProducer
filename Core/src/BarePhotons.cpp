#include "NeroProducer/Core/interface/BarePhotons.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BarePhotons::BarePhotons():BareP4(){
}

BarePhotons::~BarePhotons(){
    BareFunctions::Delete(iso);
    BareFunctions::Delete(sieie);
    BareFunctions::Delete(selBits);
    BareFunctions::Delete(chIso);
    BareFunctions::Delete(nhIso);
    BareFunctions::Delete(phoIso);
    BareFunctions::Delete(puIso);
    BareFunctions::Delete(rawpt);
    BareFunctions::Delete(e55);
    BareFunctions::Delete(hOverE);
    BareFunctions::Delete(chWorstIso);
    BareFunctions::Delete(chIsoMax);
    BareFunctions::Delete(sipip);
    BareFunctions::Delete(sieip);
    BareFunctions::Delete(r9);
    BareFunctions::Delete(s4);
    BareFunctions::Delete(mipEnergy);
    BareFunctions::Delete(time);
    BareFunctions::Delete(timeSpan);
    BareFunctions::Delete(genMatched);
    BareFunctions::Delete(etaSC);
    BareFunctions::Delete(phoP4_smear);
}

void BarePhotons::init(){
    BareP4::init();

    BareFunctions::New(iso);
    BareFunctions::New(sieie);
    BareFunctions::New(selBits);
    BareFunctions::New(chIso);
    BareFunctions::New(nhIso);
    BareFunctions::New(phoIso);
    BareFunctions::New(puIso);
    BareFunctions::New(phoP4_smear);

    if (IsExtend()) {
        BareFunctions::New(rawpt);
        BareFunctions::New(e55);
        BareFunctions::New(hOverE);
        BareFunctions::New(chWorstIso);
        BareFunctions::New(chIsoMax);
        BareFunctions::New(sipip);
        BareFunctions::New(sieip);
        BareFunctions::New(r9);
        BareFunctions::New(etaSC);
        BareFunctions::New(s4);
        BareFunctions::New(mipEnergy);
        BareFunctions::New(time);
        BareFunctions::New(timeSpan);
        BareFunctions::New(genMatched);
    }
}

void BarePhotons::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
    iso -> clear();
    sieie -> clear();
    selBits -> clear();

    chIso -> clear();
    nhIso -> clear();
    phoIso -> clear();
    puIso -> clear();

    phoP4_smear->Clear();

    if (extend_) {
        rawpt->clear();
        e55->clear();
        hOverE->clear();
        chWorstIso->clear();
        chIsoMax->clear();
        sipip->clear();
        sieip->clear();
        r9->clear();
        etaSC->clear();
        s4->clear();
        mipEnergy->clear();
        time->clear();
        timeSpan->clear();
        genMatched->clear();
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
    t->Branch("photonNhIso","vector<float>",&nhIso);
    t->Branch("photonPhoIso","vector<float>",&phoIso);
    t->Branch("photonPuIso","vector<float>",&puIso);

    t->Branch("phoP4_smear","TClonesArray", &phoP4_smear, 128000, 0);

    if (IsExtend()) {
        t->Branch("photonRawPt", "vector<float>", &rawpt);
        t->Branch("photonE55", "vector<float>", &e55);
        t->Branch("photonHOverE", "vector<float>", &hOverE);
        t->Branch("photonChWorstIso", "vector<float>", &chWorstIso);
        t->Branch("photonChIsoMax", "vector<float>", &chIsoMax);
        t->Branch("photonSipip", "vector<float>", &sipip);
        t->Branch("photonSieip", "vector<float>", &sieip);
        t->Branch("photonR9", "vector<float>", &r9);
        t->Branch("photonEtaSC", "vector<float>", &etaSC);
        t->Branch("photonS4", "vector<float>", &s4);
        t->Branch("photonMipEnergy", "vector<float>", &mipEnergy);
        t->Branch("photonTime", "vector<float>", &time);
        t->Branch("photonTimeSpan", "vector<float>", &timeSpan);
        t->Branch("photonGenMatched", "vector<short>", &genMatched);
    }
}

void BarePhotons::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"photon");

    BareFunctions::SetBranchAddress(t,"photonIso"	,&iso);
    BareFunctions::SetBranchAddress(t,"photonSieie"	,&sieie);
    BareFunctions::SetBranchAddress(t,"photonSelBits"	,&selBits);

    BareFunctions::SetBranchAddress(t,"photonChIso",&chIso);
    BareFunctions::SetBranchAddress(t,"photonNhIso",&nhIso);
    BareFunctions::SetBranchAddress(t,"photonPhoIso",&phoIso);
    BareFunctions::SetBranchAddress(t,"photonPuIso",&puIso);

    BareFunctions::SetBranchAddress(t,"phoP4_smear", &phoP4_smear);

    if (IsExtend()) {
        BareFunctions::SetBranchAddress(t, "photonRawPt", &rawpt);
        BareFunctions::SetBranchAddress(t, "photonE55", &e55);
        BareFunctions::SetBranchAddress(t, "photonHOverE", &hOverE);
        BareFunctions::SetBranchAddress(t, "photonChWorstIso", &chWorstIso);
        BareFunctions::SetBranchAddress(t, "photonChIsoMax", &chIsoMax);
        BareFunctions::SetBranchAddress(t, "photonSipip", &sipip);
        BareFunctions::SetBranchAddress(t, "photonSieip", &sieip);
        BareFunctions::SetBranchAddress(t, "photonR9", &r9);
        BareFunctions::SetBranchAddress(t, "photonEtaSC", &etaSC);
        BareFunctions::SetBranchAddress(t, "photonS4", &s4);
        BareFunctions::SetBranchAddress(t, "photonMipEnergy", &mipEnergy);
        BareFunctions::SetBranchAddress(t, "photonTime", &time);
        BareFunctions::SetBranchAddress(t, "photonTimeSpan", &timeSpan);
        BareFunctions::SetBranchAddress(t, "photonGenMatched", &genMatched);
    }
}
BAREREGISTER(BarePhotons);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
