#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

//#define VERBOSE 1
#ifdef VERBOSE
    #include "NeroProducer/Core/interface/BareFunctions.hpp"
#endif


NeroEvent::NeroEvent(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
    BareEvent()
{
    //* init maps
    metNameToBit["FullRecommendation"] = FullRecommendation;
    metNameToBit["Flag_HBHENoiseFilter"] = HBHENoiseFilter;
    metNameToBit["Flag_HBHENoiseIsoFilter"] = HBHENoiseIsoFilter;
    metNameToBit["Flag_EcalDeadCellTriggerPrimitiveFilter"] = EcalDeadCellTriggerPrimitiveFilter;
    metNameToBit["Flag_goodVertices"] = goodVertices;
    metNameToBit["Flag_eeBadScFilter" ] = eeBadScFilter;
    metNameToBit["Flag_globalTightHalo2016Filter" ] = GlobalTightHalo2016;
    metNameToBit["Flag_globalSuperTightHalo2016Filter" ] = GlobalSuperTightHalo2016;
    metNameToBit["Flag_BadPFMuonFilter" ] = BadPFMuonFilter;
    metNameToBit["Flag_BadChargedCandidateFilter"] = BadChargedCandidateFilter;
    metNameToBit["Flag_ecalBadCalibFilter"] = ecalBadCalibFilter;

    // init tokens
    rho_token       = cc.consumes<double>(iConfig.getParameter<edm::InputTag>("rho"));
    filter_token    = cc.consumes<edm::TriggerResults>(iConfig.getParameter < edm::InputTag > ("metFilterToken"));
    *( metfilterNames) = iConfig.getParameter < std::vector<std::string> > ("metfilterNames");

    // bad filters
    ecalBadCalibFilterUpdate_token= cc.consumes<bool>(edm::InputTag("ecalBadCalibReducedMINIAODFilter"));

}

NeroEvent::~NeroEvent(){
}

int NeroEvent::analyze(const edm::Event& iEvent){
    isRealData = iEvent.isRealData() ? 1 : 0 ;
    runNum     = iEvent.id().run();
    lumiNum    = iEvent.luminosityBlock();
    eventNum   = iEvent.id().event();

    iEvent.getByToken(rho_token,rho_handle);
    if ( not rho_handle.isValid() ) cout <<"[NeroEvent]::[analyze]::[ERROR] rho_handle is not valid"<<endl;

    rho 	   =  *rho_handle;

    // Implement MET Filters
    
    edm::Handle < edm::TriggerResults > metFiltersResults;
    iEvent.getByToken(filter_token, metFiltersResults);
    const edm::TriggerNames &names = iEvent.triggerNames(*metFiltersResults);

    unsigned int filter=0;

    // override ECAL Bad Filters
    edm::Handle < bool > passecalBadCalibFilterUpdate ;
    iEvent.getByToken(ecalBadCalibFilterUpdate_token,passecalBadCalibFilterUpdate);
    if (not passecalBadCalibFilterUpdate.isValid()) cout<<"[NeroEvents]::[ERROR] EcalBadFilterUpdate not valid"<<std::endl;
    bool    _passecalBadCalibFilterUpdate =  (*passecalBadCalibFilterUpdate );


    if ( metFiltersResults.isValid() and not metFiltersResults.failedToGet() ) {

        //std::auto_ptr<std::vector<bool> > metFilters(new std::vector<bool>() );
        std::auto_ptr<bool> passesMETFilters(new bool(true));

        for ( unsigned int i = 0; i < names.size(); ++i) {            
            if ( std::find( metfilterNames->begin(), metfilterNames->end(), names.triggerName(i) ) != metfilterNames->end() ) {
                bool passThisFilter=metFiltersResults->accept( i );
                if (names.triggerName(i) == "Flag_ecalBadCalibFilter")
                    passThisFilter=_passecalBadCalibFilterUpdate;

                if (names.triggerName(i) != "Flag_eeBadScFilter" and not isRealData)                
                {
                    //*passesMETFilters = *passesMETFilters && metFiltersResults->accept( i );
                    *passesMETFilters = *passesMETFilters && passThisFilter;//metFiltersResults->accept( i );
                }
                unsigned bitflag = Unknown;
                const auto& it = metNameToBit.find(names.triggerName(i) );
                if ( it != metNameToBit.end()) bitflag = it->second;
                else { 
                    std::cout<<"[NeroEvent]::[analyze]::[ERROR] trigger Name in cfg "<< names.triggerName(i) <<" has no selection bit associated. Will not be saved"<<std::endl;
                }
                filter |= passThisFilter * bitflag;

#ifdef VERBOSE
                if(VERBOSE>0)std::cout << "MetFilter : " << names.triggerName(i) << " is registered " << passThisFilter << std::endl;
#endif

            }
        }



        filter |= ((*passesMETFilters) * FullRecommendation);

        selBits = filter;                

#ifdef VERBOSE
        if(VERBOSE>0)cout << "SelBits is:"<<selBits<<": "<<BareFunctions::printBinary(selBits)<<endl;
#endif

    }

    //} //end isRealData


    return 0;
}

NEROREGISTER(NeroEvent);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
