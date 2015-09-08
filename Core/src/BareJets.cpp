#include "NeroProducer/Core/interface/BareJets.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BareJets::BareJets(): BareP4(){
}

BareJets::~BareJets(){
    BareFunctions::Delete(rawPt);
    BareFunctions::Delete(bDiscr);
    BareFunctions::Delete(bDiscrLegacy);
    BareFunctions::Delete(puId);
    BareFunctions::Delete(unc);
    BareFunctions::Delete(qgl);
    BareFunctions::Delete(flavour);
    BareFunctions::Delete(matchedPartonPdgId);
    BareFunctions::Delete(motherPdgId);
    BareFunctions::Delete(grMotherPdgId);
    BareFunctions::Delete(mjId);
    BareFunctions::Delete(mjId_loose);
    BareFunctions::Delete(Q);
    BareFunctions::Delete(QnoPU);
}

void BareJets::init(){
    BareP4::init();

    BareFunctions::New(rawPt);
    BareFunctions::New(bDiscr);
    BareFunctions::New(bDiscrLegacy);
    BareFunctions::New(puId);
    BareFunctions::New(unc);
    BareFunctions::New(qgl);
    BareFunctions::New(flavour);
    BareFunctions::New(matchedPartonPdgId);
    BareFunctions::New(motherPdgId);
    BareFunctions::New(grMotherPdgId);
    BareFunctions::New(mjId);
    BareFunctions::New(mjId_loose);
    BareFunctions::New(Q);
    BareFunctions::New(QnoPU);

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
    BareFunctions::SetBranchAddress(t,"jetRawPt"	,&rawPt);
    BareFunctions::SetBranchAddress(t,"jetBdiscr"	,&bDiscr);
    BareFunctions::SetBranchAddress(t,"jetBdiscrLegacy"	,&bDiscrLegacy);
    BareFunctions::SetBranchAddress(t,"jetPuId"	,&puId);
    BareFunctions::SetBranchAddress(t,"jetUnc"	,&unc);
    BareFunctions::SetBranchAddress(t,"jetQGL"	,&qgl);
    BareFunctions::SetBranchAddress(t,"jetFlavour"	,&flavour);
    BareFunctions::SetBranchAddress(t,"jetMatchedPartonPdgId", &matchedPartonPdgId);
    BareFunctions::SetBranchAddress(t,"jetMotherPdgId", &motherPdgId);
    BareFunctions::SetBranchAddress(t,"jetGrMotherPdgId", &grMotherPdgId);
    BareFunctions::SetBranchAddress(t,"jetMonojetId", &mjId);
    BareFunctions::SetBranchAddress(t,"jetMonojetIdLoose", &mjId_loose);

    // ---
    BareFunctions::SetBranchAddress(t,"jetQ",&Q);
    BareFunctions::SetBranchAddress(t,"jetQnoPU",&QnoPU);

}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
