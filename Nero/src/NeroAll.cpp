#include "NeroProducer/Nero/interface/NeroAll.hpp"
#define VERBOSE 0

NeroAll::NeroAll():
        NeroCollection(),
        BareAll(),
        NeroLumi()
{
    isSkim_=1;
    isMc_=-1;
}
NeroAll::~NeroAll(){}

int NeroAll::analyze(const edm::Event&iEvent)
{
    if(VERBOSE>0) cout<<"[NeroAll]::[analyze]::[DEBUG] figure out if it is MC"<<endl;

    if(isMc_ <0 )
    {
        isMc_ = ( iEvent.isRealData() ) ? 0 : 1;
        isRealData = ( iEvent.isRealData() ) ? 1 : 0;
    }
    
    if(VERBOSE>1) cout<<"[NeroAll]::[analyze]::[DEBUG] isMc_"<<isMc_<<endl;

    // FILL the  histograms on the fly, TODO move in the skim ?!? SKIM will still be big if this is kept.
    //
    //I suppose to have a info_handle
    iEvent.getByToken(info_token,info_handle);
    iEvent.getByToken(lhe_token,lhe_handle);

    if(VERBOSE>0) { if (lhe_handle.isValid()) cout<<"[NeroAll]::[analyze]::[DEBUG] LHE Handle is valid. SIZE="<<lhe_handle->weights().size()<<endl;
        else cout<<"[NeroAll]::[analyze]::[DEBUG] LHE Handle is NOT valid."<<endl;
    }
    //---  scale
    if (lhe_handle.isValid() and  lhe_handle->weights().size() >=9){
        hDscaleReweightSums -> Fill( 0+.5 , double(lhe_handle -> weights() . at(1) .wgt)) ;
        hDscaleReweightSums -> Fill( 1+.5 , double(lhe_handle -> weights() . at(2) .wgt)) ;
        hDscaleReweightSums -> Fill( 2+.5 , double(lhe_handle -> weights() . at(3) .wgt)) ;
        hDscaleReweightSums -> Fill( 3+.5 , double(lhe_handle -> weights() . at(4) .wgt)) ;
        hDscaleReweightSums -> Fill( 4+.5 , double(lhe_handle -> weights() . at(6) .wgt)) ;    
        hDscaleReweightSums -> Fill( 5+.5 , double(lhe_handle -> weights() . at(8) .wgt)) ;     
    }

    // ------ pdf
    if (lhe_handle.isValid() and  lhe_handle->weights().size() > 109)
        for( int pdfw = 9 ; pdfw<109 ;++pdfw)
        {
        hDpdfReweightSums -> Fill( pdfw -9 + .5, double(lhe_handle -> weights() . at(pdfw) .wgt));    
        }

    if( isSkim() == 0) 
    {
        //TODO FILL all_
        throw 5;
        isRealData = ( iEvent.isRealData() ) ? 1 : 0;	
        runNum = iEvent.id().run();
        lumiNum = iEvent.luminosityBlock(); 	
        eventNum = iEvent.id().event();  

        if( not isRealData ) {
            //iEvent.getByLabel(edm::InputTag("generator"), info_handle); // USE TOKEN AND INPUT TAG ?
            //iEvent.getByLabel(edm::InputTag("addPileupInfo"), pu_handle);
            //TODO! don't use this path for the moment. Check that double fetching the token still work
            //iEvent.getByToken(info_token,info_handle);
            iEvent.getByToken(pu_token,pu_handle );

            if (not info_handle.isValid() ) cout<<"[NeroAll]::[analyze]::[ERROR] Info handle is not valid"<<endl;
            if (not pu_handle.isValid() ) cout <<"[NeroAll]::[analyze]::[ERROR] PU handle is not valide"<<endl;
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

    //iLumi.getByLabel(edm::InputTag("InfoProducer","vecEvents"), events_handle);

    //iLumi.getByLabel(edm::InputTag("InfoProducer","vecMcWeights"), weights_handle);
    //iLumi.getByLabel(edm::InputTag("InfoProducer","vecPuTrueInt"), putrue_handle);

    iLumi.getByToken( events_token, events_handle);
    iLumi.getByToken( weights_token, weights_handle);
    iLumi.getByToken( putrue_token, putrue_handle);

    if ( not events_handle.isValid() ) cout<<"[NeroAll]::[analyzeLumi]::[ERROR] events_handle is not valid"<<endl;
    if ( not weights_handle.isValid() ) cout<<"[NeroAll]::[analyzeLumi]::[ERROR] weights_handle is not valid"<<endl;
    if ( not putrue_handle.isValid() ) cout<<"[NeroAll]::[analyzeLumi]::[ERROR] putrue_handle is not valid"<<endl;

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

        if(hDEvents) hDEvents->Fill(0.5);
        if(hDTotalMcWeight) hDTotalMcWeight->Fill(0.5,mcWeight);
        if(hDPileup) hDPileup->Fill(puTrueInt,1.0); // generation is independent
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
