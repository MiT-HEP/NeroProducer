#ifndef BARE_MET_H
#define BARE_MET_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BareMet : virtual public BareP4
{
    public:
        BareMet();
        ~BareMet();
        void clear();
        void defineBranches(TTree *t);
        virtual void setBranchAddresses(TTree*) ;
        virtual inline string name(){return "BareMet";};

        // -- variables
        //TClonesArray *p4;
        vector<float> *ptJESUP;
        vector<float> *ptJESDOWN;

        // Should not stay here, but in MC -> we will esculed it if run on onlyMc
        TClonesArray *genP4;
    
        float metNoMu;
        float phiNoMu;

        float metChargedHadron;
        float phiChargedHadron;

        float metNeutralHadron;
        float phiNeutralHadron;

        float metNeutralEM;
        float phiNeutralEM;

};
#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
