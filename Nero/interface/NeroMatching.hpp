#ifndef NERO_MATCHING_H
#define NERO_MATCHING_H

#include "NeroProducer/Nero/interface/Includes.hpp"
#include "NeroProducer/Core/interface/BareP4.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"

class Matcher
{
    protected:
        float dR_;
    public:
        Matcher(){dR_ = 0.3; }
        ~Matcher(){}
        
        void SetDr(float dR ){dR_=dR;} 

        void match(TClonesArray *a, TClonesArray *b, 
                vector<int> &r,
                vector<int> *pdgIdB = NULL, // of "b"
                int pdgId=0  // of "a", if NULL or 0 will be neglected
                );
        void match(BareP4* a, BareP4*b, vector<int> &r);
        // for each "a" object performe a 
        // deltaR matching within the two collections, 
        // returns -1 if not matched otherwise the position in "b"
        // double matching returns -2
};

class NeroMatching :virtual public NeroCollection
{
    // This analyzer needs to be run after both the reco and the gen information is filled
    public:
        NeroMatching():NeroCollection(){};
        ~NeroMatching(){};
        int analyze(const edm::Event& iEvent);
        void clear(){};
        void setBranchAddresses(TTree*){};
        void defineBranches(TTree*){};
        virtual inline string name(){return "NeroMatching";};
        
        Matcher matcher_;

        NeroJets    *jets_;
        NeroLeptons *leps_;
        NeroPhotons *phos_;
        NeroTaus    *taus_;
        NeroMonteCarlo *mc_;

        // --- configuration
        float mTauDr;
        float mJetDr;
        float mLepDr;
        float mPhoDr;
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
