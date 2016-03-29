#include "NeroProducer/Core/interface/BareCollection.hpp"

BareCollection* BareFactory::create(const std::string & name)
{
    const auto& i = table_.find(name);
    if ( i == table_.end() ) { std::cout<<"[BareFactory]::[ERROR]::Unable to create: "<<name<<std::endl; return (BareCollection*)NULL ; }
    return i -> second -> create();
}   



// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 
