#ifndef BARE_MONTECARLO_H
#define BARE_MONTECARLO_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BareMonteCarlo : virtual public BareP4
{
    public:
        BareMonteCarlo();
        ~BareMonteCarlo();
        void clear();
        void defineBranches(TTree *t);
        void setBranchAddresses(TTree*) ;
        virtual inline string name(){return "BareMonteCarlo";};
        virtual void compress();

        // -- variables
        //TClonesArray *p4; // gen particles
        vector<int>  *pdgId;

        // genjets
        TClonesArray *jetP4;

        //
        int puTrueInt;
        //
        float mcWeight ;
        float qScale   ;
        float alphaQED ;
        float alphaQCD ;
        float x1       ;
        float x2       ;
        int   pdf1Id   ;
        int   pdf2Id   ;
        float scalePdf ;
        

};
#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
