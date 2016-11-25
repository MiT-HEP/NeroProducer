#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

//#define VERBOSE 1
#ifdef VERBOSE
    #include "NeroProducer/Core/interface/BareFunctions.hpp"
#endif

NeroEvent::NeroEvent():
    NeroCollection(),
    BareEvent()
{
    metNameToBit["FullRecommendation"] = FullRecommendation;
    metNameToBit["Flag_HBHENoiseFilter"] = HBHENoiseFilter;
    metNameToBit["Flag_HBHENoiseIsoFilter"] = HBHENoiseIsoFilter;
    metNameToBit["Flag_EcalDeadCellTriggerPrimitiveFilter"] = EcalDeadCellTriggerPrimitiveFilter;
    metNameToBit["Flag_goodVertices"] = goodVertices;
    metNameToBit["Flag_eeBadScFilter" ] = eeBadScFilter;
    metNameToBit["Flag_globalTightHalo2016Filter" ] = GlobalTightHalo2016;
}

NeroEvent::~NeroEvent(){
}

int NeroEvent::analyze(const edm::Event& iEvent){
    isRealData = iEvent.isRealData() ? 1 : 0 ;
    runNum     = iEvent.id().run();
    lumiNum    = iEvent.luminosityBlock();
    eventNum   = iEvent.id().event();

    if ( mOnlyMc  ) return 0;
    iEvent.getByToken(rho_token,rho_handle);
    if ( not rho_handle.isValid() ) cout <<"[NeroEvent]::[analyze]::[ERROR] rho_handle is not valid"<<endl;

    rho 	   =  *rho_handle;

    // Implement MET Filters
    //if (isRealData) {
    edm::Handle< bool > ifilterbadChCand;
    iEvent.getByToken(BadChCandFilter_token, ifilterbadChCand);
    filterbadChCandidate = *ifilterbadChCand;       
    
    edm::Handle< bool > ifilterbadPFMuon;
    iEvent.getByToken(BadPFMuon_token, ifilterbadPFMuon);
    filterbadPFMuon = *ifilterbadPFMuon;

    edm::Handle < edm::TriggerResults > metFiltersResults;
    iEvent.getByToken(filter_token, metFiltersResults);
    const edm::TriggerNames &names = iEvent.triggerNames(*metFiltersResults);

    unsigned int filter=0;

    if ( metFiltersResults.isValid() and not metFiltersResults.failedToGet() ) {

        //std::auto_ptr<std::vector<bool> > metFilters(new std::vector<bool>() );
        std::auto_ptr<bool> passesMETFilters(new bool(true));

        for ( unsigned int i = 0; i < names.size(); ++i) {            
            if ( std::find( metfilterNames->begin(), metfilterNames->end(), names.triggerName(i) ) != metfilterNames->end() ) {

                *passesMETFilters = *passesMETFilters && metFiltersResults->accept( i );
                //metFilters->push_back( metFiltersResults->accept( i ) );
                unsigned bitflag = Unknown;
                const auto& it = metNameToBit.find(names.triggerName(i) );
                if ( it != metNameToBit.end()) bitflag = it->second;
                else { 
                    std::cout<<"[NeroEvent]::[analyze]::[ERROR] trigger Name in cfg "<< names.triggerName(i) <<" has no selection bit associated. Will not be saved"<<std::endl;
                }
                filter |= metFiltersResults->accept(i) * bitflag;

#ifdef VERBOSE
                if(VERBOSE>0)std::cout << "MetFilter : " << names.triggerName(i) << " is registered " << metFiltersResults->accept(i) << std::endl;
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

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
