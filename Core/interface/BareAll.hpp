#ifndef BARE_ALL_H
#define BARE_ALL_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareAll : virtual public BareCollection 
{
    /* container for the inforamations that will be saved for all events */
    public:
        BareAll();
        ~BareAll();
        void clear();
        void defineBranches(TTree*);
        virtual void setBranchAddresses(TTree*);
        virtual inline string name(){ return "BareAll";}

        // -- variables
        int isRealData;
        int runNum;
        int lumiNum;
        ULong64_t eventNum;

        int puTrueInt;
        float mcWeight;

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
