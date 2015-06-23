#ifndef BARE_COLLECTION_H
#define BARE_COLLECTION_H

// ONLY STD&ROOT INCLUDES

#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <vector>
#include <string>
using namespace std;

class BareCollection
{
    protected:
        bool extend_;  // for collection switch to a more inclusive mode
    public:
        BareCollection(){ extend_=false;}
        virtual void clear() = 0;
        virtual void defineBranches(TTree*) = 0 ;
        virtual void setBranchAddresses(TTree*) = 0;
        virtual string name() = 0;
        
        inline void SetExtend(bool value=true){extend_=value;}
        inline bool IsExtend(){return extend_;}
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
