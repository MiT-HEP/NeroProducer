#ifndef BARE_MONTECARLO_H
#define BARE_MONTECARLO_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BareMonteCarlo : virtual public BareP4
{
    public:
        BareMonteCarlo();
        ~BareMonteCarlo();
        void init() override;
        void clear() override;
        using BareP4::defineBranches;
        void defineBranches(TTree*) override;
        using BareP4::setBranchAddresses;
        void setBranchAddresses(TTree*) override;
        inline string name() { return "BareMonteCarlo"; }
        void compress() override;

        // -- variables
        //TClonesArray *p4{0}; // gen particles
        vector<int>  *pdgId{0};

        // genjets
        TClonesArray *jetP4{0};

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
