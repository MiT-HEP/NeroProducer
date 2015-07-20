#ifndef BARE_JETS_H
#define BARE_JETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareJets : virtual public BareP4
{
    public:
        BareJets();
        ~BareJets();
        void clear();
        void defineBranches(TTree*);
        virtual void setBranchAddresses(TTree*);
        virtual inline string name(){ return "BareJets";}

        // -- variables
        //TClonesArray  *p4;
        vector<float> *rawPt;
        vector<float> *bDiscr;
        vector<float> *bDiscrLegacy;
        vector<float> *puId;
        vector<float> *unc;
        vector<float> *qgl;
        vector<int>   *flavour;
        vector<int>   *matchedPartonPdgId;
        vector<int>   *motherPdgId;
        vector<int>   *grMotherPdgId;
        vector<bool>  *mjId;
        vector<bool>  *mjId_loose;

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
