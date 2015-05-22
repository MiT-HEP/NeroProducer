#ifndef NERO_COLLECTION_H
#define NERO_COLLECTION_H

#include "NeroProducer/Nero/interface/Includes.hpp"
#include "NeroProducer/Nero/interface/BareCollection.hpp"
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
	virtual inline string name(){return "NeroCollection";};
};
#endif
