#ifndef BARE_FATJETS_H
#define BARE_FATJETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BareFatJets : virtual public BareP4
{
    public:
        BareFatJets();
        ~BareFatJets();
        void clear() override;
        void init() override;
        void defineBranches(TTree* t) override { defineBranches(t, ""); }
        void defineBranches(TTree*, std::string) override;
        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string) override;
        inline string name() override { return "BareFatJets"; }
        void compress() override;

        // -- variables
        //TClonesArray  *p4;
        vector<float> *rawPt{0};
        vector<int>   *flavour{0};
        vector<float> *tau1{0};
        vector<float> *tau2{0};
        vector<float> *tau3{0};

        vector<float> *trimmedMass{0};
        vector<float> *prunedMass{0};
        vector<float> *filteredMass{0};
        vector<float> *softdropMass{0};

        TClonesArray  *ak8_subjet{0};
        vector<int>   *ak8jet_hasSubjet{0};
        vector<float> *ak8subjet_btag{0};
    
        vector<float> *hbb{0};
        vector<float> *topMVA{0};

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
