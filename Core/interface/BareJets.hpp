#ifndef BARE_JETS_H
#define BARE_JETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"
#include <map>

class BareJets : virtual public BareP4
{
    public:

        enum Selection{
            JetBaseline = 1UL << 0,
            JetLoose  = 1UL << 3, 
            JetMedium = 1UL << 4,
            JetTight  = 1UL << 5,
            // 0--7 POG
            mjId = 1UL<< 8,
            mjIdLoose = 1UL << 9,
            mjId2015 = 1UL << 10
            //TODO add Pileup Id and remove corresponding variable

        };

        std::string cachedPrefix="";

        BareJets();
        virtual ~BareJets();
        void init() override;
        void clear() override;
        void defineBranches(TTree* t) override;
        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string prefix) override;
        inline string name() override { return "BareJets"; }

        // -- variables
        //TClonesArray  *p4{0};
        vector<float> *rawPt{0};
        //vector<float> *refPt{0}; // as in MINIAOD->pt()
        vector<float> *bDiscr{0};
        vector<float> *bMva{0};
        
        // deep information for jets
        vector<float> *deepB{0};
        vector<float> *deepBB{0};
        vector<float> *deepC{0};
        vector<float> *deepL{0};

        vector<float> *bcorr{0};
        vector<float> *bcorrunc{0};

        //vector<float> *bDiscrLegacy{0};
        vector<float> *puId{0};
        vector<float> *unc{0};
        // JER:
        vector<float> *ptResUncCentral{0};
        vector<float> *ptResUncUp{0};
        vector<float> *ptResUncDown{0};
        // energy fractions
        vector<float> *chef{0};
        vector<float> *nhef{0};
        vector<float> *nemf{0};
        vector<float> *cemf{0};
        vector<int>   *cm{0};
        vector<int>   *nm{0};
        //
        vector<float> *qgl{0};
        vector<int>   *flavour{0};
        vector<int>   *hadFlavour{0};
        vector<int>   *matchedPartonPdgId{0};
        vector<int>   *motherPdgId{0};
        vector<int>   *grMotherPdgId{0};
        vector<unsigned> *selBits{0};

        // QGL variables
        vector<int>   *qglMult{0};
        vector<float> *qglPtD{0};
        vector<float> *qglAxis2{0};
        //extra
        vector<float> *qglAxis1{0};
        vector<int> *qglCMult{0};
        vector<int> *qglNMult{0};
};



#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
