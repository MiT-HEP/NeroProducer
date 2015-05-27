#ifndef NERO_ALL_H
#define NERO_ALL_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Nero/interface/NeroRunLumi.hpp"
#include "NeroProducer/Core/interface/BareAll.hpp"


class NeroAll : virtual public NeroCollection, 
	virtual public BareAll,
	virtual public NeroLumi
{
public:
	NeroAll();
	~NeroAll();
	int analyze(const edm::Event&);
	int analyzeLumi(const edm::LuminosityBlock&,TTree*);
	virtual inline string name(){return "NeroAll";};

	// --- members	
	int isSkim_; // -1: figure it out. 0 no skim info. 1 skim info
	int isMc_ ;  // -1: figure it out. 0 no, 1 yes.
	// --- specific fuctions
	int isSkim(){return isSkim_;}
	//
	// --- Handle
	
	// from the skim
	edm::Handle<std::vector<long> > events_handle;
	edm::Handle<std::vector<float> > weights_handle;
	edm::Handle<std::vector<int> > putrue_handle;
	// from PAT-MINIAOD
        edm::EDGetTokenT<GenEventInfoProduct> info_token;
	edm::Handle<GenEventInfoProduct> info_handle;

	edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pu_token;
	edm::Handle< std::vector<PileupSummaryInfo> > pu_handle;

	// --- Token


};


#endif
