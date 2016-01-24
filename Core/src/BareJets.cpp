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
    BareFunctions::Delete(selBits);
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
    BareFunctions::New(selBits);
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

    selBits -> clear();

    Q->clear();
    QnoPU->clear();
}

void BareJets::defineBranches(TTree *t){
    TString jetName("jet" + cachedPrefix);

    //
    BareP4::defineBranches(t, jetName.Data());
    //
    t->Branch(jetName + "RawPt","vector<float>",&rawPt);
    //
    t->Branch(jetName + "Bdiscr","vector<float>",&bDiscr);
    //
    t->Branch(jetName + "BdiscrLegacy","vector<float>",&bDiscrLegacy);
    //	
    t->Branch(jetName + "PuId","vector<float>",&puId);
    // -- JES uncertainty
    t->Branch(jetName + "Unc","vector<float>",&unc);
    // --QGL
    t->Branch(jetName + "QGL","vector<float>",&qgl);
    // -- Jet Flavour by PAT
    t->Branch(jetName + "Flavour","vector<int>",&flavour);

    t->Branch(jetName + "MatchedPartonPdgId","vector<int>",&matchedPartonPdgId);
    
    t->Branch(jetName + "MotherPdgId","vector<int>",&motherPdgId);
    
    t->Branch(jetName + "GrMotherPdgId","vector<int>",&grMotherPdgId);
    
    t->Branch(jetName + "SelBits","vector<unsigned>",&selBits);

    t->Branch(jetName + "Q","vector<float>",&Q);

    t->Branch(jetName + "QnoPU","vector<float>",&QnoPU);
}

void BareJets::setBranchAddresses(TTree* t, std::string prefix)
{
    cachedPrefix = prefix;
    TString jetName("jet" + cachedPrefix);

    BareP4::setBranchAddresses(t,jetName.Data());
    BareFunctions::SetBranchAddress(t,jetName + "RawPt"	,&rawPt);
    BareFunctions::SetBranchAddress(t,jetName + "Bdiscr"	,&bDiscr);
    BareFunctions::SetBranchAddress(t,jetName + "BdiscrLegacy"	,&bDiscrLegacy);
    BareFunctions::SetBranchAddress(t,jetName + "PuId"	,&puId);
    BareFunctions::SetBranchAddress(t,jetName + "Unc"	,&unc);
    BareFunctions::SetBranchAddress(t,jetName + "QGL"	,&qgl);
    BareFunctions::SetBranchAddress(t,jetName + "Flavour"	,&flavour);
    BareFunctions::SetBranchAddress(t,jetName + "MatchedPartonPdgId", &matchedPartonPdgId);
    BareFunctions::SetBranchAddress(t,jetName + "MotherPdgId", &motherPdgId);
    BareFunctions::SetBranchAddress(t,jetName + "GrMotherPdgId", &grMotherPdgId);
    BareFunctions::SetBranchAddress(t,jetName + "SelBits", &selBits);

    // ---
    BareFunctions::SetBranchAddress(t,jetName + "Q",&Q);
    BareFunctions::SetBranchAddress(t,jetName + "QnoPU",&QnoPU);

}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
