#ifndef BARE_TAUS_H
#define BARE_TAUS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareTaus : virtual public BareCollection,
    virtual public BareP4
{
    public:
        BareTaus();
        ~BareTaus();
        void clear();
        void defineBranches(TTree *t);
        void setBranchAddresses(TTree*) ;
        virtual inline string name(){return "BareTaus";};

        // --  members
        //TClonesArray    *p4;
        vector<float>   *id ; 
        vector<int>     *Q ; // charge
        vector<float>   *M ; // mass
        vector<float>   *iso ;

        // EXTENDED VARIABLES
        vector<float>   *chargedIsoPtSum;
        vector<float>   *neutralIsoPtSum;
        vector<float>   *isoDeltaBetaCorr;
        vector<int>   *againstEleLoose;
        vector<int>   *againstEleMedium;
        vector<int>   *againstMuLoose;
        vector<int>   *againstMuTight;
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
