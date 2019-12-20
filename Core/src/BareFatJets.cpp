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
    BareFunctions::Delete(softdropMass   );
    BareFunctions::Delete(subjet         );
    BareFunctions::Delete(nSubjets       );
    BareFunctions::Delete(firstSubjet    );
    BareFunctions::Delete(subjet_btag    );
    BareFunctions::Delete(hbb            );
    BareFunctions::Delete(TvsQCD         );
    BareFunctions::Delete(WvsQCD         );
    BareFunctions::Delete(ZvsQCD         );
    BareFunctions::Delete(ZbbvsQCD       );
    BareFunctions::Delete(TvsQCDMD       );
    BareFunctions::Delete(WvsQCDMD       );
    BareFunctions::Delete(ZHbbvsQCDMD    );
    BareFunctions::Delete(ZHccvsQCDMD    );

}

void BareFatJets::init(){
    BareP4::init();


    BareFunctions::New(rawPt);
    BareFunctions::New(flavour);
    BareFunctions::New(tau1);
    BareFunctions::New(tau2);
    BareFunctions::New(tau3);
    BareFunctions::New(softdropMass   );
    BareFunctions::New(subjet         );
    BareFunctions::New(nSubjets       );
    BareFunctions::New(firstSubjet    );
    BareFunctions::New(subjet_btag    );
    BareFunctions::New(hbb            );
    BareFunctions::New(TvsQCD         );
    BareFunctions::New(WvsQCD         );
    BareFunctions::New(ZvsQCD         );
    BareFunctions::New(ZbbvsQCD       );
    BareFunctions::New(TvsQCDMD       );
    BareFunctions::New(WvsQCDMD       );
    BareFunctions::New(ZHbbvsQCDMD    );
    BareFunctions::New(ZHccvsQCDMD    );
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
    softdropMass -> clear();
    subjet->Clear();
    nSubjets->clear();
    firstSubjet->clear();
    subjet_btag ->clear();
    hbb -> clear();
    TvsQCD->clear();
    WvsQCD->clear();
    ZvsQCD->clear();
    ZbbvsQCD->clear();
    TvsQCDMD->clear();
    WvsQCDMD->clear();
    ZHbbvsQCDMD->clear();
    ZHccvsQCDMD->clear();

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

    t->Branch(jetName + "SoftdropMass","vector<float>",&softdropMass);

    t->Branch(jetName + "subjet","TClonesArray", &subjet, 128000, 0);
    t->Branch(jetName + "nSubjets","vector<int>",&nSubjets);
    t->Branch(jetName + "firstSubjet","vector<int>",&firstSubjet);
    t->Branch(jetName + "subjet_btag","vector<float>",&subjet_btag);

    t->Branch(jetName + "Hbb","vector<float>",&hbb);
    t->Branch(jetName + "TvsQCD","vector<float>",&TvsQCD);
    t->Branch(jetName + "WvsQCD","vector<float>",&WvsQCD);
    t->Branch(jetName + "ZvsQCD","vector<float>",&ZvsQCD);
    t->Branch(jetName + "ZbbvsQCD","vector<float>",&ZbbvsQCD);

    t->Branch(jetName + "TvsQCDMD","vector<float>",&TvsQCDMD);
    t->Branch(jetName + "WvsQCDMD","vector<float>",&WvsQCDMD);
    t->Branch(jetName + "ZHbbvsQCDMD","vector<float>",&ZHbbvsQCDMD);
    t->Branch(jetName + "ZHccvsQCDMD","vector<float>",&ZHccvsQCDMD);

    //    t->Branch("puppi_softdrop_masscorr","vector<float>",&puppi_softdrop_masscorr);
    //    t->Branch("puppi_softdrop_mass","vector<float>",&puppi_softdrop_mass);

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

    BareFunctions::SetBranchAddress(t,jetName + "SoftdropMass"  ,&softdropMass);

    BareFunctions::SetBranchAddress(t,jetName + "subjet"  ,&subjet);
    BareFunctions::SetBranchAddress(t,jetName + "nSubjets",&nSubjets);
    BareFunctions::SetBranchAddress(t,jetName + "firstSubjet",&firstSubjet);
    BareFunctions::SetBranchAddress(t,jetName + "subjet_btag",&subjet_btag);

    BareFunctions::SetBranchAddress(t,jetName + "Hbb",&hbb);
    BareFunctions::SetBranchAddress(t,jetName + "TvsQCD",&TvsQCD);
    BareFunctions::SetBranchAddress(t,jetName + "WvsQCD",&WvsQCD);
    BareFunctions::SetBranchAddress(t,jetName + "ZvsQCD",&ZvsQCD);
    BareFunctions::SetBranchAddress(t,jetName + "ZbbvsQCD",&ZbbvsQCD);

    BareFunctions::SetBranchAddress(t,jetName + "TvsQCDMD",&TvsQCDMD);
    BareFunctions::SetBranchAddress(t,jetName + "WvsQCDMD",&WvsQCDMD);
    BareFunctions::SetBranchAddress(t,jetName + "ZHbbvsQCDMD",&ZHbbvsQCDMD);
    BareFunctions::SetBranchAddress(t,jetName + "ZHccvsQCDMD",&ZHccvsQCDMD);

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
