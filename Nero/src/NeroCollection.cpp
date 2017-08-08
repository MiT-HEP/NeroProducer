#include "NeroProducer/Nero/interface/NeroCollection.hpp"
NeroCollection* NeroFactory::create(const string& name,edm::ConsumesCollector & cc, const edm::ParameterSet& iConfig)
{
    const auto& i = table_.find(name);
    if ( i == table_.end() ) { 
        std::cout<<"[NeroFactory]::[ERROR]::Unable to create: "<<name<<std::endl; 
        return (NeroCollection*)NULL ; 
    }
    return i -> second -> create(cc,iConfig);
};

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
