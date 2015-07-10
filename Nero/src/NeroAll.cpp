#include "NeroProducer/Nero/interface/NeroAll.hpp"
#define VERBOSE 2

NeroAll::NeroAll(){isSkim_=1;isMc_=-1;}
NeroAll::~NeroAll(){}

int NeroAll::analyze(const edm::Event&iEvent)
{
    if(VERBOSE>1) cout<<"[NeroAll]::[analyze]::[DEBUG] figure out if it is MC"<<endl;

    if(isMc_ <0 )
    {
        isMc_ = ( iEvent.isRealData() ) ? 0 : 1;
        isRealData = ( iEvent.isRealData() ) ? 1 : 0;
    }
    
    if(VERBOSE>1) cout<<"[NeroAll]::[analyze]::[DEBUG] isMc_"<<isMc_<<endl;

    if( isSkim() == 0) 
    {
        //TODO FILL all_
        isRealData = ( iEvent.isRealData() ) ? 1 : 0;	
        runNum = iEvent.id().run();
        lumiNum = iEvent.luminosityBlock(); 	
        eventNum = iEvent.id().event();  

        if( not isRealData ) {
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
    }
    return 0;
}

int NeroAll::analyzeLumi(const edm::LuminosityBlock &iLumi, TTree *t)
{
    if (isSkim() <= 0) return 0;

    if(VERBOSE>1) cout<<"[NeroAll]::[analyzeLumi]::[DEBUG] isMc_"<<isMc_<<endl;

    iLumi.getByLabel(edm::InputTag("InfoProducer","vecEvents"), events_handle);

    iLumi.getByLabel(edm::InputTag("InfoProducer","vecMcWeights"), weights_handle);
    iLumi.getByLabel(edm::InputTag("InfoProducer","vecPuTrueInt"), putrue_handle);

    if(isMc_<0) isMc_ = weights_handle->size() > 0 ; // if the n.events is 0, doesn't matter, because the loop will exit

    for( unsigned int iE = 0 ; iE < events_handle->size() ;++iE)
    {
        clear();
        isRealData = 1;
        eventNum = events_handle->at(iE);
        lumiNum = iLumi.id().luminosityBlock();
        runNum = iLumi.id().run();
        if (isMc_  ){
            isRealData = 0;
            mcWeight = weights_handle->at(iE);
            puTrueInt = putrue_handle->at(iE);
        }
        else{
            mcWeight=1; 
            puTrueInt=0;
        }

        if (VERBOSE) 
            cout<<"[NeroAll]::[analyzeLumi] Filling "
                <<"iE="<<iE
                << "| r:"<<runNum
                << "| l:"<<lumiNum
                << "| e:"<<eventNum
                << "| w:"<<mcWeight
                << "| p:"<<puTrueInt
                <<endl;
        t->Fill();
    }
    return 0;
}
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
