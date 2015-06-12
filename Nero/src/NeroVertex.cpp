#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroVertex::NeroVertex():BareVertex(){
}

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
	if (handle->empty()) return 1; // skip the event if no PV found
	const reco::Vertex &PV = handle->front();
	pv_ = &PV; // it should remain after ending

    using namespace std;
    using namespace edm;
    using namespace reco;

    // Find the first vertex in the collection that passes
    // good quality criteria
    VertexCollection::const_iterator firstGoodVertex = handle->end();
    int firstGoodVertexIdx = 0;
    for (VertexCollection::const_iterator vtx = handle->begin(); 
         vtx != handle->end(); ++vtx, ++firstGoodVertexIdx) {
        bool isFake = (vtx->chi2()==0 && vtx->ndof()==0);
        if ( !isFake
             && vtx->ndof()>=4. && vtx->position().Rho()<=2.0
             && fabs(vtx->position().Z())<=24.0) {
            firstGoodVertex = vtx;
            break;
        }
    }
    
    if ( firstGoodVertex==handle->end() ) return 1; // skip event if there are no good PVs
  
	return 0;
}

