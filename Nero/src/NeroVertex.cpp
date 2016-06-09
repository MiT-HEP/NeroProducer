#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroVertex::NeroVertex() :
        NeroCollection(),
        BareVertex(){}

NeroVertex::~NeroVertex(){
}

void NeroVertex::clear(){
    BareVertex::clear();
    // This function clear all the internal storage and init it to an arbitrary value
    pv_ = NULL;
}

int NeroVertex::analyze(const edm::Event& iEvent){
    if ( mOnlyMc  ) return 0;
    iEvent.getByToken(token, handle);
    if ( not handle.isValid() ) cout<<"[NeroVertex]::[analyze]::[ERROR] handle is not valid"<<endl;

    if (handle->empty()) return 1; // skip the event if no PV found

    using namespace std;
    using namespace edm;
    using namespace reco;

    // Find the first vertex in the collection that passes
    // good quality criteria
    /*
    VertexCollection::const_iterator firstGoodVertex = handle->end();
    firstGoodVertexIdx = -1;
    npv=0;
    for (VertexCollection::const_iterator vtx = handle->begin(); 
            vtx != handle->end(); ++vtx) {
        bool isFake = (vtx->chi2()==0 && vtx->ndof()==0);
        if ( !isFake
                && vtx->ndof()>=4. && vtx->position().Rho()<=2.0
                && fabs(vtx->position().Z())<=24.0) {
            if(firstGoodVertex == handle->end() ) { 
                firstGoodVertex = vtx; 
                firstGoodVertexIdx = firstGoodVertex - handle->begin() ;
            }
            ++npv;
            //break;
        }
    }

    if ( firstGoodVertex==handle->end() ) return 1; // skip event if there are no good PVs
    pv_ = &(*firstGoodVertex); // it should remain after ending
    */
    npv= handle->size();
    if (handle->empty() ) return 1; // skip event if there are no good PVs
    pv_  = &handle->front();

    return 0;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
