#ifndef BARE_P4_OBJ_H
#define BARE_P4_OBJ_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareP4 : virtual public BareCollection
{
    // just a container to identify all the objects that are based on p4
    bool doMatch_;
    public:
        BareP4(){ p4=NULL;doMatch_=false;match=NULL;}

        virtual inline string name(){ return "BareP4";}

        TClonesArray  *p4;
        vector<int>   *match;

        virtual bool isInit(){ return p4 != NULL;}

        inline void SetMatch(bool match=true){doMatch_=match;}
        inline bool doMatch() { return doMatch_;}

        virtual void clear();
        void defineBranches(TTree *t,string prefix="");
        void setBranchAddresses(TTree*t,string prefix ="");
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
