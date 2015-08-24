#include "NeroProducer/Core/interface/BareFunctions.hpp"

void BareFunctions::Compress(TLorentzVector &a)
{
	float x = a.Px();
	float y = a.Py();
	float z = a.Pz();
	float e = a.E();
	a.SetPxPyPzE( x,y,z,e);

}
