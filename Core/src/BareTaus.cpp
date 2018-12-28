#include "NeroProducer/Core/interface/BareTaus.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareTaus::BareTaus(): BareP4() {
}

BareTaus::~BareTaus(){
    BareFunctions::Delete(selBits);
    BareFunctions::Delete(Q);
    //BareFunctions::Delete(M);
    //BareFunctions::Delete(iso);
    BareFunctions::Delete(chargedIsoPtSum);
    BareFunctions::Delete(neutralIsoPtSum);
    BareFunctions::Delete(isoDeltaBetaCorr);
    //BareFunctions::Delete(isoPileupWeightedRaw);
    BareFunctions::Delete(isoMva);
    BareFunctions::Delete(leadTrackPt);
}

void BareTaus::init(){
    BareP4::init();

    BareFunctions::New(selBits);
    BareFunctions::New(Q);
    //BareFunctions::New(M);
    //BareFunctions::New(iso);
    BareFunctions::New(leadTrackPt);

    BareFunctions::New(chargedIsoPtSum);
    BareFunctions::New(neutralIsoPtSum);
    BareFunctions::New(isoDeltaBetaCorr);
    //BareFunctions::New(isoPileupWeightedRaw);
    BareFunctions::New(isoMva);
}

void BareTaus::clear(){
    BareP4::clear();
    selBits->clear();
    Q->clear();
    //M->clear();
    //iso -> clear();
    leadTrackPt -> clear();
    chargedIsoPtSum->clear();
    neutralIsoPtSum->clear() ;
    isoDeltaBetaCorr->clear();
    //isoPileupWeightedRaw->clear();
    isoMva->clear();
}

void BareTaus::defineBranches(TTree *t){
    BareP4::defineBranches(t, "tau" );
    //
    t->Branch("tauSelBits","vector<unsigned>",&selBits);
    //
    t->Branch("tauQ","vector<int>",&Q);
    //
    //t->Branch("tauM","vector<float>",&M);
    //
    //t->Branch("tauIso","vector<float>",&iso);
    //
    t->Branch("tauLeadTrackPt","vector<float>",&leadTrackPt);

    t->Branch("tauChargedIsoPtSum","vector<float>",&chargedIsoPtSum);
    t->Branch("tauNeutralIsoPtSum","vector<float>",&neutralIsoPtSum);
    t->Branch("tauIsoDeltaBetaCorr","vector<float>",&isoDeltaBetaCorr);
    //t->Branch("tauIsoPileupWeightedRaw","vector<float>",&isoPileupWeightedRaw);
    t->Branch("tauIsoMva","vector<float>",&isoMva);

}

void BareTaus::setBranchAddresses(TTree *t){
    BareP4::setBranchAddresses(t,"tau");

    BareFunctions::SetBranchAddress(t,"tauSelBits"	,&selBits);
    BareFunctions::SetBranchAddress(t,"tauQ"	,&Q);
    //BareFunctions::SetBranchAddress(t,"tauM"	,&M);
    //BareFunctions::SetBranchAddress(t,"tauIso"	,&iso);
    BareFunctions::SetBranchAddress(t,"tauLeadTrackPt"	,&leadTrackPt);

    BareFunctions::SetBranchAddress(t,"tauChargedIsoPtSum",&chargedIsoPtSum);
    BareFunctions::SetBranchAddress(t,"tauNeutralIsoPtSum",&neutralIsoPtSum);
    BareFunctions::SetBranchAddress(t,"tauIsoDeltaBetaCorr",&isoDeltaBetaCorr);
    //BareFunctions::SetBranchAddress(t,"tauIsoPileupWeightedRaw",&isoPileupWeightedRaw);
    BareFunctions::SetBranchAddress(t,"tauIsoMva"	,&isoMva);
}
BAREREGISTER(BareTaus);
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
