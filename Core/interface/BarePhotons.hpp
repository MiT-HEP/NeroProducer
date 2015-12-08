#ifndef BARE_PHOTONS_H
#define BARE_PHOTONS_H

#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"


class BarePhotons : virtual public BareP4
{
    public:
        enum Selection{
            PhoBaseline = 1UL << 0,
            PhoLoose  = 1UL << 3, 
            PhoMedium = 1UL << 4,
            PhoTight  = 1UL << 5,
            PhoElectronVeto =1UL<<7,
            // NONPOG
            PhoVLoose50 = 1UL<<8, // loose, no-sieie, looser ph-iso
            PhoVLoose25 = 1UL <<9, // loose + no-sieie, looser ph-iso
            PhoHighPt = 1UL << 10,
            PhoLooseNoEVeto = 1UL << 11,
            PhoMediumNoEVeto = 1UL << 12,
            PhoTightNoEVeto = 1UL << 13
        };

        BarePhotons();
        ~BarePhotons();
        void init() override;
        void clear() override;
        void defineBranches(TTree*) override;
        void defineBranches(TTree* t, std::string) override { defineBranches(t); }
        void setBranchAddresses(TTree*) override;
        void setBranchAddresses(TTree* t, std::string) override { setBranchAddresses(t); }
        inline string name() override { return "BarePhotons"; }

        bool inline passSelection(const unsigned &idx, const Selection &sel) const  { return (selBits->at(idx) & sel) != 0; }
        // -- variables
        //TClonesArray *p4{0};
        vector<float> *sieie{0};
        vector<float> *iso{0};
        vector<unsigned> *selBits{0};

        vector<float> *chIso{0};
        vector<float> *chIsoRC{0};
        vector<float> *nhIso{0};
        vector<float> *nhIsoRC{0};
        vector<float> *phoIso{0};
        vector<float> *phoIsoRC{0};
        vector<float> *puIso{0};
        vector<float> *puIsoRC{0};

        // EXTENDED VARIABLES
        vector<float> *rawpt{0};
        vector<float> *r9{0};
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
