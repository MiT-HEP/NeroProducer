#include "NeroProducer/Nero/interface/NeroAll.hpp"

NeroAll::NeroAll(){isSkim_=1;isMc_=-1;}
NeroAll::~NeroAll(){}

int NeroAll::analyze(const edm::Event&iEvent)
{
if(isMc_ <0 )
	{
	isMc_ = ( iEvent.isRealData() ) ? 0 : 1;
	isRealData = ( iEvent.isRealData() ) ? 1 : 0;
	}
if( isSkim() == 0) 
	{
	isRealData = ( iEvent.isRealData() ) ? 1 : 0;	
	runNum = iEvent.id().run();
	lumiNum = iEvent.luminosityBlock(); 	
	eventNum = iEvent.id().event();  
	iEvent.getByLabel(edm::InputTag("generator"), info_handle); // USE TOKEN AND INPUT TAG ?
	iEvent.getByLabel(edm::InputTag("addPileupInfo"), pu_handle);
	mcWeight = info_handle->weight();
	//PU
        puTrueInt = 0;
        for(const auto & pu : *pu_handle)
                {
                //Intime
                if (pu.getBunchCrossing() == 0)
                        puTrueInt += pu.getTrueNumInteractions();
                //Out-of-time
                }
	}
	return 0;
}

int NeroAll::analyzeLumi(const edm::LuminosityBlock &iLumi, TTree *t)
{
	if (isSkim() <= 0) return 0;

	iLumi.getByLabel(edm::InputTag("InfoProducer","vecEvents"), events_handle);
	if (isMc_){
		iLumi.getByLabel(edm::InputTag("InfoProducer","vecMcWeights"), weights_handle);
		iLumi.getByLabel(edm::InputTag("InfoProducer","vecPuTrueInt"), putrue_handle);
	}

	for( unsigned int iE = 0 ; iE < events_handle->size() ;++iE)
	{
	clear();
	isRealData = 1;
	eventNum = events_handle->at(iE);
	lumiNum = iLumi.id().luminosityBlock();
	runNum = iLumi.id().run();
	if (isMc_){
		isRealData = 0;
		mcWeight = weights_handle->at(iE);
		puTrueInt = putrue_handle->at(iE);
	}

	t->Fill();
	}
	return 0;
}
