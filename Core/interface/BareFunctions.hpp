#ifndef BARE_FUNCTIONS_H
#define BARE_FUNCTIONS_H

#include "TTree.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"

namespace BareFunctions{
    // Compress to float precision, 
    // zeroing  double precision elements
    void Compress(TLorentzVector &a);

    template< typename TYPE >
        void SetBranchAddress(TTree *t,const char *name, TYPE *x)
        {
            if(t->GetBranchStatus(name) ) 
                t->SetBranchAddress(name,x);
        }

    template< typename TYPE>
        void Delete(TYPE &x)
        {
            delete x;
            x = NULL;
        }

    template<typename TYPE>
        void New(TYPE* &x)
        {
            if (x==NULL) x=new TYPE();
        }

    //template specification for TClonesArray
    template<> void New<TClonesArray>(TClonesArray* &x);

}; // end namespace
#endif

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
