#ifndef BARE_PUPPIJETS_H
#define BARE_PUPPIJETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BarePuppiJets : virtual public BareP4
{
    public:
        enum Selection{
            // 0--7 POG
            JetBaseline = 1UL << 0,
            JetLoose  = 1UL << 3, 
            JetMedium = 1UL << 4,
            JetTight  = 1UL << 5
        };

        BarePuppiJets();
        ~BarePuppiJets();
        void init() override;
        void clear() override;
        void defineBranches(TTree* t) override { defineBranches(t, ""); }
        void defineBranches(TTree*, std::string prefix) override;

        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string prefix) override;

        inline string name() override { return "BarePuppiJets"; }

        // variables --
        vector<float> *rawPt{0};
        vector<float> *bDiscr{0};
        vector<unsigned> *selBits{0};
        vector<float> *Q{0};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
