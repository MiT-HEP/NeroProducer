#ifndef NERO_VERTEX_H
#define NERO_VERTEX_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareVertex.hpp"


class NeroVertex : virtual public NeroCollection,
    virtual public BareVertex
{
    public:
        NeroVertex():NeroCollection(){};
        NeroVertex(edm::ConsumesCollector& cc,edm::ParameterSet iConfig );
        ~NeroVertex();
        virtual void clear(); // some extra for the vertex
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroVertex";};

        // --- specific fuctions
        // --- Handle
        edm::Handle<reco::VertexCollection> handle;
        // --- Token
        edm::EDGetTokenT<reco::VertexCollection> token;

        // -- variables
        const reco::Vertex *pv_;	
        const inline reco::Vertex* GetPV(){ return pv_ ; }

        int firstGoodVertexIdx; // save this information
};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
