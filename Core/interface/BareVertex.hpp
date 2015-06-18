#ifndef BARE_VERTEX_H
#define BARE_VERTEX_H

#include "NeroProducer/Core/interface/BareCollection.hpp"


class BareVertex : virtual public BareCollection
{
    public:
        BareVertex();
        ~BareVertex();
        void clear();
        void defineBranches(TTree *t);
        virtual void setBranchAddresses(TTree*);
        virtual inline string name(){return "BareVertex";};

};
#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
