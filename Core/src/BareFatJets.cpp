#include "NeroProducer/Core/interface/BareFatJets.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BareFatJets::BareFatJets(): BareP4(){
}

BareFatJets::~BareFatJets(){

    BareFunctions::Delete(rawPt);
    BareFunctions::Delete(flavour        );
    BareFunctions::Delete(tau1           );
    BareFunctions::Delete(tau2           );
    BareFunctions::Delete(tau3           );
    BareFunctions::Delete(trimmedMass    );
    BareFunctions::Delete(prunedMass     );
    BareFunctions::Delete(filteredMass   );
    BareFunctions::Delete(softdropMass   );
    BareFunctions::Delete(ak8_subjet     );
    BareFunctions::Delete(ak8jet_hasSubjet);
    BareFunctions::Delete(ak8subjet_btag );
    BareFunctions::Delete(topMVA         );
}

void BareFatJets::init(){
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
    BareFunctions::New(ak8_subjet     );
    BareFunctions::New(ak8jet_hasSubjet);
    BareFunctions::New(ak8subjet_btag );
    BareFunctions::New(topMVA         );
}

void BareFatJets::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
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
    ak8_subjet->Clear();
    ak8subjet_btag ->clear();
    ak8jet_hasSubjet->clear();
    hbb -> clear();
    topMVA->clear();
}

void BareFatJets::defineBranches(TTree *t, std::string prefix){
    TString jetName("fatjet" + prefix);
    TString algo(prefix);
    if (prefix == "")
        algo = "ak8";

    // init() called by BareP4
    BareP4::defineBranches(t, jetName.Data());
    //
    t->Branch(jetName + "RawPt","vector<float>",&rawPt);
    // -- Jet Flavour by PAT
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

    t->Branch(algo + "_subjet","TClonesArray", &ak8_subjet, 128000, 0);
    t->Branch(algo + "jet_hasSubjet","vector<int>",&ak8jet_hasSubjet);
    t->Branch(algo + "subjet_btag","vector<float>",&ak8subjet_btag);

    t->Branch(jetName + "Hbb","vector<float>",&hbb);

    t->Branch(jetName + "topMVA","vector<float>",&topMVA);
}

void BareFatJets::setBranchAddresses(TTree *t, std::string prefix){
    TString jetName("fatjet" + prefix);
    TString algo(prefix);
    if (prefix == "")
        algo = "ak8";

    // init() called by BareP4
    BareP4::setBranchAddresses(t,jetName.Data());

    BareFunctions::SetBranchAddress(t,jetName + "RawPt", &rawPt);

    BareFunctions::SetBranchAddress(t,jetName + "Flavour" ,&flavour);
    BareFunctions::SetBranchAddress(t,jetName + "Tau1"	,&tau1);
    BareFunctions::SetBranchAddress(t,jetName + "Tau2"	,&tau2);
    BareFunctions::SetBranchAddress(t,jetName + "Tau3"	,&tau3);

    BareFunctions::SetBranchAddress(t,jetName + "TrimmedMass"	,&trimmedMass);
    BareFunctions::SetBranchAddress(t,jetName + "PrunedMass"	,&prunedMass);
    BareFunctions::SetBranchAddress(t,jetName + "FilteredMass"	,&filteredMass);
    BareFunctions::SetBranchAddress(t,jetName + "SoftdropMass"	,&softdropMass);

    BareFunctions::SetBranchAddress(t,algo + "_subjet"	,&ak8_subjet);
    BareFunctions::SetBranchAddress(t,algo + "jet_hasSubjet",&ak8jet_hasSubjet);
    BareFunctions::SetBranchAddress(t,algo + "subjet_btag",&ak8subjet_btag);

    BareFunctions::SetBranchAddress(t,jetName + "Hbb",&hbb);
    BareFunctions::SetBranchAddress(t,jetName + "topMVA",&topMVA);
}
void BareFatJets::compress(){
    BareP4::compress();
	for(int i=0;i<ak8_subjet->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) ak8_subjet->At(i)  );
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
