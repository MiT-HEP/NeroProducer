#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroVertex::NeroVertex(){
}

NeroVertex::~NeroVertex(){
}

void NeroVertex::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	pv_ = NULL;
}

void NeroVertex::defineBranches(TTree *t){
	// nothing to be done

}

int NeroVertex::analyze(const edm::Event& iEvent){
	iEvent.getByToken(token, handle);
	if (handle->empty()) return 1; // skip the event if no PV found
	const reco::Vertex &PV = handle->front();
	pv_ = &PV; // it should remain after ending
	return 0;
}

