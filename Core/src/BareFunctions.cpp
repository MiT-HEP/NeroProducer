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

    template<>
void BareFunctions::New<TClonesArray>(TClonesArray* &x)
{
    if(x==NULL) x=new TClonesArray("TLorentzVector",20);
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
