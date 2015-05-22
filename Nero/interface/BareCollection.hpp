#ifndef BARE_COLLECTION_H
#define BARE_COLLECTION_H

// ONLY STD&ROOT INCLUDES
#include "TTree.h"
#include "TFile.h"
#include "TH1F.h"
#include <vector>
#include <string>
using namespace std;

class BareCollection
{
public:
	virtual void clear() = 0;
	virtual void defineBranches(TTree*) = 0 ;
	virtual string name() = 0;
};

#endif
