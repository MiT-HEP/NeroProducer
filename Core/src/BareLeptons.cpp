#include "NeroProducer/Core/interface/BareLeptons.hpp"

BareLeptons::BareLeptons():BareP4(){
}

BareLeptons::~BareLeptons(){
    delete pdgId;
    delete iso;
    delete selBits;
    delete lepPfPt;
    delete chIso;
    delete nhIso;
    delete phoIso;
    delete puIso;
}

void BareLeptons::init(){
    BareP4::init();

    if (!pdgId)
        pdgId = new vector<int>;
    //
    if (!iso)
        iso = new vector<float>;
    //
    if (!selBits)
        selBits = new vector<unsigned>;
    //
    if (!lepPfPt)
        lepPfPt = new vector<float>;
    // 
    if (!chIso)
        chIso=new vector<float>;
    if (!nhIso)
        nhIso=new vector<float>;
    if (!phoIso)
        phoIso=new vector<float>;
    if (!puIso)
        puIso=new vector<float>;
}

void BareLeptons::clear(){
    BareP4::clear();

    pdgId -> clear();
    iso -> clear();
    selBits ->clear();
    lepPfPt->clear();    

    chIso->clear();
    nhIso->clear();
    phoIso->clear();
    puIso->clear();
}

void BareLeptons::defineBranches(TTree*t){
    //
    BareP4::defineBranches(t, "lep" );
    //
    t->Branch("lepPdgId","vector<int>",&pdgId);
    //
    t->Branch("lepIso","vector<float>",&iso);
    //
    t->Branch("lepSelBits","vector<unsigned>",&selBits);
    //
    t->Branch("lepPfPt","vector<float>",&lepPfPt);

    // 
    t->Branch("lepChIso","vector<float>",&chIso);
    t->Branch("lepNhIso","vector<float>",&nhIso);
    t->Branch("lepPhoIso","vector<float>",&phoIso);
    t->Branch("lepPuIso","vector<float>",&puIso);
}

void BareLeptons::setBranchAddresses(TTree*t){

    BareP4::setBranchAddresses(t,"lep");

    if (t->GetBranchStatus("lepPdgId"))
        t->SetBranchAddress("lepPdgId"	,&pdgId);
    if (t->GetBranchStatus("lepIso"))
        t->SetBranchAddress("lepIso"	,&iso);
    if (t->GetBranchStatus("lepSelBits"))
        t->SetBranchAddress("lepSelBits"	,&selBits);
    if (t->GetBranchStatus("lepPfPt"))
        t->SetBranchAddress("lepPfPt"	,&lepPfPt);

    if (t->GetBranchStatus("lepChIso"))
        t->SetBranchAddress("lepChIso",&chIso);
    if (t->GetBranchStatus("lepNhIso"))
        t->SetBranchAddress("lepNhIso",&nhIso);
    if (t->GetBranchStatus("lepPhoIso"))
        t->SetBranchAddress("lepPhoIso",&phoIso);
    if (t->GetBranchStatus("lepPuIso"))
        t->SetBranchAddress("lepPuIso",&puIso);
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
