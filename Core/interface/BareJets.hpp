#ifndef BARE_JETS_H
#define BARE_JETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BareJets : virtual public BareP4
{
    public:
        BareJets();
        ~BareJets();
        void init() override;
        void clear() override;
        // non-prefix versions are defined in BareP4 and will call the following functions with prefix = ""
        using BareP4::defineBranches;
        void defineBranches(TTree*, std::string prefix) override;
        using BareP4::setBranchAddresses;
        void setBranchAddresses(TTree*, std::string prefix) override;
        inline string name() override { return "BareJets"; }

        // -- variables
        //TClonesArray  *p4{0};
        vector<float> *rawPt{0};
        vector<float> *bDiscr{0};
        vector<float> *bDiscrLegacy{0};
        vector<float> *puId{0};
        vector<float> *unc{0};
        vector<float> *qgl{0};
        vector<int>   *flavour{0};
        vector<int>   *matchedPartonPdgId{0};
        vector<int>   *motherPdgId{0};
        vector<int>   *grMotherPdgId{0};
        vector<bool>  *mjId{0};
        vector<bool>  *mjId_loose{0};

        vector<float> *Q{0};
        vector<float> *QnoPU{0};

};

#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
