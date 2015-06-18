#include "NeroProducer/Core/interface/BareJets.hpp"

BareJets::BareJets(){
    p4      = NULL;
    rawPt   = NULL;
    puId    = NULL;
    unc     = NULL;
    qgl     = NULL;
    flavour = NULL;
    mjId    = NULL;
    mjId_loose  = NULL;
}

BareJets::~BareJets(){}


void BareJets::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    p4 -> Clear();
    rawPt -> clear();
    bDiscr -> clear();
    puId -> clear();
    unc -> clear();
    qgl -> clear();
    flavour -> clear();
    mjId -> clear();
    mjId_loose -> clear();
}

void BareJets::defineBranches(TTree *t){
    //
    p4 = new TClonesArray("TLorentzVector", 20);
    t->Branch("jetP4","TClonesArray", &p4, 128000, 0);
    //
    rawPt = new vector<float>;
    t->Branch("jetRawPt","vector<float>",&rawPt);
    //
    bDiscr = new vector<float>;
    t->Branch("jetBdiscr","vector<float>",&bDiscr);
    //	
    puId = new vector<float>;
    t->Branch("jetPuId","vector<float>",&puId);
    // -- JES uncertainty
    unc = new vector<float>;
    t->Branch("jetUnc","vector<float>",&unc);
    // --QGL
    qgl = new vector<float>;
    t->Branch("jetQGL","vector<float>",&qgl);
    // -- Jet Flavour by PAT
    flavour = new vector<int>;
    t->Branch("jetFlavour","vector<int>",&flavour);

    mjId = new vector<bool>;
    t->Branch("jetMonojetId","vector<bool>",&mjId);

    mjId_loose = new vector<bool>;
    t->Branch("jetMonojetIdLoose","vector<bool>",&mjId_loose);
}

void BareJets::setBranchAddresses(TTree*t)
{
    p4 = new TClonesArray("TLorentzVector", 20);
    t->SetBranchAddress("jetP4"	, &p4	);
    rawPt = new vector<float>;
    t->SetBranchAddress("jetRawPt"	,&rawPt);
    bDiscr = new vector<float>;
    t->SetBranchAddress("jetBdiscr"	,&bDiscr);
    puId = new vector<float>;
    t->SetBranchAddress("jetPuId"	,&puId);
    unc = new vector<float>;
    t->SetBranchAddress("jetUnc"	,&unc);
    qgl = new vector<float>;
    t->SetBranchAddress("jetQGL"	,&qgl);
    flavour = new vector<int>;
    t->SetBranchAddress("jetFlavour"	,&flavour);
    mjId= new vector<bool>;
    t->SetBranchAddress("jetMonojetId", &mjId);
    mjId_loose= new vector<bool>;
    t->SetBranchAddress("jetMonojetIdLoose", &mjId_loose);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
