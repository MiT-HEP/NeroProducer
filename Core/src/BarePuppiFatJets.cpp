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
    BareFunctions::Delete(nSubjets       );
    BareFunctions::Delete(firstSubjet    );
    BareFunctions::Delete(subjet_btag    );
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
    BareFunctions::New(nSubjets       );
    BareFunctions::New(firstSubjet    );
    BareFunctions::New(subjet_btag    );
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
    nSubjets->clear();
    firstSubjet->clear();
    subjet_btag ->clear();
    hbb -> clear();
    topMVA->clear();
}

void BarePuppiFatJets::defineBranches(TTree *t){

    TString jetName("fatjet" + cachedPrefix);

    // init() called by BareP4
    BareP4::defineBranches(t, jetName.Data());
    //
    t->Branch(jetName + "_RawPt","vector<float>",&rawPt);
    t->Branch(jetName + "_Flavour","vector<int>",&flavour);
    //
    t->Branch(jetName + "_Tau1","vector<float>",&tau1);
    t->Branch(jetName + "_Tau2","vector<float>",&tau2);
    t->Branch(jetName + "_Tau3","vector<float>",&tau3);
    //
    t->Branch(jetName + "_TrimmedMass","vector<float>",&trimmedMass);
    t->Branch(jetName + "_PrunedMass","vector<float>",&prunedMass);
    t->Branch(jetName + "_FilteredMass","vector<float>",&filteredMass);
    t->Branch(jetName + "_SoftdropMass","vector<float>",&softdropMass);

    t->Branch(jetName + "_subjet","TClonesArray", &subjet, 128000, 0);
    t->Branch(jetName + "_nSubjets","vector<int>",&nSubjets);
    t->Branch(jetName + "_firstSubjet","vector<int>",&firstSubjet);
    t->Branch(jetName + "_subjet_btag","vector<float>",&subjet_btag);

    t->Branch(jetName + "_Hbb","vector<float>",&hbb);

    t->Branch(jetName + "_topMVA","vector<float>",&topMVA);
}

void BarePuppiFatJets::setBranchAddresses(TTree* t, std::string prefix)
{
    cachedPrefix = prefix;

    TString jetName("fatjet" + cachedPrefix);
    // init() called by BareP4
    BareP4::setBranchAddresses(t,jetName.Data());

    BareFunctions::SetBranchAddress(t,jetName + "_RawPt", &rawPt);

    BareFunctions::SetBranchAddress(t,jetName + "_Flavour" ,&flavour);
    BareFunctions::SetBranchAddress(t,jetName + "_Tau1"  ,&tau1);
    BareFunctions::SetBranchAddress(t,jetName + "_Tau2"  ,&tau2);
    BareFunctions::SetBranchAddress(t,jetName + "_Tau3"  ,&tau3);

    BareFunctions::SetBranchAddress(t,jetName + "_TrimmedMass"   ,&trimmedMass);
    BareFunctions::SetBranchAddress(t,jetName + "_PrunedMass"    ,&prunedMass);
    BareFunctions::SetBranchAddress(t,jetName + "_FilteredMass"  ,&filteredMass);
    BareFunctions::SetBranchAddress(t,jetName + "_SoftdropMass"  ,&softdropMass);

    BareFunctions::SetBranchAddress(t,jetName + "_subjet"  ,&subjet);
    BareFunctions::SetBranchAddress(t,jetName + "_nSubjets",&nSubjets);
    BareFunctions::SetBranchAddress(t,jetName + "_firstSubjet",&firstSubjet);
    BareFunctions::SetBranchAddress(t,jetName + "_subjet_btag",&subjet_btag);

    BareFunctions::SetBranchAddress(t,jetName + "_Hbb",&hbb);
    BareFunctions::SetBranchAddress(t,jetName + "_topMVA",&topMVA);
}
void BarePuppiFatJets::compress(){
    BareP4::compress();
    for(int i=0;i<subjet->GetEntries();++i)
        BareFunctions::Compress( * (TLorentzVector*) subjet->At(i)  );
}
