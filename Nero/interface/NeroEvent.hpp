#ifndef NERO_EVENT_H
#define NERO_EVENT_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareEvent.hpp"

#include <map>
#include <string>

class NeroEvent : virtual public NeroCollection, virtual public BareEvent
{
    public:
        NeroEvent():NeroCollection(){};
        NeroEvent(edm::ConsumesCollector & cc,edm::ParameterSet iConfig );
        ~NeroEvent();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroEvent";};

        // --- specific fuctions
        // --- Handle
        edm::Handle<double> rho_handle;

        // --- Token
        edm::EDGetTokenT<double> rho_token;

        // --- met filters
        edm::EDGetTokenT<edm::TriggerResults> filter_token;
        edm::EDGetTokenT<bool> BadChCandFilter_token;
        edm::EDGetTokenT<bool> BadPFMuon_token;
        
        // need to convert the name to bit in the enum
        map<string,unsigned> metNameToBit;

        // Ecal Bad Filter Token
        edm::EDGetTokenT<bool> ecalBadCalibFilterUpdate_token ;
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
