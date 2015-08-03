#include "NeroProducer/Core/interface/BareMet.hpp"


BareMet::BareMet(){
    p4 = NULL;
    ptJESUP = NULL;
    ptJESDOWN = NULL;
    genP4 = NULL;


    metNoMu = NULL;
    pfMet_e3p0 = NULL;
    metChargedHadron = NULL;
    metNeutralHadron = NULL;
    metNeutralEM = NULL;
}

BareMet::~BareMet(){
}

void BareMet::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();

    ptJESUP -> clear();
    ptJESDOWN -> clear();
    genP4 -> Clear();

    if (extend_)
    {
        metNoMu -> Clear();
        pfMet_e3p0 -> Clear();
        metChargedHadron -> Clear();
        metNeutralHadron -> Clear();
        metNeutralEM -> Clear();
    }
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
    if ( IsExtend() )
    {
        t->Branch("metNoMu","TLorentzVector",&metNoMu);
        //
        t->Branch("pfMet_e3p0","TLorentzVector",&pfMet_e3p0);
        //
        t->Branch("metChargedHadron","TLorentzVector",&metChargedHadron);
        //
        t->Branch("metNeutralHadron","TLorentzVector",&metNeutralHadron);
        //
        t->Branch("metNeutralEM","TLorentzVector",&metNeutralEM);
    }
    //
}

void BareMet::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"met");

    ptJESUP = new vector<float>;
    t->SetBranchAddress("metPtJESUP"	,&ptJESUP);
    ptJESDOWN = new vector<float>;
    t->SetBranchAddress("metPtJESDOWN",&ptJESDOWN);
    genP4 = new TClonesArray("TLorentzVector", 20);
    t->SetBranchAddress("metP4_GEN"	, &genP4 );

    if ( IsExtend() ) 
    {
        t->SetBranchAddress("metNoMu", &metNoMu);
        t->SetBranchAddress("pfMet_e3p0", &pfMet_e3p0);
        t->SetBranchAddress("metChargedHadron", &metChargedHadron);
        t->SetBranchAddress("metNeutralHadron", &metNeutralHadron);
        t->SetBranchAddress("metNeutralEM", &metNeutralEM);
    }
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
