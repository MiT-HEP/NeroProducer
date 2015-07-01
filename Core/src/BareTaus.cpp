#include "NeroProducer/Core/interface/BareTaus.hpp"

BareTaus::BareTaus(): BareCollection() {
    p4 = NULL;
    id = NULL;
    Q = NULL;
    M = NULL;
    iso = NULL;
    chargedIsoPtSum = NULL;
    neutralIsoPtSum = NULL;
    isoDeltaBetaCorr = NULL;
    againstEleLoose = NULL;
    againstEleMedium = NULL;
    againstMuLoose = NULL;
    againstMuTight = NULL;
}

BareTaus::~BareTaus(){
}

void BareTaus::clear(){
    BareP4::clear();
    id->clear();
    Q->clear();
    M->clear();
    iso -> clear();
    if ( extend_ ) 
        {
        chargedIsoPtSum->clear();
        neutralIsoPtSum->clear() ;
        isoDeltaBetaCorr->clear();
        againstEleLoose->clear();
        againstEleMedium->clear();
        againstMuLoose->clear();
        againstMuTight->clear()  ;
        }
}

void BareTaus::defineBranches(TTree *t){
    BareP4::defineBranches(t, "tau" );
    //
    id = new vector<float>;
    t->Branch("tauId","vector<float>",&id);
    //
    Q = new vector<int>;
    t->Branch("tauQ","vector<int>",&Q);
    //
    M = new vector<float>;
    t->Branch("tauM","vector<float>",&M);
    //
    iso = new vector<float>;
    t->Branch("tauIso","vector<float>",&iso);

    if ( IsExtend() )
        {
        chargedIsoPtSum = new vector<float>;
        t->Branch("tauChargedIsoPtSum","vector<float>",&chargedIsoPtSum);
        neutralIsoPtSum = new vector<float> ;
        t->Branch("tauNeutralIsoPtSum","vector<float>",&neutralIsoPtSum);
        isoDeltaBetaCorr = new vector<float>;
        t->Branch("tauIsoDeltaBetaCorr","vector<float>",&isoDeltaBetaCorr);
        againstEleLoose = new vector<int>;
        t->Branch("tauAgainstEleLoose","vector<int>",&againstEleLoose);
        againstEleMedium = new vector<int>;
        t->Branch("tauAgainstEleMedium","vector<int>",&againstEleMedium);
        againstMuLoose = new vector<int>;
        t->Branch("tauAgainstMuLoose","vector<int>",&againstMuLoose);
        againstMuTight = new vector<int>  ;
        t->Branch("tauAgainstMuTight","vector<int>",&againstMuTight);
        }

}

void BareTaus::setBranchAddresses(TTree *t){
    BareP4::setBranchAddresses(t,"tau");

    id = new vector<float>;
    t->SetBranchAddress("tauId"	,&id);
    //
    Q = new vector<int>;
    t->SetBranchAddress("tauQ"	,&Q);
    //
    M = new vector<float>;
    t->SetBranchAddress("tauM"	,&M);
    //
    iso = new vector<float>;
    t->SetBranchAddress("tauIso"	,&iso);

    // EXTENDED VARIBALES
    if ( IsExtend() )
        {
        chargedIsoPtSum = new vector<float>;
        t->SetBranchAddress("tauChargedIsoPtSum",&chargedIsoPtSum);

        neutralIsoPtSum = new vector<float> ;
        t->SetBranchAddress("tauNeutralIsoPtSum",&neutralIsoPtSum);

        isoDeltaBetaCorr = new vector<float>;
        t->SetBranchAddress("tauIsoDeltaBetaCorr",&isoDeltaBetaCorr);

        againstEleLoose = new vector<int>;
        t->SetBranchAddress("tauAgainstEleLoose",&againstEleLoose);

        againstEleMedium = new vector<int>;
        t->SetBranchAddress("tauAgainstEleMedium",&againstEleMedium);

        againstMuLoose = new vector<int>;
        t->SetBranchAddress("tauAgainstMuLoose",&againstMuLoose);

        againstMuTight = new vector<int>  ;
        t->SetBranchAddress("tauAgainstMuTight",&againstMuTight);
        }
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
