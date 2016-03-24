#ifndef BARE_EVENT_H
#define BARE_EVENT_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareEvent : virtual public BareCollection
{
    public:
        BareEvent();
        ~BareEvent();
        void clear() override;
        void defineBranches(TTree*) override;
        void setBranchAddresses(TTree*) override;
        inline string name() override { return "BareEvent"; }
        inline unsigned size() const override { return 1; }
    

        // -- variables
        int isRealData;
        int runNum;
        int lumiNum;
        ULong64_t eventNum;

        float rho;	
        //
        //extend
        int originalRun;
        int originalLumi;
        int originalEvent;
};

BAREREGISTER(BareEvent);

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
