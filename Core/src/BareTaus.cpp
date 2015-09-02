#include "NeroProducer/Core/interface/BareTaus.hpp"

BareTaus::BareTaus(): BareP4() {
}

BareTaus::~BareTaus(){
    delete id;
    delete Q;
    delete M;
    delete iso;
    delete chargedIsoPtSum;
    delete neutralIsoPtSum;
    delete isoDeltaBetaCorr;
    delete againstEleLoose;
    delete againstEleMedium;
    delete againstMuLoose;
    delete againstMuTight;
}

void BareTaus::init(){
    BareP4::init();

    if (!id)
        id = new vector<float>;
    //
    if (!Q)
        Q = new vector<int>;
    //
    if (!M)
        M = new vector<float>;
    //
    if (!iso)
        iso = new vector<float>;

    if ( IsExtend() )
        {
            if (!chargedIsoPtSum)
                chargedIsoPtSum = new vector<float>;
            if (!neutralIsoPtSum)
                neutralIsoPtSum = new vector<float> ;
            if (!isoDeltaBetaCorr)
                isoDeltaBetaCorr = new vector<float>;
            if (!againstEleLoose)
                againstEleLoose = new vector<int>;
            if (!againstEleMedium)
                againstEleMedium = new vector<int>;
            if (!againstMuLoose)
                againstMuLoose = new vector<int>;
            if (!againstMuTight)
                againstMuTight = new vector<int>;
        }
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
    t->Branch("tauId","vector<float>",&id);
    //
    t->Branch("tauQ","vector<int>",&Q);
    //
    t->Branch("tauM","vector<float>",&M);
    //
    t->Branch("tauIso","vector<float>",&iso);

    if ( IsExtend() )
        {
        t->Branch("tauChargedIsoPtSum","vector<float>",&chargedIsoPtSum);
        t->Branch("tauNeutralIsoPtSum","vector<float>",&neutralIsoPtSum);
        t->Branch("tauIsoDeltaBetaCorr","vector<float>",&isoDeltaBetaCorr);
        t->Branch("tauAgainstEleLoose","vector<int>",&againstEleLoose);
        t->Branch("tauAgainstEleMedium","vector<int>",&againstEleMedium);
        t->Branch("tauAgainstMuLoose","vector<int>",&againstMuLoose);
        t->Branch("tauAgainstMuTight","vector<int>",&againstMuTight);
        }

}

void BareTaus::setBranchAddresses(TTree *t){
    BareP4::setBranchAddresses(t,"tau");

    if (t->GetBranchStatus("tauId"))
        t->SetBranchAddress("tauId"	,&id);
    //
    if (t->GetBranchStatus("tauQ"))
        t->SetBranchAddress("tauQ"	,&Q);
    //
    if (t->GetBranchStatus("tauM"))
        t->SetBranchAddress("tauM"	,&M);
    //
    if (t->GetBranchStatus("tauIso"))
        t->SetBranchAddress("tauIso"	,&iso);

    // EXTENDED VARIBALES
    if ( IsExtend() )
        {
            if (t->GetBranchStatus("tauChargedIsoPtSum"))
                  t->SetBranchAddress("tauChargedIsoPtSum",&chargedIsoPtSum);
            if (t->GetBranchStatus("tauNeutralIsoPtSum"))
                t->SetBranchAddress("tauNeutralIsoPtSum",&neutralIsoPtSum);
            if (t->GetBranchStatus("tauIsoDeltaBetaCorr"))
                t->SetBranchAddress("tauIsoDeltaBetaCorr",&isoDeltaBetaCorr);
            if (t->GetBranchStatus("tauAgainstEleLoose"))
                t->SetBranchAddress("tauAgainstEleLoose",&againstEleLoose);
            if (t->GetBranchStatus("tauAgainstEleMedium"))
                t->SetBranchAddress("tauAgainstEleMedium",&againstEleMedium);
            if (t->GetBranchStatus("tauAgainstMuLoose"))
                t->SetBranchAddress("tauAgainstMuLoose",&againstMuLoose);
            if (t->GetBranchStatus("tauAgainstMuTight"))
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
