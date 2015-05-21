#ifndef NERO_COLLECTION_H
#define NERO_COLLECTION_H

#include "NeroProducer/Nero/interface/Includes.hpp"
// user include files

#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TTree.h"

#include <vector>
using namespace std;

class NeroCollection
{
public:
	// --- Just virtual
	virtual void clear() = 0;
	virtual void defineBranches(TTree*) = 0 ;
	virtual int  analyze(const edm::Event &) = 0 ;
	virtual inline string name(){return "NeroCollection";};
};
#endif
