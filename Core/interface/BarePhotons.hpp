#ifndef BARE_PHOTONS_H
#define BARE_PHOTONS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BarePhotons : virtual public BareP4
{
    public:
        BarePhotons();
        ~BarePhotons();
        void init();
        void clear();
        void defineBranches(TTree *t);
        void setBranchAddresses(TTree*) ;
        virtual inline string name() { return "BarePhotons"; }

        // -- variables
        //TClonesArray *p4{0};
        vector<float> *sieie{0};
        vector<float> *iso{0};
        vector<int> *looseid{0};
        vector<int> *tightid{0};
        vector<int> *mediumid{0};

        vector<float> *chIso{0};
        vector<float> *chIsoRC{0};
        vector<float> *nhIso{0};
        vector<float> *nhIsoRC{0};
        vector<float> *phoIso{0};
        vector<float> *phoIsoRC{0};
        vector<float> *puIso{0};
        vector<float> *puIsoRC{0};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
