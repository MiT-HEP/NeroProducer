#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroVertex::NeroVertex(edm::ConsumesCollector& cc,edm::ParameterSet iConfig) :
        NeroCollection(cc,iConfig),
        BareVertex(){
    token = cc.consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"));
}

NeroVertex::~NeroVertex(){
}

void NeroVertex::clear(){
    BareVertex::clear();
    // This function clear all the internal storage and init it to an arbitrary value
    pv_ = NULL;
}

int NeroVertex::analyze(const edm::Event& iEvent){

    iEvent.getByToken(token, handle);
    if ( not handle.isValid() ) cout<<"[NeroVertex]::[analyze]::[ERROR] handle is not valid"<<endl;

    if (handle->empty()) return 1; // skip the event if no PV found

    using namespace std;
    using namespace edm;
    using namespace reco;

    npv= handle->size();
    pv_  = &handle->front();

    return 0;
}

NEROREGISTER(NeroVertex);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
