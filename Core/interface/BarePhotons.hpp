#ifndef BARE_PHOTONS_H
#define BARE_PHOTONS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BarePhotons : virtual public BareCollection,
    virtual public BareP4
{
    public:
        BarePhotons();
        ~BarePhotons();
        void clear();
        void defineBranches(TTree *t);
        void setBranchAddresses(TTree*) ;
        virtual inline string name(){return "BarePhotons";};

        // -- variables
        //TClonesArray *p4;
        vector<float> *sieie;
        vector<float> *iso;
        vector<int> *tightid;
        vector<int> *mediumid;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
