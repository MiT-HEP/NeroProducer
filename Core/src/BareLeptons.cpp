#include "NeroProducer/Core/interface/BareLeptons.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareLeptons::BareLeptons():BareP4(){
}

BareLeptons::~BareLeptons(){
    BareFunctions::Delete(pdgId);
    BareFunctions::Delete(iso);
    BareFunctions::Delete(corr);
    BareFunctions::Delete(selBits);
    BareFunctions::Delete(lepPfPt);
    BareFunctions::Delete(mva);
    BareFunctions::Delete(chIso);
    BareFunctions::Delete(nhIso);
    BareFunctions::Delete(phoIso);
    BareFunctions::Delete(puIso);
    BareFunctions::Delete(miniIso);
    BareFunctions::Delete(etaSC);
    BareFunctions::Delete(sieie);
    BareFunctions::Delete(sipip);
    BareFunctions::Delete(sieip);
    BareFunctions::Delete(r9);
    BareFunctions::Delete(resolution);
    BareFunctions::Delete(nLayers);
    BareFunctions::Delete(kinfitP4);
    BareFunctions::Delete(kinfitPtErr);
}

void BareLeptons::init(){
    BareP4::init();

    BareFunctions::New(pdgId);
    BareFunctions::New(iso);
    BareFunctions::New(corr);
    BareFunctions::New(selBits);
    BareFunctions::New(lepPfPt);
    BareFunctions::New(mva);
    BareFunctions::New(chIso);
    BareFunctions::New(nhIso);
    BareFunctions::New(phoIso);
    BareFunctions::New(puIso);
    BareFunctions::New(miniIso);
    BareFunctions::New(etaSC);
    BareFunctions::New(sieie);
    BareFunctions::New(sipip);
    BareFunctions::New(sieip);
    BareFunctions::New(r9);
    BareFunctions::New(resolution);
    BareFunctions::New(nLayers);

    BareFunctions::New(kinfitP4);
    BareFunctions::New(kinfitPtErr);
}

void BareLeptons::clear(){
    BareP4::clear();

    pdgId -> clear();
    iso -> clear();
    corr -> clear();
    selBits ->clear();
    lepPfPt->clear();    

    etaSC->clear();    
    sieie->clear();
    sipip->clear();    
    sieip->clear();    
    r9->clear();    

    mva->clear();
    chIso->clear();
    nhIso->clear();
    phoIso->clear();
    puIso->clear();
    miniIso->clear();
   
    resolution->clear() ;
    nLayers->clear();

    kinfitP4->Clear();
    kinfitPtErr->clear();

}

void BareLeptons::defineBranches(TTree*t){
    //
    BareP4::defineBranches(t, "lep" );
    //
    t->Branch("lepPdgId","vector<int>",&pdgId);
    //
    t->Branch("lepIso","vector<float>",&iso);
    //
    t->Branch("lepCorrections","vector<float>",&corr);
    //
    t->Branch("lepSelBits","vector<unsigned>",&selBits);
    //
    t->Branch("lepPfPt","vector<float>",&lepPfPt);

    // 
    t->Branch("lepMva","vector<float>",&mva);
    t->Branch("lepChIso","vector<float>",&chIso);
    t->Branch("lepNhIso","vector<float>",&nhIso);
    t->Branch("lepPhoIso","vector<float>",&phoIso);
    t->Branch("lepPuIso","vector<float>",&puIso);
    t->Branch("lepMiniIso","vector<float>",&miniIso);

    t->Branch("lepEtaSC","vector<float>",&etaSC);
    t->Branch("lepSieie","vector<float>",&sieie);
    t->Branch("lepSipip","vector<float>",&sipip);
    t->Branch("lepSieip","vector<float>",&sieip);
    t->Branch("lepR9","vector<float>",&r9);
    //
    t->Branch("lepResolution","vector<float>",&resolution);
    t->Branch("lepNLayers","vector<int>",&nLayers);


    //kintfit
    t->Branch("lepKinfitPtErr","vector<float>",&kinfitPtErr);
    t->Branch( "lepKinfitP4","TClonesArray", &kinfitP4, 128000, 0);


}

void BareLeptons::setBranchAddresses(TTree*t){

    BareP4::setBranchAddresses(t,"lep");

    BareFunctions::SetBranchAddress(t,"lepPdgId"	,&pdgId);
    BareFunctions::SetBranchAddress(t,"lepIso"	,&iso);
    BareFunctions::SetBranchAddress(t,"lepCorrections"	,&corr);
    BareFunctions::SetBranchAddress(t,"lepSelBits"	,&selBits);
    BareFunctions::SetBranchAddress(t,"lepPfPt"	,&lepPfPt);

    BareFunctions::SetBranchAddress(t,"lepMva",&mva);
    BareFunctions::SetBranchAddress(t,"lepChIso",&chIso);
    BareFunctions::SetBranchAddress(t,"lepNhIso",&nhIso);
    BareFunctions::SetBranchAddress(t,"lepPhoIso",&phoIso);
    BareFunctions::SetBranchAddress(t,"lepPuIso",&puIso);
    BareFunctions::SetBranchAddress(t,"lepMiniIso",&miniIso);

    BareFunctions::SetBranchAddress(t,"lepEtaSC",&etaSC);
    BareFunctions::SetBranchAddress(t,"lepSieie",&sieie);
    BareFunctions::SetBranchAddress(t,"lepSipip",&sipip);
    BareFunctions::SetBranchAddress(t,"lepSieip",&sieip);
    BareFunctions::SetBranchAddress(t,"lepR9",&r9);

    BareFunctions::SetBranchAddress(t,"lepResolution",&resolution);
    BareFunctions::SetBranchAddress(t,"lepNLayers",&nLayers);

    BareFunctions::SetBranchAddress(t,"lepKinfitPtErr",&kinfitPtErr);
    BareFunctions::SetBranchAddress(t,"lepKinfitP4",&kinfitP4);
}

BAREREGISTER(BareLeptons);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
