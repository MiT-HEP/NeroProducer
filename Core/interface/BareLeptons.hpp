#ifndef BARE_LEPTONS_H
#define BARE_LEPTONS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"

class BareLeptons : virtual public BareCollection
{
    public:
        BareLeptons();
        ~BareLeptons();
        void clear();
        void defineBranches(TTree *t);
        void setBranchAddresses(TTree*);
        virtual inline string name(){return "BareLeptons";};
        // ----
        TClonesArray  *p4;
        vector<int>   *pdgId;	
        vector<float> *iso;	
        vector<int>   *tightId;	
        vector<float> *lepPfPt;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
