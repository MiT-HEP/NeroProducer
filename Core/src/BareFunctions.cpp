#include "NeroProducer/Core/interface/BareFunctions.hpp"
#include "TMath.h"

void BareFunctions::Compress(TLorentzVector &a)
{
	float x = a.Px();
	float y = a.Py();
	float z = a.Pz();
	//float e = a.E();
	float m = a.M();
	float e = TMath::Sqrt( x*x + y*y + z*z + m*m ) ;  // set e in such a way that the mass value is kept
	a.SetPxPyPzE( x, y, z, e);
	//a.SetXYZM( x, y, z, m); // e is double precision to preserve m

}
