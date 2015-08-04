#ifndef NEROPF_H
#define NEROPF_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"

class NeroPF : virtual public NeroCollection
{
// class to handle the PFCandidates
    public:
        NeroPF():NeroCollection(){}
        ~NeroPF(){}
        edm::Handle<pat::PackedCandidateCollection> handle;	
        edm::EDGetTokenT<pat::PackedCandidateCollection> token;

        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroPF";};

        // no Bare class for this: define this classes here
        void clear() {}
        void defineBranches(TTree *t) {}
        void setBranchAddresses(TTree*){} 
        
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
