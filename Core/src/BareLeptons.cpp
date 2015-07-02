#include "NeroProducer/Core/interface/BareLeptons.hpp"

BareLeptons::BareLeptons(){
    p4 = NULL;
    pdgId = NULL;
    iso = NULL;
    tightId = NULL;
    lepPfPt = NULL;
}

BareLeptons::~BareLeptons(){
}

void BareLeptons::clear(){
    BareP4::clear();

    pdgId -> clear();
    iso -> clear();
    tightId ->clear();
    lepPfPt->clear();    
}

void BareLeptons::defineBranches(TTree*t){
    //
    BareP4::defineBranches(t, "lep" );
    //
    pdgId = new vector<int>;
    t->Branch("lepPdgId","vector<int>",&pdgId);
    //
    iso = new vector<float>;
    t->Branch("lepIso","vector<float>",&iso);
    //
    tightId = new vector<int>;
    t->Branch("lepTightId","vector<int>",&tightId);
    //
    lepPfPt = new vector<float>;
    t->Branch("lepPfPt","vector<float>",&lepPfPt);

}

void BareLeptons::setBranchAddresses(TTree*t){

    BareP4::setBranchAddresses(t,"lep");

    pdgId = new vector<int>;
    t->SetBranchAddress("lepPdgId"	,&pdgId);
    iso = new vector<float>;
    t->SetBranchAddress("lepIso"	,&iso);
    tightId = new vector<int>;
    t->SetBranchAddress("lepTightId"	,&tightId);
    lepPfPt = new vector<float>;
    t->SetBranchAddress("lepPfPt"	,&lepPfPt);

}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
