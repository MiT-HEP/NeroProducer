#ifndef NERO_VERTEX_H
#define NERO_VERTEX_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"


class NeroVertex : public NeroCollection
{
public:
	NeroVertex();
	~NeroVertex();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event& iEvent);
	
	// --- specific fuctions
	// --- Handle
	edm::Handle<reco::VertexCollection> handle;
	// --- Token
	edm::EDGetTokenT<reco::VertexCollection> token;

	// -- variables
	const reco::Vertex *pv_;	
	const inline reco::Vertex* GetPV(){ return pv_ ; }

};


#endif
