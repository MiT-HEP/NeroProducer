#include "NeroProducer/Core/interface/BareFatJets.hpp"


BareFatJets::BareFatJets(){
    p4 = NULL;
    rawPt = NULL;
    flavour = NULL;
    tau1 = NULL;
    tau2 = NULL;
    tau3 = NULL;
    trimmedMass = NULL;
    prunedMass = NULL;
    filteredMass = NULL;
    softdropMass = NULL;
    ak8_subjet = NULL;
    ak8jet_hasSubjet = NULL;
    ak8subjet_btag = NULL;

}

BareFatJets::~BareFatJets(){
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
}

void BareFatJets::defineBranches(TTree *t){
    //
    BareP4::defineBranches(t, "fatjet" );
    //
    rawPt = new vector<float>;
    t->Branch("fatjetRawPt","vector<float>",&rawPt);
    // -- Jet Flavour by PAT
    flavour = new vector<int>;
    t->Branch("fatjetFlavour","vector<int>",&flavour);
    //
    tau1 = new vector<float>;
    t->Branch("fatjetTau1","vector<float>",&tau1);
    tau2 = new vector<float>;
    t->Branch("fatjetTau2","vector<float>",&tau2);
    tau3 = new vector<float>;
    t->Branch("fatjetTau3","vector<float>",&tau3);

    //
    trimmedMass = new vector<float>;
    t->Branch("fatjetTrimmedMass","vector<float>",&trimmedMass);
    prunedMass = new vector<float>;
    t->Branch("fatjetPrunedMass","vector<float>",&prunedMass);
    filteredMass = new vector<float>;
    t->Branch("fatjetFilteredMass","vector<float>",&filteredMass);
    softdropMass = new vector<float>;
    t->Branch("fatjetSoftdropMass","vector<float>",&softdropMass);

    ak8_subjet = new TClonesArray("TLorentzVector", 20);
    t->Branch("ak8_subjet","TClonesArray", &ak8_subjet, 128000, 0);
    ak8jet_hasSubjet =  new vector<int>;
    t->Branch("ak8jet_hasSubjet","vector<int>",&ak8jet_hasSubjet);
    ak8subjet_btag =  new vector<float>;
    t->Branch("ak8subjet_btag","vector<float>",&ak8subjet_btag);

}

void BareFatJets::setBranchAddresses(TTree *t){
    //
    BareP4::setBranchAddresses(t,"fatjet");

    rawPt = new vector<float>;
    // -- Jet Flavour by PAT
    flavour = new vector<int>;
    //
    tau1 = new vector<float>;
    tau2 = new vector<float>;
    tau3 = new vector<float>;

    trimmedMass = new vector<float>;
    prunedMass = new vector<float>;
    filteredMass = new vector<float>;
    softdropMass = new vector<float>;

    t->SetBranchAddress("fatjetRawPt"	,&rawPt);
    t->SetBranchAddress("fatjetFlavour" ,&flavour);
    t->SetBranchAddress("fatjetTau1"	,&tau1);
    t->SetBranchAddress("fatjetTau2"	,&tau2);
    t->SetBranchAddress("fatjetTau3"	,&tau3);

    t->SetBranchAddress("fatjetTrimmedMass"	,&trimmedMass);
    t->SetBranchAddress("fatjetPrunedMass"	,&prunedMass);
    t->SetBranchAddress("fatjetFilteredMass"	,&filteredMass);
    t->SetBranchAddress("fatjetSoftdropMass"	,&softdropMass);

    ak8_subjet = new TClonesArray("TLorentzVector", 20);
    t->SetBranchAddress("ak8_subjet"	,&ak8_subjet);
    t->SetBranchAddress("ak8jet_hasSubjet",&ak8jet_hasSubjet);
    t->SetBranchAddress("ak8subjet_btag",&ak8subjet_btag);
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
