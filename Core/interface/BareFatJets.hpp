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
        void defineBranches(TTree* t) override;
        void setBranchAddresses(TTree* t) override { setBranchAddresses(t, ""); }
        void setBranchAddresses(TTree*, std::string) override;
        inline string name() override { return "BareFatJets"; }
        void compress() override;

        std::string cachedPrefix="";
        
        // -- variables
        //TClonesArray  *p4;
        vector<float> *rawPt{0};
        vector<int>   *flavour{0};
        vector<float> *tau1{0};
        vector<float> *tau2{0};
        vector<float> *tau3{0};
        vector<float> *softdropMass{0};

        TClonesArray  *subjet{0};
        vector<int>   *nSubjets{0};
        vector<int>   *firstSubjet{0};
        vector<float> *subjet_btag{0};
    
        vector<float> *hbb{0};
        vector<float> *TvsQCD{0};
        vector<float> *WvsQCD{0};
        vector<float> *ZvsQCD{0};
        vector<float> *ZbbvsQCD{0};
        vector<float> *TvsQCDMD{0};
        vector<float> *WvsQCDMD{0};
        vector<float> *ZHbbvsQCDMD{0};
        vector<float> *ZHccvsQCDMD{0};

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
