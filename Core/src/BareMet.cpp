#include "NeroProducer/Core/interface/BareMet.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BareMet::BareMet() : BareP4() {
}

BareMet::~BareMet(){
    BareFunctions::Delete(genP4);
    BareFunctions::Delete(metNoMu);
    BareFunctions::Delete(metPuppi);
    BareFunctions::Delete(metPuppiSyst);
    BareFunctions::Delete(metSyst);
    BareFunctions::Delete(pfMet_e3p0);
    BareFunctions::Delete(trackMet);
    BareFunctions::Delete(neutralMet);
    BareFunctions::Delete(photonMet);
    BareFunctions::Delete(HFMet);
    BareFunctions::Delete(CaloMet);
    BareFunctions::Delete(RawMet);
    BareFunctions::Delete(SmearedMet);

}

void BareMet::init(){
    BareP4::init();

    BareFunctions::New(genP4);
    BareFunctions::New(metPuppi);
    BareFunctions::New(metPuppiSyst);
    BareFunctions::New(metSyst);
    BareFunctions::New(CaloMet);
    BareFunctions::New(RawMet);

    if ( IsExtend() )
    {
        BareFunctions::New(metNoMu);
        BareFunctions::New(pfMet_e3p0);
        BareFunctions::New(trackMet);
        BareFunctions::New(neutralMet);
        BareFunctions::New(photonMet);
        BareFunctions::New(HFMet);
        BareFunctions::New(SmearedMet);
    }
}

void BareMet::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();

    genP4 -> Clear();
    *metPuppi *= 0;
    metPuppiSyst->Clear();
    metSyst->Clear();
    *CaloMet *= 0.;
    *RawMet *= 0.;

    if (extend_)
    {
        *metNoMu *= 0.;
        *pfMet_e3p0 *= 0.;
        *trackMet *= 0.;
        *neutralMet *= 0.;
        *photonMet *= 0.;
        *HFMet *= 0.;
        *SmearedMet *= 0.;
    }
}

void BareMet::defineBranches(TTree *t){
    //
    BareP4::defineBranches(t, "met" );
    t->Branch("metP4_GEN","TClonesArray", &genP4, 128000, 0);
    //
    t->Branch("metPuppi","TLorentzVector",&metPuppi);
    t->Branch("metPuppiSyst","TClonesArray",&metPuppiSyst,128000,0);
    t->Branch("metSyst","TClonesArray",&metSyst,128000,0);
    
    t->Branch("CaloMet","TLorentzVector",&CaloMet);
    t->Branch("RawMet","TLorentzVector",&RawMet);

    if ( IsExtend() )
    {
        t->Branch("metNoMu","TLorentzVector",&metNoMu);
        t->Branch("pfMet_e3p0","TLorentzVector",&pfMet_e3p0);        
        t->Branch("trackMet","TLorentzVector",&trackMet);
        t->Branch("neutralMet","TLorentzVector",&neutralMet);
        t->Branch("photonMet","TLorentzVector",&photonMet);
        t->Branch("HFMet","TLorentzVector",&HFMet);
        t->Branch("SmearedMet","TLorentzVector",&SmearedMet);
    }
    //
}

void BareMet::setBranchAddresses(TTree *t){

    BareP4::setBranchAddresses(t,"met");

    BareFunctions::SetBranchAddress(t,"metP4_GEN"	, &genP4 );
    BareFunctions::SetBranchAddress(t,"metPuppi", &metPuppi);
    BareFunctions::SetBranchAddress(t,"metPuppiSyst", &metPuppiSyst);
    BareFunctions::SetBranchAddress(t,"metSyst", &metSyst);
    BareFunctions::SetBranchAddress(t,"CaloMet",&CaloMet);
    BareFunctions::SetBranchAddress(t,"RawMet",&RawMet);

    if ( IsExtend() ) 
    {
        BareFunctions::SetBranchAddress(t,"metNoMu", &metNoMu);
        BareFunctions::SetBranchAddress(t,"pfMet_e3p0", &pfMet_e3p0);
        BareFunctions::SetBranchAddress(t,"trackMet", &trackMet);
        BareFunctions::SetBranchAddress(t,"neutralMet", &neutralMet);
        BareFunctions::SetBranchAddress(t,"photonMet", &photonMet);
        BareFunctions::SetBranchAddress(t,"HFMet", &HFMet);
        BareFunctions::SetBranchAddress(t,"SmearedMet", &SmearedMet);
    }
}

void BareMet::compress(){

    BareP4::compress();

	for(int i=0;i<genP4->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) genP4->At(i)  );

    BareFunctions::Compress(*metPuppi);

	for(int i=0;i<metPuppiSyst->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) metPuppiSyst->At(i)  );

	for(int i=0;i<metSyst->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) metSyst->At(i)  );

    BareFunctions::Compress(*RawMet);
    BareFunctions::Compress(*CaloMet);

    if ( IsExtend() ) 
    {
        BareFunctions::Compress(*metNoMu);
        BareFunctions::Compress(*pfMet_e3p0);
        BareFunctions::Compress(*trackMet);
        BareFunctions::Compress(*neutralMet);
        BareFunctions::Compress(*photonMet);
        BareFunctions::Compress(*HFMet);
        BareFunctions::Compress(*SmearedMet);
    }

}

BAREREGISTER(BareMet);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
