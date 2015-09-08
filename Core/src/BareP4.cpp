#include "NeroProducer/Core/interface/BareP4.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareP4::BareP4() : BareCollection(){
}

BareP4::~BareP4(){
    delete p4;
    delete match;
}

void BareP4::init(){
    if (!p4)
        p4 = new TClonesArray("TLorentzVector", 20);

    if (doMatch_ && !match)
        match = new vector<int>;
}

void BareP4::clear(){ 

	p4->Clear() ; 

	if (doMatch_) match->clear(); 
}

void BareP4::defineBranches(TTree * t, string prefix)
{
    init();

    t->Branch( (prefix + "P4").c_str(),"TClonesArray", &p4, 128000, 0);

    if (doMatch_){
    	t->Branch( (prefix+"Match").c_str(),"vector<int>",&match);
    }
}

void BareP4::setBranchAddresses(TTree* t, string prefix)
{
    init();

    if (t->GetBranchStatus((prefix + "P4").c_str()))
      t->SetBranchAddress( (prefix+"P4").c_str(), &p4	);

    if ( doMatch_ && t->GetBranchStatus((prefix + "Match").c_str()) )
    	t->SetBranchAddress((prefix+"Match").c_str()	,&match);
}

void BareP4::compress(){
	for(int i=0;i<p4->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) p4->At(i)  );
}
