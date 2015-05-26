#include "NeroProducer/Nero/interface/BareEvent.hpp"

BareEvent::BareEvent(){};
BareEvent::~BareEvent(){};

void BareEvent::clear()
{
	// This function clear all the internal storage and init it to an arbitrary value
	isRealData = -1;
	runNum = -1;
	lumiNum = -1;
	eventNum = 0;
	rho = -999;

}

void BareEvent::defineBranches(TTree *t){
	//
	t->Branch("isRealData"  ,&isRealData   ,"isRealData/I");
  	t->Branch("runNum"      ,&runNum       ,"runNum/I");
  	t->Branch("lumiNum"     ,&lumiNum      ,"lumiNum/I");
  	t->Branch("eventNum"    ,&eventNum     ,"eventNum/l");
  	t->Branch("rho"    ,&rho     ,"rho/F");
}

void BareEvent::setBranchAddresses(TTree *t){
	//
	t->SetBranchAddress("isRealData"  ,&isRealData  );
  	t->SetBranchAddress("runNum"      ,&runNum      );
  	t->SetBranchAddress("lumiNum"     ,&lumiNum     );
  	t->SetBranchAddress("eventNum"    ,&eventNum    );
  	t->SetBranchAddress("rho"    	,&rho         );
}
