#ifndef NERO_JETS_H
#define NERO_JETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"


class NeroJets : public NeroCollection
{
public:
	NeroJets();
	~NeroJets();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event& iEvent);
	
	// --- specific fuctions
	static bool JetId(const pat::Jet &);
	// --- Handle
	edm::Handle<pat::JetCollection> handle;	
	edm::Handle<edm::ValueMap<float>> qg_handle;

	// --- Token
	edm::EDGetTokenT<pat::JetCollection> token;
	edm::EDGetTokenT<edm::ValueMap<float> > qg_token;

	// -- variables
	TClonesArray *p4;
	vector<float> *rawPt;
	vector<float> *bDiscr;
	vector<float> *puId;
	vector<float> *unc;
	vector<float> *qgl;
	vector<int>   *flavour;
	

};


#endif
