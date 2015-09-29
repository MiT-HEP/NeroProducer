#ifndef NERO_MET_H
#define NERO_MET_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareMet.hpp"
#include "NeroProducer/Nero/interface/NeroPF.hpp"


class NeroMet : virtual public NeroCollection,
    virtual public BareMet
{
    public:
        NeroMet();
        ~NeroMet();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroMet";};

        // --- Handle
        edm::Handle<pat::METCollection> handle;	
        edm::Handle<pat::METCollection> handle_noHF;
        edm::Handle<pat::METCollection> handle_puppi;

        // --- Token
        edm::EDGetTokenT<pat::METCollection> token;
        edm::EDGetTokenT<pat::METCollection> token_noHF;
        edm::EDGetTokenT<pat::METCollection> token_puppi;
        //
        NeroPF * pf;

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
