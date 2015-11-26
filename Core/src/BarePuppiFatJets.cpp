#include "NeroProducer/Core/interface/BarePuppiFatJets.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BarePuppiFatJets::BarePuppiFatJets() : BareP4() {}

BarePuppiFatJets::~BarePuppiFatJets(){
    BareFunctions::Delete(rawPt);
    BareFunctions::Delete(flavour        );
    BareFunctions::Delete(tau1           );
    BareFunctions::Delete(tau2           );
    BareFunctions::Delete(tau3           );
    BareFunctions::Delete(trimmedMass    );
    BareFunctions::Delete(prunedMass     );
    BareFunctions::Delete(filteredMass   );
    BareFunctions::Delete(softdropMass   );
    BareFunctions::Delete(subjet         );
    BareFunctions::Delete(hasSubjet      );
    BareFunctions::Delete(subjetBtag     );
    BareFunctions::Delete(topMVA         );
}

void BarePuppiFatJets::init(){
    BareP4::init();
    BareFunctions::New(rawPt);
    BareFunctions::New(flavour);
    BareFunctions::New(tau1);
    BareFunctions::New(tau2);
    BareFunctions::New(tau3);
    BareFunctions::New(trimmedMass    );
    BareFunctions::New(prunedMass     );
    BareFunctions::New(filteredMass   );
    BareFunctions::New(softdropMass   );
    BareFunctions::New(subjet         );
    BareFunctions::New(hasSubjet      );
    BareFunctions::New(subjetBtag     );
    BareFunctions::New(topMVA         );
}

void BarePuppiFatJets::clear(){
    BareP4::clear();
    p4 -> Clear();
    rawPt -> clear();
    flavour -> clear();
    tau1 -> clear();
    tau2 -> clear();
    tau3 -> clear();
    trimmedMass -> clear();
    prunedMass -> clear();
    filteredMass -> clear();
    softdropMass -> clear();
    subjet->Clear();
    subjetBtag ->clear();
    hasSubjet->clear();
    hbb -> clear();
    topMVA->clear();
}

void BarePuppiFatJets::defineBranches(TTree *t, std::string prefix){

    TString jetName("fatjetpuppi" + prefix);
    TString algo(prefix+"puppi");
    if (prefix == "")
        algo = "ak8puppi";

    // init() called by BareP4
    BareP4::defineBranches(t, jetName.Data());
    //
    t->Branch(jetName + "RawPt","vector<float>",&rawPt);
    t->Branch(jetName + "Flavour","vector<int>",&flavour);
    //
    t->Branch(jetName + "Tau1","vector<float>",&tau1);
    t->Branch(jetName + "Tau2","vector<float>",&tau2);
    t->Branch(jetName + "Tau3","vector<float>",&tau3);
    //
    t->Branch(jetName + "TrimmedMass","vector<float>",&trimmedMass);
    t->Branch(jetName + "PrunedMass","vector<float>",&prunedMass);
    t->Branch(jetName + "FilteredMass","vector<float>",&filteredMass);
    t->Branch(jetName + "SoftdropMass","vector<float>",&softdropMass);

    t->Branch(algo + "_subjet","TClonesArray", &subjet, 128000, 0);
    t->Branch(algo + "jet_hasSubjet","vector<int>",&hasSubjet);
    t->Branch(algo + "subjet_btag","vector<float>",&subjetBtag);

    t->Branch(jetName + "Hbb","vector<float>",&hbb);

    t->Branch(jetName + "topMVA","vector<float>",&topMVA);
}

void BarePuppiFatJets::setBranchAddresses(TTree* t, std::string prefix)
{
    TString jetName("fatjetpuppi" + prefix);
    TString algo(prefix);
    if (prefix == "")
        algo = "ak8";

    // init() called by BareP4
    BareP4::setBranchAddresses(t,jetName.Data());

    BareFunctions::SetBranchAddress(t,jetName + "RawPt", &rawPt);

    BareFunctions::SetBranchAddress(t,jetName + "Flavour" ,&flavour);
    BareFunctions::SetBranchAddress(t,jetName + "Tau1"  ,&tau1);
    BareFunctions::SetBranchAddress(t,jetName + "Tau2"  ,&tau2);
    BareFunctions::SetBranchAddress(t,jetName + "Tau3"  ,&tau3);

    BareFunctions::SetBranchAddress(t,jetName + "TrimmedMass"   ,&trimmedMass);
    BareFunctions::SetBranchAddress(t,jetName + "PrunedMass"    ,&prunedMass);
    BareFunctions::SetBranchAddress(t,jetName + "FilteredMass"  ,&filteredMass);
    BareFunctions::SetBranchAddress(t,jetName + "SoftdropMass"  ,&softdropMass);

    BareFunctions::SetBranchAddress(t,algo + "_subjet"  ,&subjet);
    BareFunctions::SetBranchAddress(t,algo + "jet_hasSubjet",&hasSubjet);
    BareFunctions::SetBranchAddress(t,algo + "subjet_btag",&subjetBtag);

    BareFunctions::SetBranchAddress(t,jetName + "Hbb",&hbb);
    BareFunctions::SetBranchAddress(t,jetName + "topMVA",&topMVA);
}
void BarePuppiFatJets::compress(){
    BareP4::compress();
    for(int i=0;i<subjet->GetEntries();++i)
        BareFunctions::Compress( * (TLorentzVector*) subjet->At(i)  );
}
