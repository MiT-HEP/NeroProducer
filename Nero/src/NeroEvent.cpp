#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroEvent::NeroEvent(){
}

NeroEvent::~NeroEvent(){
}

void NeroEvent::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	isRealData = -1;
	runNum = -1;
	lumiNum = -1;
	eventNum = 0;
	rho = -999;
}

void NeroEvent::defineBranches(TTree *t){
	//
	t->Branch("isRealData"  ,&isRealData   ,"isRealData/I");
  	t->Branch("runNum"      ,&runNum       ,"runNum/I");
  	t->Branch("lumiNum"     ,&lumiNum      ,"lumiNum/I");
  	t->Branch("eventNum"    ,&eventNum     ,"eventNum/l");
  	t->Branch("rho"    ,&rho     ,"rho/D");
}

int NeroEvent::analyze(const edm::Event& iEvent){
	isRealData = iEvent.isRealData() ? 1 : 0 ;
	lumiNum    = iEvent.luminosityBlock();
	eventNum   = iEvent.id().event();

	iEvent.getByToken(rho_token,rho_handle);

	rho 	   =  *rho_handle;
	return 0;
}

