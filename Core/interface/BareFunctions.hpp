#ifndef BARE_FUNCTIONS_H
#define BARE_FUNCTIONS_H

#include "TLorentzVector.h"

namespace BareFunctions{
	// Compress to float precision, 
	// zeroing  double precision elements
	void Compress(TLorentzVector &a);
};

#endif
