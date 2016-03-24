#ifndef BARE_PUPPIFATJETS_H
#define BARE_PUPPIFATJETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"

class BarePuppiFatJets : virtual public BareP4
{
    public:

        BarePuppiFatJets();
        ~BarePuppiFatJets();
        virtual void init() override;
        void clear() override;
        void defineBranches(TTree* t) override;

        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string prefix) override;

        std::string cachedPrefix="";

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

        TClonesArray  *subjet{0};
        vector<int>   *nSubjets{0};
        vector<int>   *firstSubjet{0};
        vector<float> *subjet_btag{0};
    
        vector<float> *hbb{0};
        vector<float> *topMVA{0};

};

BAREREGISTER(BarePuppiFatJets);


#endif
