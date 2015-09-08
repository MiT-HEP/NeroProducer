#ifndef BARE_EVENT_H
#define BARE_EVENT_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareEvent : virtual public BareCollection
{
    public:
        BareEvent();
        ~BareEvent();
        void clear();
        void defineBranches(TTree*);
        void setBranchAddresses(TTree*);
        virtual inline string name() { return "BareEvent"; }
        virtual inline unsigned size() const { return 1; }
    

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

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
