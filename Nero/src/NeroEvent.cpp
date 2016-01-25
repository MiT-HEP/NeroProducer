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
    if ( not rho_handle.isValid() ) cout <<"[NeroEvent]::[analyze]::[ERROR] rho_handle is not valid"<<endl;

    rho 	   =  *rho_handle;

    if (IsExtend() ) {

        iEvent.getByToken( originalRun_token , originalRun_handle); 
        iEvent.getByToken( originalLumi_token , originalLumi_handle); 
        iEvent.getByToken( originalEvent_token , originalEvent_handle); 

        // -- these are under the 'isValid' condition, so program will not crash if not present
        if( originalRun_handle.isValid() ) originalRun = *originalRun_handle;
        if( originalLumi_handle.isValid()) originalLumi = *originalLumi_handle;
        if( originalEvent_handle.isValid()) originalEvent = *originalEvent_handle;
    }
    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
