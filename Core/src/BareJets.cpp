#include "NeroProducer/Core/interface/BareJets.hpp"

BareJets::BareJets(): BareP4(){
}

BareJets::~BareJets(){
    delete rawPt;
    delete bDiscr;
    delete bDiscrLegacy;
    delete puId;
    delete unc;
    delete qgl;
    delete flavour;
    delete matchedPartonPdgId;
    delete motherPdgId;
    delete grMotherPdgId;
    delete mjId;
    delete mjId_loose;
    delete Q;
    delete QnoPU;
}

void BareJets::init(){
    BareP4::init();

    if (!rawPt)
        rawPt = new vector<float>;
    //
    if (!bDiscr)
        bDiscr = new vector<float>;
    //
    if (!bDiscrLegacy)
        bDiscrLegacy = new vector<float>;
    //	
    if (!puId)
        puId = new vector<float>;
    // -- JES uncertainty
    if (!unc)
        unc = new vector<float>;
    // --QGL
    if (!qgl)
        qgl = new vector<float>;
    // -- Jet Flavour by PAT
    if (!flavour)
        flavour = new vector<int>;

    if (!matchedPartonPdgId)
        matchedPartonPdgId = new vector<int>;
    if (!motherPdgId)
        motherPdgId = new vector<int>;
    if (!grMotherPdgId)
        grMotherPdgId = new vector<int>;

    if (!mjId)
        mjId = new vector<bool>;
    if (!mjId_loose)
        mjId_loose = new vector<bool>;

    if (!Q)
        Q = new vector<float>;
    if (!QnoPU)
        QnoPU = new vector<float>;
}

void BareJets::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
    rawPt -> clear();
    bDiscr -> clear();
    bDiscrLegacy -> clear();
    puId -> clear();
    unc -> clear();
    qgl -> clear();
    // gen matching
    flavour -> clear();
    matchedPartonPdgId -> clear();
    motherPdgId -> clear();
    grMotherPdgId -> clear();

    mjId -> clear();
    mjId_loose -> clear();

    Q->clear();
    QnoPU->clear();
}

void BareJets::defineBranches(TTree *t){
    //
    //t->Branch("jetP4","TClonesArray", &p4, 128000, 0);
    BareP4::defineBranches(t, "jet" );
    //
    t->Branch("jetRawPt","vector<float>",&rawPt);
    //
    t->Branch("jetBdiscr","vector<float>",&bDiscr);
    //
    t->Branch("jetBdiscrLegacy","vector<float>",&bDiscrLegacy);
    //	
    t->Branch("jetPuId","vector<float>",&puId);
    // -- JES uncertainty
    t->Branch("jetUnc","vector<float>",&unc);
    // --QGL
    t->Branch("jetQGL","vector<float>",&qgl);
    // -- Jet Flavour by PAT
    t->Branch("jetFlavour","vector<int>",&flavour);

    t->Branch("jetMatchedPartonPdgId","vector<int>",&matchedPartonPdgId);
    
    t->Branch("jetMotherPdgId","vector<int>",&motherPdgId);
    
    t->Branch("jetGrMotherPdgId","vector<int>",&grMotherPdgId);
    
    t->Branch("jetMonojetId","vector<bool>",&mjId);

    t->Branch("jetMonojetIdLoose","vector<bool>",&mjId_loose);

    t->Branch("jetQ","vector<float>",&Q);

    t->Branch("jetQnoPU","vector<float>",&QnoPU);
}

void BareJets::setBranchAddresses(TTree* t)
{
    BareP4::setBranchAddresses(t,"jet");
    //p4 = new TClonesArray("TLorentzVector", 20);
    //if (t->GetBranchStatus("jetP4"))
        //t->SetBranchAddress("jetP4"	, &p4	);
    if (t->GetBranchStatus("jetRawPt"))
        t->SetBranchAddress("jetRawPt"	,&rawPt);
    if (t->GetBranchStatus("jetBdiscr"))
        t->SetBranchAddress("jetBdiscr"	,&bDiscr);
    if (t->GetBranchStatus("jetBdiscrLegacy"))
        t->SetBranchAddress("jetBdiscrLegacy"	,&bDiscrLegacy);
    if (t->GetBranchStatus("jetPuId"))
        t->SetBranchAddress("jetPuId"	,&puId);
    if (t->GetBranchStatus("jetUnc"))
        t->SetBranchAddress("jetUnc"	,&unc);
    if (t->GetBranchStatus("jetQGL"))
        t->SetBranchAddress("jetQGL"	,&qgl);
    if (t->GetBranchStatus("jetFlavour"))
        t->SetBranchAddress("jetFlavour"	,&flavour);
    if (t->GetBranchStatus("jetMatchedPartonPdgId"))
        t->SetBranchAddress("jetMatchedPartonPdgId", &matchedPartonPdgId);
    if (t->GetBranchStatus("jetMotherPdgId"))
        t->SetBranchAddress("jetMotherPdgId", &motherPdgId);
    if (t->GetBranchStatus("jetGrMotherPdgId"))
        t->SetBranchAddress("jetGrMotherPdgId", &grMotherPdgId);
    if (t->GetBranchStatus("jetMonojetId"))
        t->SetBranchAddress("jetMonojetId", &mjId);
    if (t->GetBranchStatus("jetMonojetIdLoose"))
        t->SetBranchAddress("jetMonojetIdLoose", &mjId_loose);

    // ---
    if (t->GetBranchStatus("jetQ"))
        t->SetBranchAddress("jetQ",&Q);
    if (t->GetBranchStatus("jetQnoPU"))
        t->SetBranchAddress("jetQnoPU",&QnoPU);

}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
