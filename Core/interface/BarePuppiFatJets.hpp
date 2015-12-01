#ifndef BARE_PUPPIFATJETS_H
#define BARE_PUPPIFATJETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BarePuppiFatJets : virtual public BareP4
{
    public:

        BarePuppiFatJets();
        ~BarePuppiFatJets();
        void init() override;
        void clear() override;
        void defineBranches(TTree* t) override { defineBranches(t, ""); }
        void defineBranches(TTree*, std::string prefix) override;

        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string prefix) override;

        void compress() override;

        inline string name() override { return "BarePuppiFatJets"; }

        // variables --
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
        vector<int>   *ak8_hasSubjet{0};
        vector<float> *ak8subjet_btag{0};
    
        vector<float> *hbb{0};
        vector<float> *topMVA{0};

};

#endif
