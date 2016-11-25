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
    BareFunctions::Delete(corrprunedMass );
    BareFunctions::Delete(softdropMass   );
    BareFunctions::Delete(subjet         );
    BareFunctions::Delete(nSubjets       );
    BareFunctions::Delete(firstSubjet    );
    BareFunctions::Delete(subjet_btag    );
    BareFunctions::Delete(topMVA         );
    BareFunctions::Delete(puppiAK8       );
    BareFunctions::Delete(puppi_tau1     );
    BareFunctions::Delete(puppi_tau2     );
    BareFunctions::Delete(puppi_softdrop_masscorr);

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
    BareFunctions::New(corrprunedMass );
    BareFunctions::New(softdropMass   );
    BareFunctions::New(subjet         );
    BareFunctions::New(nSubjets       );
    BareFunctions::New(firstSubjet    );
    BareFunctions::New(subjet_btag    );
    BareFunctions::New(topMVA         );
    BareFunctions::New(hbb         );
    BareFunctions::New(puppiAK8       );
    BareFunctions::New(puppi_tau1     );
    BareFunctions::New(puppi_tau2     );
    BareFunctions::New(puppi_softdrop_masscorr);

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
    corrprunedMass -> clear();
    softdropMass -> clear();
    subjet->Clear();
    nSubjets->clear();
    firstSubjet->clear();
    subjet_btag ->clear();
    hbb -> clear();
    topMVA->clear();
    puppiAK8->Clear();
    puppi_tau1->clear();
    puppi_tau2->clear();
    puppi_softdrop_masscorr->clear();
}

void BareFatJets::defineBranches(TTree *t){

    TString jetName("fatjet" + cachedPrefix);

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
    t->Branch(jetName + "CorrectedPrunedMass","vector<float>",&corrprunedMass);
    t->Branch(jetName + "SoftdropMass","vector<float>",&softdropMass);

    t->Branch(jetName + "subjet","TClonesArray", &subjet, 128000, 0);
    t->Branch(jetName + "nSubjets","vector<int>",&nSubjets);
    t->Branch(jetName + "firstSubjet","vector<int>",&firstSubjet);
    t->Branch(jetName + "subjet_btag","vector<float>",&subjet_btag);

    t->Branch(jetName + "Hbb","vector<float>",&hbb);

    t->Branch(jetName + "topMVA","vector<float>",&topMVA);

    //puppi
    t->Branch("puppiAK8","TClonesArray", &puppiAK8, 128000, 0);
    t->Branch("puppi_tau1","vector<float>",&puppi_tau1);
    t->Branch("puppi_tau2","vector<float>",&puppi_tau2);
    t->Branch("puppi_softdrop_masscorr","vector<float>",&puppi_softdrop_masscorr);

}

void BareFatJets::setBranchAddresses(TTree *t, std::string prefix){

    cachedPrefix = prefix;
    TString jetName("fatjet" + cachedPrefix);
    // init() called by BareP4
    BareP4::setBranchAddresses(t,jetName.Data());

    BareFunctions::SetBranchAddress(t,jetName + "RawPt", &rawPt);

    BareFunctions::SetBranchAddress(t,jetName + "Flavour" ,&flavour);
    BareFunctions::SetBranchAddress(t,jetName + "Tau1"  ,&tau1);
    BareFunctions::SetBranchAddress(t,jetName + "Tau2"  ,&tau2);
    BareFunctions::SetBranchAddress(t,jetName + "Tau3"  ,&tau3);

    BareFunctions::SetBranchAddress(t,jetName + "TrimmedMass"   ,&trimmedMass);
    BareFunctions::SetBranchAddress(t,jetName + "PrunedMass"    ,&prunedMass);
    BareFunctions::SetBranchAddress(t,jetName + "CorrectedPrunedMass"  ,&corrprunedMass);
    BareFunctions::SetBranchAddress(t,jetName + "SoftdropMass"  ,&softdropMass);

    BareFunctions::SetBranchAddress(t,jetName + "subjet"  ,&subjet);
    BareFunctions::SetBranchAddress(t,jetName + "nSubjets",&nSubjets);
    BareFunctions::SetBranchAddress(t,jetName + "firstSubjet",&firstSubjet);
    BareFunctions::SetBranchAddress(t,jetName + "subjet_btag",&subjet_btag);

    BareFunctions::SetBranchAddress(t,jetName + "Hbb",&hbb);
    BareFunctions::SetBranchAddress(t,jetName + "topMVA",&topMVA);
}
void BareFatJets::compress(){
    BareP4::compress();
	for(int i=0;i<subjet->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) subjet->At(i)  );
}

BAREREGISTER(BareFatJets);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
