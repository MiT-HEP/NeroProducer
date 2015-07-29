#include "NeroProducer/Core/interface/BareMet.hpp"


BareMet::BareMet(){
    p4 = NULL;
    ptJESUP = NULL;
    ptJESDOWN = NULL;
    genP4 = NULL;
    metNoMu = -1.0;
    phiNoMu = 0.;
    metChargedHadron = -1.0;
    phiChargedHadron = 0.;
    metNeutralHadron = -1.0;
    phiNeutralHadron = 0.;
    metNeutralEM = -1.0;
    phiNeutralEM = 0.;
}

BareMet::~BareMet(){
}

void BareMet::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();

    ptJESUP -> clear();
    ptJESDOWN -> clear();
    genP4 -> Clear();

    metNoMu = -1.0;
    phiNoMu = 0.;
    metChargedHadron = -1.0;
    phiChargedHadron = 0.;
    metNeutralHadron = -1.0;
    phiNeutralHadron = 0.;
    metNeutralEM = -1.0;
    phiNeutralEM = 0.;
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
    //
    t->Branch("phiNoMu",&phiNoMu,"phiNoMu/F");
    //
    t->Branch("metChargedHadron",&metChargedHadron,"metChargedHadron/F");
    //
    t->Branch("phiChargedHadron",&phiChargedHadron,"phiChargedHadron/F");
    //
    t->Branch("metNeutralHadron",&metNeutralHadron,"metNeutralHadron/F");
    //
    t->Branch("phiNeutralHadron",&phiNeutralHadron,"phiNeutralHadron/F");
    //
    t->Branch("metNeutralEM",&metNeutralEM,"metNeutralEM/F");
    //
    t->Branch("phiNeutralEM",&phiNeutralEM,"phiNeutralEM/F");
}

void BareMet::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"met");

    ptJESUP = new vector<float>;
    t->SetBranchAddress("metPtJESUP"	,&ptJESUP);
    ptJESDOWN = new vector<float>;
    t->SetBranchAddress("metPtJESDOWN",&ptJESDOWN);
    genP4 = new TClonesArray("TLorentzVector", 20);
    t->SetBranchAddress("metP4_GEN"	, &genP4 );

    t->SetBranchAddress("metNoMu", &metNoMu);
    t->SetBranchAddress("phiNoMu", &phiNoMu);
    t->SetBranchAddress("metChargedHadron", &metChargedHadron);
    t->SetBranchAddress("phiChargedHadron", &phiChargedHadron);
    t->SetBranchAddress("metNeutralHadron", &metNeutralHadron);
    t->SetBranchAddress("phiNeutralHadron", &phiNeutralHadron);
    t->SetBranchAddress("metNeutralEM", &metNeutralEM);
    t->SetBranchAddress("phiNeutralEM", &phiNeutralEM);
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
