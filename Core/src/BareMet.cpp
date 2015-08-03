#include "NeroProducer/Core/interface/BareMet.hpp"


BareMet::BareMet(){
    p4 = NULL;
    ptJESUP = NULL;
    ptJESDOWN = NULL;
    genP4 = NULL;


    metNoMu = NULL;
    pfMet_e3p0 = NULL;
    //phiNoMu = 0.;
    metChargedHadron = NULL;
    //phiChargedHadron = 0.;
    metNeutralHadron = NULL;
    //phiNeutralHadron = 0.;
    metNeutralEM = NULL;
    //phiNeutralEM = 0.;
}

BareMet::~BareMet(){
}

void BareMet::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();

    ptJESUP -> clear();
    ptJESDOWN -> clear();
    genP4 -> Clear();

    metNoMu -> Clear();
    pfMet_e3p0 -> Clear();
    //phiNoMu = 0.;
    metChargedHadron -> Clear();
    //phiChargedHadron = 0.;
    metNeutralHadron -> Clear();
    //phiNeutralHadron = 0.;
    metNeutralEM -> Clear();
    //phiNeutralEM = 0.;
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
    t->Branch("metNoMu","TLorentzVector",&metNoMu);
    t->Branch("pfMet_e3p0","TLorentzVector",&pfMet_e3p0);
    //
    //t->Branch("metNoMu",&phiNoMu,"Phi/F");
    //
    t->Branch("metChargedHadron","TLorentzVector",&metChargedHadron);
    //
    //t->Branch("metChargedHadron",&phiChargedHadron,"Phi/F");
    //
    t->Branch("metNeutralHadron","TLorentzVector",&metNeutralHadron);
    //
    //t->Branch("metNeutralHadron",&phiNeutralHadron,"Phi/F");
    //
    t->Branch("metNeutralEM","TLorentzVector",&metNeutralEM);
    //
    //t->Branch("metNeutralEM",&phiNeutralEM,"Phi/F");
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
    t->SetBranchAddress("pfMet_e3p0", &pfMet_e3p0);
    //t->SetBranchAddress("phiNoMu", &phiNoMu);
    t->SetBranchAddress("metChargedHadron", &metChargedHadron);
    //t->SetBranchAddress("phiChargedHadron", &phiChargedHadron);
    t->SetBranchAddress("metNeutralHadron", &metNeutralHadron);
    //t->SetBranchAddress("phiNeutralHadron", &phiNeutralHadron);
    t->SetBranchAddress("metNeutralEM", &metNeutralEM);
    //t->SetBranchAddress("phiNeutralEM", &phiNeutralEM);
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
