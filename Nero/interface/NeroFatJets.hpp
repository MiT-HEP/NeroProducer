#ifndef NERO_FATJETS_H
#define NERO_FATJETS_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"


class NeroFatJets : public NeroCollection
{
public:
	NeroFatJets();
	~NeroFatJets();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event& iEvent);
	virtual inline string name(){return "NeroFatJets";};
	
	// --- specific fuctions
	// --- Handle
	edm::Handle<pat::JetCollection> handle;	

	// --- Token
	edm::EDGetTokenT<pat::JetCollection> token;

	// -- variables
	TClonesArray *p4;
	vector<float> *rawPt;
	vector<int>   *flavour;
	vector<float> *tau1;
	vector<float> *tau2;
	vector<float> *tau3;

	vector<float> *trimmedMass;
	vector<float> *prunedMass;
	vector<float> *filteredMass;
	vector<float> *softdropMass;
	

};


#endif
