#ifndef BARE_FATJETS_H
#define BARE_FATJETS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BareFatJets : virtual public BareP4
{
    public:
        BareFatJets();
        ~BareFatJets();
        void clear();
        void defineBranches(TTree *t);
        virtual void setBranchAddresses(TTree*);
        virtual inline string name(){return "BareFatJets";};
        virtual void compress();

        // -- variables
        //TClonesArray  *p4;
        vector<float> *rawPt;
        vector<int>   *flavour;
        vector<float> *tau1;
        vector<float> *tau2;
        vector<float> *tau3;

        vector<float> *trimmedMass;
        vector<float> *prunedMass;
        vector<float> *filteredMass;
        vector<float> *softdropMass;

        TClonesArray  *ak8_subjet;
        vector<int>   *ak8jet_hasSubjet;
        vector<float> *ak8subjet_btag;
    
        vector<float> *hbb;

};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
