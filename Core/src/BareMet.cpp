#include "NeroProducer/Core/interface/BareMet.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BareMet::BareMet() : BareP4() {
}

BareMet::~BareMet(){
    delete ptJESUP;
    delete ptJESDOWN;
    delete genP4;
    delete metNoMu;
    delete pfMet_e3p0;
    delete metChargedHadron;
    delete metNeutralHadron;
    delete metNeutralEM;
}

void BareMet::init(){
    BareP4::init();

    if (!ptJESUP)
        ptJESUP = new vector<float>;
    //
    if (!ptJESDOWN)
        ptJESDOWN = new vector<float>;
    //	
    if (!genP4)
        genP4 = new TClonesArray("TLorentzVector", 20);
    //

    if ( IsExtend() )
    {
        if (!metNoMu)
            metNoMu = new TLorentzVector;
        //
        if (!pfMet_e3p0)
            pfMet_e3p0 = new TLorentzVector;
        //
        if (!metChargedHadron)
            metChargedHadron = new TLorentzVector;
        //
        if (!metNeutralHadron)
            metNeutralHadron = new TLorentzVector;
        //
        if (!metNeutralEM)
            metNeutralEM = new TLorentzVector;
    }
}

void BareMet::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();

    ptJESUP -> clear();
    ptJESDOWN -> clear();
    genP4 -> Clear();

    if (extend_)
    {
        *metNoMu *= 0.;
        *pfMet_e3p0 *= 0.;
        *metChargedHadron *= 0.;
        *metNeutralHadron *= 0.;
        *metNeutralEM *= 0.;
    }
}

void BareMet::defineBranches(TTree *t){
    //
    BareP4::defineBranches(t, "met" );
    //
    t->Branch("metPtJESUP","vector<float>",&ptJESUP);
    //
    t->Branch("metPtJESDOWN","vector<float>",&ptJESDOWN);
    //	
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
        // calo Met
        t->Branch("caloMet_Pt",&caloMet_Pt,"caloMet_Pt/F");
        t->Branch("caloMet_Phi",&caloMet_Phi,"caloMet_Phi/F");
        t->Branch("caloMet_SumEt",&caloMet_SumEt,"caloMet_SumEt/F");
    }
    //
}

void BareMet::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"met");

    if (t->GetBranchStatus("metPtJESUP"))
        t->SetBranchAddress("metPtJESUP"	,&ptJESUP);
    if (t->GetBranchStatus("metPtJESDOWN"))
        t->SetBranchAddress("metPtJESDOWN",&ptJESDOWN);
    if (t->GetBranchStatus("metP4_GEN"))
        t->SetBranchAddress("metP4_GEN"	, &genP4 );

    if ( IsExtend() ) 
    {
        if (t->GetBranchStatus("metNoMu"))
            t->SetBranchAddress("metNoMu", &metNoMu);
        if (t->GetBranchStatus("pfMet_e3p0"))
            t->SetBranchAddress("pfMet_e3p0", &pfMet_e3p0);
        if (t->GetBranchStatus("metChargedHadron"))
            t->SetBranchAddress("metChargedHadron", &metChargedHadron);
        if (t->GetBranchStatus("metNeutralHadron"))
            t->SetBranchAddress("metNeutralHadron", &metNeutralHadron);
        if (t->GetBranchStatus("metNeutralEM"))
            t->SetBranchAddress("metNeutralEM", &metNeutralEM);
            // calo met
        if (t->GetBranchStatus("caloMet_Pt"))
            t->SetBranchAddress("caloMet_Pt", &caloMet_Pt);
        if (t->GetBranchStatus("caloMet_Phi"))
            t->SetBranchAddress("caloMet_Phi", &caloMet_Phi);
        if (t->GetBranchStatus("caloMet_SumEt"))
            t->SetBranchAddress("caloMet_SumEt", &caloMet_SumEt);
    }
}

void BareMet::compress(){

    BareP4::compress();

	for(int i=0;i<genP4->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) genP4->At(i)  );

    if ( IsExtend() ) 
    {
        BareFunctions::Compress(*metNoMu );
        BareFunctions::Compress(*pfMet_e3p0 );
        BareFunctions::Compress(*metChargedHadron );
        BareFunctions::Compress(*metNeutralHadron );
        BareFunctions::Compress(*metNeutralEM );
    }

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
