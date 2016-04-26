#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroEvent::NeroEvent():
        NeroCollection(),
        BareEvent()
{
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
    if (isRealData) {
        
        edm::Handle < edm::TriggerResults > metFiltersResults;
        iEvent.getByToken(filter_token, metFiltersResults);
        const edm::TriggerNames &names = iEvent.triggerNames(*metFiltersResults);

        if ( metFiltersResults.isValid() and not metFiltersResults.failedToGet() ) {

            std::auto_ptr<std::vector<bool> > metFilters(new std::vector<bool>() );
            std::auto_ptr<bool> passesMETFilters(new bool(true));
            
            for ( unsigned int i = 0; i < names.size(); ++i) {            
                if ( std::find( metfilterNames->begin(), metfilterNames->end(), names.triggerName(i) ) != metfilterNames->end() ) {
                    
                    *passesMETFilters = *passesMETFilters && metFiltersResults->accept( i );
                    metFilters->push_back( metFiltersResults->accept( i ) );
                    std::cout << "MetFilter : " << names.triggerName(i) << " is registered " << metFiltersResults->accept(i) << std::endl;
                    
                }
            }


            unsigned int filter=0;

            filter |= ((*passesMETFilters) * FullRecommendation);
            filter |= ((*metFilters)[0] * HBHENoiseFilter) ;
            filter |= ((*metFilters)[1] * HBHENoiseIsoFilter) ;
            filter |= ((*metFilters)[2] * CSCTightHalo2015Filter) ;
            filter |= ((*metFilters)[3] * EcalDeadCellTriggerPrimitiveFilter) ;
            filter |= ((*metFilters)[4] * goodVertices) ;
            filter |= ((*metFilters)[5] * eeBadScFilter) ;

            selBits = filter;                

            metFilters->clear();            
        }

    }



    if (IsExtend() ) {

        iEvent.getByToken( originalRun_token , originalRun_handle); 
        iEvent.getByToken( originalLumi_token , originalLumi_handle); 
        iEvent.getByToken( originalEvent_token , originalEvent_handle); 

        // -- these are under the 'isValid' condition, so program will not crash if not present
        if( originalRun_handle.isValid() ) originalRun = *originalRun_handle;
        if( originalLumi_handle.isValid()) originalLumi = *originalLumi_handle;
        if( originalEvent_handle.isValid()) originalEvent = *originalEvent_handle;
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
