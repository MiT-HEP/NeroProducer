#ifndef BARE_TAUS_H
#define BARE_TAUS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareTaus : virtual public BareP4
{
    public:
        BareTaus();
        ~BareTaus();
        void init() override;
        void clear() override;
        using BareP4::defineBranches;
        void defineBranches(TTree*) override;
        using BareP4::setBranchAddresses;
        void setBranchAddresses(TTree*) override;
        inline string name() override { return "BareTaus"; }

        // --  members
        //TClonesArray    *p4{0};
        vector<float>   *id{0}; 
        vector<int>     *Q{0}; // charge
        vector<float>   *M{0}; // mass
        vector<float>   *iso{0};

        // EXTENDED VARIABLES
        vector<float>   *chargedIsoPtSum{0};
        vector<float>   *neutralIsoPtSum{0};
        vector<float>   *isoDeltaBetaCorr{0};
        vector<int>   *againstEleLoose{0};
        vector<int>   *againstEleMedium{0};
        vector<int>   *againstMuLoose{0};
        vector<int>   *againstMuTight{0};
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
