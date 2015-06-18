#ifndef NERO_EVENT_H
#define NERO_EVENT_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareEvent.hpp"


class NeroEvent : virtual public NeroCollection, virtual public BareEvent
{
    public:
        NeroEvent();
        ~NeroEvent();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroEvent";};

        // --- specific fuctions
        // --- Handle
        edm::Handle<double> rho_handle;

        // --- Token
        edm::EDGetTokenT<double> rho_token;


};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
