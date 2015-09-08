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
}

void BareFatJets::defineBranches(TTree *t){
    // init() called by BareP4
    BareP4::defineBranches(t, "fatjet" );
    //
    t->Branch("fatjetRawPt","vector<float>",&rawPt);
    // -- Jet Flavour by PAT
    t->Branch("fatjetFlavour","vector<int>",&flavour);
    //
    t->Branch("fatjetTau1","vector<float>",&tau1);
    t->Branch("fatjetTau2","vector<float>",&tau2);
    t->Branch("fatjetTau3","vector<float>",&tau3);

    //
    t->Branch("fatjetTrimmedMass","vector<float>",&trimmedMass);
    t->Branch("fatjetPrunedMass","vector<float>",&prunedMass);
    t->Branch("fatjetFilteredMass","vector<float>",&filteredMass);
    t->Branch("fatjetSoftdropMass","vector<float>",&softdropMass);

    t->Branch("ak8_subjet","TClonesArray", &ak8_subjet, 128000, 0);
    t->Branch("ak8jet_hasSubjet","vector<int>",&ak8jet_hasSubjet);
    t->Branch("ak8subjet_btag","vector<float>",&ak8subjet_btag);

    t->Branch("fatjetHbb","vector<float>",&hbb);
}

void BareFatJets::setBranchAddresses(TTree *t){
    // init() called by BareP4
    BareP4::setBranchAddresses(t,"fatjet");

    BareFunctions::SetBranchAddress(t,"fatjetRawPt", &rawPt);

    BareFunctions::SetBranchAddress(t,"fatjetFlavour" ,&flavour);
    BareFunctions::SetBranchAddress(t,"fatjetTau1"	,&tau1);
    BareFunctions::SetBranchAddress(t,"fatjetTau2"	,&tau2);
    BareFunctions::SetBranchAddress(t,"fatjetTau3"	,&tau3);

    BareFunctions::SetBranchAddress(t,"fatjetTrimmedMass"	,&trimmedMass);
    BareFunctions::SetBranchAddress(t,"fatjetPrunedMass"	,&prunedMass);
    BareFunctions::SetBranchAddress(t,"fatjetFilteredMass"	,&filteredMass);
    BareFunctions::SetBranchAddress(t,"fatjetSoftdropMass"	,&softdropMass);

    BareFunctions::SetBranchAddress(t,"ak8_subjet"	,&ak8_subjet);
    BareFunctions::SetBranchAddress(t,"ak8jet_hasSubjet",&ak8jet_hasSubjet);
    BareFunctions::SetBranchAddress(t,"ak8subjet_btag",&ak8subjet_btag);

    BareFunctions::SetBranchAddress(t,"fatjetHbb",&hbb);
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
