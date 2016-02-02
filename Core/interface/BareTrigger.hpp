#ifndef BARE_TRIGGER_H
#define BARE_TRIGGER_H


#include "NeroProducer/Core/interface/BareCollection.hpp"


class BareTrigger: virtual public BareCollection
{
    public:
        BareTrigger();
        ~BareTrigger();
        void init() override;
        void clear() override;
        void defineBranches(TTree*) override;
        void setBranchAddresses(TTree*) override;
        inline string name() override { return "BareTrigger"; }
        inline unsigned size() const override { return triggerNames->size(); }

        //variables
        vector<int>  *triggerFired{0}; // bool vector are not supported in TTree
        vector<float>  *triggerPrescale{0}; // bool vector are not supported in TTree

        //configuration
        // new'ed in the constructor
        vector<string> *triggerNames;

        vector<int>  *triggerLeps{0}; // TODO: int->unsigned, break back compatibility
        vector<int>  *triggerJets{0};
        vector<int>  *triggerTaus{0};
        vector<int>  *triggerPhotons{0};

        vector<unsigned>  *triggerNoneTaus{0};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
