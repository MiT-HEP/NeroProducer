#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroEvent::NeroEvent(){
}

NeroEvent::~NeroEvent(){
}

int NeroEvent::analyze(const edm::Event& iEvent){
	isRealData = iEvent.isRealData() ? 1 : 0 ;
	runNum     = iEvent.id().run();
	lumiNum    = iEvent.luminosityBlock();
	eventNum   = iEvent.id().event();

	iEvent.getByToken(rho_token,rho_handle);

	rho 	   =  *rho_handle;
	return 0;
}

