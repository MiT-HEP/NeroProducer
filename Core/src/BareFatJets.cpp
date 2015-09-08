#include "NeroProducer/Core/interface/BareFatJets.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"


BareFatJets::BareFatJets(): BareP4(){
}

BareFatJets::~BareFatJets(){
    delete rawPt;
    delete flavour;
    delete tau1;
    delete tau2;
    delete tau3;
    delete trimmedMass;
    delete prunedMass;
    delete filteredMass;
    delete softdropMass;
    delete ak8_subjet;
    delete ak8jet_hasSubjet;
    delete ak8subjet_btag;
}

void BareFatJets::init(){
    BareP4::init();

    if (!rawPt)
        rawPt = new vector<float>;

    if (!flavour)
        flavour = new vector<int>;
    //
    if (!tau1)
        tau1 = new vector<float>;
    if (!tau2)
        tau2 = new vector<float>;
    if (!tau3)
        tau3 = new vector<float>;

    if (!trimmedMass)
        trimmedMass = new vector<float>;
    if (!prunedMass)
        prunedMass = new vector<float>;
    if (!filteredMass)
        filteredMass = new vector<float>;
    if (!softdropMass)
        softdropMass = new vector<float>;

    if (!ak8_subjet)
        ak8_subjet = new TClonesArray("TLorentzVector", 20);
    if (!ak8jet_hasSubjet)
        ak8jet_hasSubjet = new vector<int>;
    if (!ak8subjet_btag)
        ak8subjet_btag = new vector<float>;
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

    if (t->GetBranchStatus("fatjetRawPt"))
        t->SetBranchAddress("fatjetRawPt"	,&rawPt);
    if (t->GetBranchStatus("fatjetFlavour"))
        t->SetBranchAddress("fatjetFlavour" ,&flavour);
    if (t->GetBranchStatus("fatjetTau1"))
        t->SetBranchAddress("fatjetTau1"	,&tau1);
    if (t->GetBranchStatus("fatjetTau2"))
        t->SetBranchAddress("fatjetTau2"	,&tau2);
    if (t->GetBranchStatus("fatjetTau3"))
        t->SetBranchAddress("fatjetTau3"	,&tau3);

    if (t->GetBranchStatus("fatjetTrimmedMass"))
        t->SetBranchAddress("fatjetTrimmedMass"	,&trimmedMass);
    if (t->GetBranchStatus("fatjetPrunedMass"))
        t->SetBranchAddress("fatjetPrunedMass"	,&prunedMass);
    if (t->GetBranchStatus("fatjetFilteredMass"))
        t->SetBranchAddress("fatjetFilteredMass"	,&filteredMass);
    if (t->GetBranchStatus("fatjetSoftdropMass"))
        t->SetBranchAddress("fatjetSoftdropMass"	,&softdropMass);

    if (t->GetBranchStatus("ak8_subjet"))
        t->SetBranchAddress("ak8_subjet"	,&ak8_subjet);
    if (t->GetBranchStatus("ak8jet_hasSubjet"))
        t->SetBranchAddress("ak8jet_hasSubjet",&ak8jet_hasSubjet);
    if (t->GetBranchStatus("ak8subjet_btag"))
        t->SetBranchAddress("ak8subjet_btag",&ak8subjet_btag);

    if (t->GetBranchStatus("fatjetHbb"))
        t->SetBranchAddress("fatjetHbb",&hbb);
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
