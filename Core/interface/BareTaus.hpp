#ifndef BARE_TAUS_H
#define BARE_TAUS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareTaus : virtual public BareP4
{
    public:
        enum Selection{
            TauDecayModeFindingNewDMs = 1UL <<0,
            TauDecayModeFinding = 1UL <<1,

            // 0--7 POG
            AgainstEleLoose = 1UL <<8,
            AgainstEleMedium = 1UL<<9,
            AgainstMuLoose = 1UL<<10,
            AgainstMuTight = 1UL<<11

        };
        BareTaus();
        ~BareTaus();
        void init() override;
        void clear() override;
        void defineBranches(TTree*) override;
        void defineBranches(TTree* t, std::string) override { defineBranches(t); }
        void setBranchAddresses(TTree*) override;
        void setBranchAddresses(TTree* t, std::string) override { setBranchAddresses(t); }
        inline string name() override { return "BareTaus"; }

        // --  members
        //TClonesArray    *p4{0};
        vector<int>     *Q{0}; // charge
        vector<float>   *M{0}; // mass
        vector<float>   *iso{0};

        // EXTENDED VARIABLES
        vector<float>   *chargedIsoPtSum{0};
        vector<float>   *neutralIsoPtSum{0};
        vector<float>   *isoDeltaBetaCorr{0};
        vector<unsigned> *selBits{0};
};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
