#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroEvent::NeroEvent():
        NeroCollection(),
        BareEvent()
{
}

NeroEvent::~NeroEvent(){
}

int NeroEvent::analyze(const edm::Event& iEvent){
    isRealData = iEvent.isRealData() ? 1 : 0 ;
    runNum     = iEvent.id().run();
    lumiNum    = iEvent.luminosityBlock();
    eventNum   = iEvent.id().event();

    if ( mOnlyMc  ) return 0;
    iEvent.getByToken(rho_token,rho_handle);

    rho 	   =  *rho_handle;
    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
