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
    //extra qg variables
    for(const auto& dR : dRToProduce)
    {
        BareFunctions::Delete(qglMult_dR[Form("0p%.0f",dR*1000)]);
        BareFunctions::Delete(qglPtD_dR[Form("0p%.0f",dR*1000)]);
        BareFunctions::Delete(qglAxis2_dR[Form("0p%.0f",dR*1000)]);
        BareFunctions::Delete(qglAxis1_dR[Form("0p%.0f",dR*1000)]);
        BareFunctions::Delete(qglGenMult_dR[Form("0p%.0f",dR*1000)]);
        BareFunctions::Delete(qglGenPtD_dR[Form("0p%.0f",dR*1000)]);
        BareFunctions::Delete(qglGenAxis2_dR[Form("0p%.0f",dR*1000)]);
        BareFunctions::Delete(qglGenAxis1_dR[Form("0p%.0f",dR*1000)]);
    }
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

    for(const auto& dR : dRToProduce)
    {
        BareFunctions::New(qglMult_dR[Form("0p%03.0f",dR*1000)]);
        BareFunctions::New(qglPtD_dR[Form("0p%03.0f",dR*1000)]);
        BareFunctions::New(qglAxis2_dR[Form("0p%03.0f",dR*1000)]);
        BareFunctions::New(qglAxis1_dR[Form("0p%03.0f",dR*1000)]);
        BareFunctions::New(qglGenMult_dR[Form("0p%03.0f",dR*1000)]);
        BareFunctions::New(qglGenPtD_dR[Form("0p%03.0f",dR*1000)]);
        BareFunctions::New(qglGenAxis2_dR[Form("0p%03.0f",dR*1000)]);
        BareFunctions::New(qglGenAxis1_dR[Form("0p%03.0f",dR*1000)]);
    }

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
    for(const auto& dR : dRToProduce)
    {
        qglMult_dR[Form("0p%03.0f",dR*1000)]	    ->clear();
        qglPtD_dR[Form("0p%03.0f",dR*1000)]	    ->clear();
        qglAxis2_dR[Form("0p%03.0f",dR*1000)]	    ->clear();
        qglAxis1_dR[Form("0p%03.0f",dR*1000)]	    ->clear();
        qglGenMult_dR[Form("0p%03.0f",dR*1000)]	->clear();
        qglGenPtD_dR[Form("0p%03.0f",dR*1000)]	->clear();
        qglGenAxis2_dR[Form("0p%03.0f",dR*1000)]	->clear();
        qglGenAxis1_dR[Form("0p%03.0f",dR*1000)]	->clear();
    }
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


    //
    t->Branch(jetName + "PtResUncCentral","vector<float>",&ptResUncCentral);
    t->Branch(jetName + "PtResUncUp","vector<float>",&ptResUncUp);
    t->Branch(jetName + "PtResUncDown","vector<float>",&ptResUncDown);

    for(const auto& dR : dRToProduce)
    {
        t->Branch(jetName+Form("QglMult_dR0p%03.0f",dR*1000),"vector<float>",&qglMult_dR[Form("0p%03.0f",dR*1000)]	    );
        t->Branch(jetName+Form("QglPtD_dR0p%03.0f",dR*1000),"vector<float>",&qglPtD_dR[Form("0p%03.0f",dR*1000)]	    );
        t->Branch(jetName+Form("QglAxis2_dR0p%03.0f",dR*1000),"vector<float>",&qglAxis2_dR[Form("0p%03.0f",dR*1000)]	);
        t->Branch(jetName+Form("QglAxis1_dR0p%03.0f",dR*1000),"vector<float>",&qglAxis1_dR[Form("0p%03.0f",dR*1000)]	);
        t->Branch(jetName+Form("QglGenMult_dR0p%03.0f",dR*1000),"vector<float>",&qglGenMult_dR[Form("0p%03.0f",dR*1000)]	);
        t->Branch(jetName+Form("QglGenPtD_dR0p%03.0f",dR*1000),"vector<float>",&qglGenPtD_dR[Form("0p%03.0f",dR*1000)]	);
        t->Branch(jetName+Form("QglGenAxis2_dR0p%03.0f",dR*1000),"vector<float>",&qglGenAxis2_dR[Form("0p%03.0f",dR*1000)]	);
        t->Branch(jetName+Form("QglGenAxis1_dR0p%03.0f",dR*1000),"vector<float>",&qglGenAxis1_dR[Form("0p%03.0f",dR*1000)]	);
    }
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
    BareFunctions::SetBranchAddress(t,jetName + "PtResUncCentral"	,&ptResUncCentral);
    BareFunctions::SetBranchAddress(t,jetName + "PtResUncUp"	,&ptResUncUp);
    BareFunctions::SetBranchAddress(t,jetName + "PtResUncDown"	,&ptResUncDown);

    for(const auto& dR : dRToProduce)
    {
        BareFunctions::SetBranchAddress(t,jetName+Form("QglMult_dR0p%03.0f",dR*1000) ,&qglMult_dR[Form("0p%03.0f",dR*1000)]	    );
        BareFunctions::SetBranchAddress(t,jetName+Form("QglPtD_dR0p%03.0f",dR*1000)  ,&qglPtD_dR[Form("0p%03.0f",dR*1000)]	    );
        BareFunctions::SetBranchAddress(t,jetName+Form("QglAxis2_dR0p%03.0f",dR*1000),&qglAxis2_dR[Form("0p%03.0f",dR*1000)]	);
        BareFunctions::SetBranchAddress(t,jetName+Form("QglAxis1_dR0p%03.0f",dR*1000),&qglAxis1_dR[Form("0p%03.0f",dR*1000)]	);
        BareFunctions::SetBranchAddress(t,jetName+Form("QglGenMult_dR0p%03.0f",dR*1000),&qglGenMult_dR[Form("0p%03.0f",dR*1000)]	);
        BareFunctions::SetBranchAddress(t,jetName+Form("QglGenPtD_dR0p%03.0f",dR*1000),&qglGenPtD_dR[Form("0p%03.0f",dR*1000)]	);
        BareFunctions::SetBranchAddress(t,jetName+Form("QglGenAxis2_dR0p%03.0f",dR*1000),&qglGenAxis2_dR[Form("0p%03.0f",dR*1000)]	);
        BareFunctions::SetBranchAddress(t,jetName+Form("QglGenAxis1_dR0p%03.0f",dR*1000),&qglGenAxis1_dR[Form("0p%03.0f",dR*1000)]	);
    }
}

BAREREGISTER(BareJets);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
