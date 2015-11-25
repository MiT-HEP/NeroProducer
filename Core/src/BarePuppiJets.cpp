#include "NeroProducer/Core/interface/BarePuppiJets.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BarePuppiJets::BarePuppiJets() : BareP4() {}

BarePuppiJets::~BarePuppiJets(){
    BareFunctions::Delete(rawPt);
    BareFunctions::Delete(bDiscr);
    BareFunctions::Delete(selBits);
}

void BarePuppiJets::init(){
    BareP4::init();
    BareFunctions::New(rawPt);
    BareFunctions::New(bDiscr);
    BareFunctions::New(selBits);
}

void BarePuppiJets::clear(){
    BareP4::clear();
    rawPt -> clear();
    bDiscr -> clear();
    selBits -> clear();
}

void BarePuppiJets::defineBranches(TTree *t, std::string prefix){

    TString jetName("puppijet" + prefix);

    BareP4::defineBranches(t, jetName.Data());

    t->Branch(jetName + "RawPt","vector<float>",&rawPt);
    t->Branch(jetName + "Bdiscr","vector<float>",&bDiscr);
    t->Branch(jetName + "SelBits","vector<unsigned>",&selBits);
}

void BarePuppiJets::setBranchAddresses(TTree* t, std::string prefix)
{
    TString jetName("puppijet" + prefix);

    BareP4::setBranchAddresses(t,jetName.Data());
    BareFunctions::SetBranchAddress(t,jetName + "RawPt"	,&rawPt);
    BareFunctions::SetBranchAddress(t,jetName + "Bdiscr"	,&bDiscr);
    BareFunctions::SetBranchAddress(t,jetName + "SelBits", &selBits);

}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
