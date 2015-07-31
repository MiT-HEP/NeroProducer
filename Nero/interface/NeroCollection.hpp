#ifndef NERO_COLLECTION_H
#define NERO_COLLECTION_H

#include "NeroProducer/Nero/interface/Includes.hpp"
#include "NeroProducer/Core/interface/BareCollection.hpp"
// user include files

#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TTree.h"


#include <vector>
using namespace std;

class NeroCollection : virtual public BareCollection
{
    public:
        // --- Just virtual
        virtual int  analyze(const edm::Event &) = 0 ;
        // -- usually just need the iEvent. This one will be the one called
        virtual int  analyze(const edm::Event &iEvent,const edm::EventSetup& iSetup) { return analyze(iEvent) ; } ;
        virtual inline string name(){return "NeroCollection";};

        // -- configuration
        bool mOnlyMc; // NO RECO INFO
};

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
