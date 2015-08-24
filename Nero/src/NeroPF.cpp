#include "NeroProducer/Nero/interface/NeroPF.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

int NeroPF::analyze(const edm::Event& iEvent)
{
	
    	iEvent.getByToken( token , handle);
        if ( not handle.isValid() ) cout<<"[NeroPF]::[analyze]::[ERROR] handle is not valid"<<endl;
	return 0;
}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
