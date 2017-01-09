#ifndef NERO_FUNCTIONS_H
#define NERO_FUNCTIONS_H
#include "NeroProducer/Nero/interface/Includes.hpp"

namespace NeroFunctions{
    template<typename T> 
        double getEGSeedCorrections(const T& electron,const edm::Handle<EcalRecHitCollection>& ebRecHits
                //const edm::Handle<EcalRecHitCollection>& eeRecHits 
                )
        {

            DetId detid = electron.superCluster()->seed()->seed();
            const EcalRecHit * rh = NULL;
            double Ecorr=1;
            if (detid.subdetId() == EcalBarrel) {
                auto rh_i =  ebRecHits->find(detid);
                if( rh_i != ebRecHits->end()) rh =  &(*rh_i);
                else rh = NULL;
            } 
            // No correction for the EE
            //else {
            //    auto rh_i =  eeRecHits->find(detid);
            //    if( rh_i != eeRecHits->end()) rh =  &(*rh_i);
            //    else rh = NULL;
            //}
            if(rh==NULL) Ecorr=1;
            else{
                if(rh->energy() > 200 && rh->energy()<300)  Ecorr=1.0199;
                else if(rh->energy()>300 && rh->energy()<400) Ecorr=  1.052;
                else if(rh->energy()>400 && rh->energy()<500) Ecorr = 1.015; 
            }
            return Ecorr;
        }
}

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
