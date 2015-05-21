#ifndef NERO_MET_H
#define NERO_MET_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"


class NeroMet : public NeroCollection
{
public:
	NeroMet();
	~NeroMet();
	void clear();
	void defineBranches(TTree *t);
	int analyze(const edm::Event& iEvent);
	virtual inline string name(){return "NeroMet";};
	
	// --- specific fuctions
	static bool JetId(const pat::Jet &);
	// --- Handle
	edm::Handle<pat::METCollection> handle;	

	// --- Token
	edm::EDGetTokenT<pat::METCollection> token;

	// -- variables
	TClonesArray *p4;
	vector<float> *ptJESUP;
	vector<float> *ptJESDOWN;

	// Should not stay here, but in MC -> we will esculed it if run on onlyMc
	TClonesArray *genP4;

};


#endif
