#include "NeroProducer/Core/interface/BareJets.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BareJets::BareJets(): BareP4(){
}

BareJets::~BareJets(){
    BareFunctions::Delete(rawPt);
    BareFunctions::Delete(bDiscr);
    BareFunctions::Delete(bMva);
    //BareFunctions::Delete(bDiscrLegacy);
    BareFunctions::Delete(puId);
    BareFunctions::Delete(unc);
    BareFunctions::Delete(qgl);
    BareFunctions::Delete(chef);
    BareFunctions::Delete(nhef);
    BareFunctions::Delete(nemf);
    BareFunctions::Delete(cemf);
    BareFunctions::Delete(flavour);
    BareFunctions::Delete(matchedPartonPdgId);
    BareFunctions::Delete(motherPdgId);
    BareFunctions::Delete(grMotherPdgId);
    BareFunctions::Delete(selBits);
    BareFunctions::Delete(Q);
    // QGL variables
    BareFunctions::Delete(qglMult);
    BareFunctions::Delete(qglPtD);
    BareFunctions::Delete(qglAxis2);
    BareFunctions::Delete(qglAxis1);
    BareFunctions::Delete(qglCMult);
    BareFunctions::Delete(qglNMult);
    BareFunctions::Delete(qglPtDrLog);
    //JER
    BareFunctions::Delete(refPt);
    BareFunctions::Delete(ptResUncCentral);
    BareFunctions::Delete(ptResUncUp);
    BareFunctions::Delete(ptResUncDown);
}

void BareJets::init(){
    BareP4::init();

    BareFunctions::New(rawPt);
    BareFunctions::New(refPt);
    BareFunctions::New(bDiscr);
    BareFunctions::New(bMva);
    //BareFunctions::New(bDiscrLegacy);
    BareFunctions::New(puId);
    BareFunctions::New(unc);
    BareFunctions::New(qgl);
    BareFunctions::New(chef);
    BareFunctions::New(nhef);
    BareFunctions::New(nemf);
    BareFunctions::New(cemf);
    BareFunctions::New(flavour);
    BareFunctions::New(matchedPartonPdgId);
    BareFunctions::New(motherPdgId);
    BareFunctions::New(grMotherPdgId);
    BareFunctions::New(selBits);
    BareFunctions::New(Q);
    // QGL vars
    BareFunctions::New(qglMult);
    BareFunctions::New(qglPtD);
    BareFunctions::New(qglAxis2);
    BareFunctions::New(qglAxis1);
    BareFunctions::New(qglCMult);
    BareFunctions::New(qglNMult);
    BareFunctions::New(qglPtDrLog);
    //
    BareFunctions::New(ptResUncCentral);
    BareFunctions::New(ptResUncUp);
    BareFunctions::New(ptResUncDown);

}

void BareJets::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
    rawPt -> clear();
    bDiscr -> clear();
    bMva -> clear();
    //bDiscrLegacy -> clear();
    puId -> clear();
    unc -> clear();
    qgl -> clear();
    chef -> clear();
    nhef -> clear();
    nemf -> clear();
    cemf -> clear();
    // gen matching
    flavour -> clear();
    matchedPartonPdgId -> clear();
    motherPdgId -> clear();
    grMotherPdgId -> clear();

    selBits -> clear();

    Q->clear();
    
    // QGL vars
    qglMult->clear();
    qglPtD->clear();
    qglAxis2->clear();
    qglAxis1->clear();
    qglCMult->clear();
    qglNMult->clear();
    qglPtDrLog->clear();

    //
    refPt->clear();
    ptResUncCentral->clear();
    ptResUncUp->clear();
    ptResUncDown->clear();
}

void BareJets::defineBranches(TTree *t){
    TString jetName("jet" + cachedPrefix);

    //
    BareP4::defineBranches(t, jetName.Data());
    //
    t->Branch(jetName + "RawPt","vector<float>",&rawPt);
    t->Branch(jetName + "RefPt","vector<float>",&refPt);
    //
    t->Branch(jetName + "Bdiscr","vector<float>",&bDiscr);
    t->Branch(jetName + "BMva","vector<float>",&bMva);
    //
    //t->Branch(jetName + "BdiscrLegacy","vector<float>",&bDiscrLegacy);
    //	
    t->Branch(jetName + "PuId","vector<float>",&puId);
    // -- JES uncertainty
    t->Branch(jetName + "Unc","vector<float>",&unc);
    // -- energy fractions
    t->Branch(jetName + "chef","vector<float>",&chef);
    t->Branch(jetName + "nhef","vector<float>",&nhef);
    t->Branch(jetName + "nemf","vector<float>",&nemf);
    t->Branch(jetName + "cemf","vector<float>",&cemf);
    // --QGL
    t->Branch(jetName + "QGL","vector<float>",&qgl);
    t->Branch(jetName + "QglMult","vector<int>",&qglMult);
    t->Branch(jetName + "QglPtD","vector<float>",&qglPtD);
    t->Branch(jetName + "QglAxis2","vector<float>",&qglAxis2);
    //--
    t->Branch(jetName + "QglAxis1","vector<float>",&qglAxis1);
    t->Branch(jetName + "QglCMult","vector<int>",&qglCMult);
    t->Branch(jetName + "QglNMult","vector<int>",&qglNMult);
    t->Branch(jetName + "QglPtDrLog","vector<float>",&qglPtDrLog);
    // -- Jet Flavour by PAT
    t->Branch(jetName + "Flavour","vector<int>",&flavour);

    t->Branch(jetName + "MatchedPartonPdgId","vector<int>",&matchedPartonPdgId);
    
    t->Branch(jetName + "MotherPdgId","vector<int>",&motherPdgId);
    
    t->Branch(jetName + "GrMotherPdgId","vector<int>",&grMotherPdgId);
    
    t->Branch(jetName + "SelBits","vector<unsigned>",&selBits);

    t->Branch(jetName + "Q","vector<float>",&Q);

    //
    t->Branch(jetName + "PtResUncCentral","vector<float>",&ptResUncCentral);
    t->Branch(jetName + "PtResUncUp","vector<float>",&ptResUncUp);
    t->Branch(jetName + "PtResUncDown","vector<float>",&ptResUncDown);
}

void BareJets::setBranchAddresses(TTree* t, std::string prefix)
{
    cachedPrefix = prefix;
    TString jetName("jet" + cachedPrefix);

    BareP4::setBranchAddresses(t,jetName.Data());
    BareFunctions::SetBranchAddress(t,jetName + "RawPt"	,&rawPt);
    BareFunctions::SetBranchAddress(t,jetName + "RefPt"	,&refPt);
    BareFunctions::SetBranchAddress(t,jetName + "Bdiscr"	,&bDiscr);
    BareFunctions::SetBranchAddress(t,jetName + "BMva"	,&bMva);
    //BareFunctions::SetBranchAddress(t,jetName + "BdiscrLegacy"	,&bDiscrLegacy);
    BareFunctions::SetBranchAddress(t,jetName + "PuId"	,&puId);
    BareFunctions::SetBranchAddress(t,jetName + "Unc"	,&unc);

    BareFunctions::SetBranchAddress(t,jetName + "chef"	,&chef);
    BareFunctions::SetBranchAddress(t,jetName + "nhef"	,&nhef);
    BareFunctions::SetBranchAddress(t,jetName + "nemf"	,&nemf);
    BareFunctions::SetBranchAddress(t,jetName + "cemf"	,&cemf);

    BareFunctions::SetBranchAddress(t,jetName + "QGL"	,&qgl);

    BareFunctions::SetBranchAddress(t,jetName + "QglMult"	,&qglMult);
    BareFunctions::SetBranchAddress(t,jetName + "QglCMult"	,&qglCMult);
    BareFunctions::SetBranchAddress(t,jetName + "QglNMult"	,&qglNMult);
    BareFunctions::SetBranchAddress(t,jetName + "QglPtD"	,&qglPtD);
    BareFunctions::SetBranchAddress(t,jetName + "QglPtDrLog"	,&qglPtDrLog);
    BareFunctions::SetBranchAddress(t,jetName + "QglAxis2"	,&qglAxis2);
    BareFunctions::SetBranchAddress(t,jetName + "QglAxis1"	,&qglAxis1);
    BareFunctions::SetBranchAddress(t,jetName + "Flavour"	,&flavour);
    BareFunctions::SetBranchAddress(t,jetName + "MatchedPartonPdgId", &matchedPartonPdgId);
    BareFunctions::SetBranchAddress(t,jetName + "MotherPdgId", &motherPdgId);
    BareFunctions::SetBranchAddress(t,jetName + "GrMotherPdgId", &grMotherPdgId);
    BareFunctions::SetBranchAddress(t,jetName + "SelBits", &selBits);

    // ---
    BareFunctions::SetBranchAddress(t,jetName + "Q",&Q);
    // ---
    BareFunctions::SetBranchAddress(t,jetName + "PtResUncCentral"	,&ptResUncCentral);
    BareFunctions::SetBranchAddress(t,jetName + "PtResUncUp"	,&ptResUncUp);
    BareFunctions::SetBranchAddress(t,jetName + "PtResUncDown"	,&ptResUncDown);

}

BAREREGISTER(BareJets);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
