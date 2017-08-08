#ifndef NERO_COLLECTION_H
#define NERO_COLLECTION_H

#include "NeroProducer/Nero/interface/Includes.hpp"
#include "NeroProducer/Core/interface/BareCollection.hpp"
#include "NeroProducer/Nero/interface/NeroLogger.hpp"
// user include files

#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TTree.h"


#include <vector>
using namespace std;

class NeroCollection : virtual public BareCollection
{
    public:
        NeroCollection():BareCollection(){ Logger::getInstance().Log( string("") +"Unable to construct w/o parameter set:" +  name(), Logger::ERROR); }
        NeroCollection(edm::ConsumesCollector & cc,const edm::ParameterSet& iConfig ):BareCollection(){}
        // --- Just virtual
        virtual int  analyze(const edm::Event &) = 0 ;
        // -- usually just need the iEvent. This one will be the one called
        virtual int  analyze(const edm::Event &iEvent,const edm::EventSetup& iSetup) { return analyze(iEvent) ; } ;
        virtual inline string name(){return "NeroCollection";};

};

// ----------------------------  factory ----------------
#include <map>
#include <iostream>
class NeroCreator;
class NeroFactory{
    private:
        NeroFactory(){} // private constructor
        NeroFactory(NeroFactory const&)  = delete; // avoid copies around
        void operator=(NeroFactory const&)  = delete; // avoid copies around
        std::map<std::string, NeroCreator*> table_;
    public:
        static NeroFactory& get(){static NeroFactory instance; return instance;}
        NeroCollection* create(const string& name,edm::ConsumesCollector & cc,const edm::ParameterSet& iConfig);
        void inline registerit(string name, NeroCreator *creator) { table_[name] = creator;}
};
class NeroCreator{
    private:
    public:
        virtual NeroCollection* create(edm::ConsumesCollector & cc,const edm::ParameterSet& iConfig ) = 0;
        NeroCreator(const string& name){ std::cout<<"* Called creator for class: "<<name <<std::endl; NeroFactory::get() . registerit( name, this); }
};

template <typename T>
class NeroCreatorImpl : public NeroCreator
{
    public:
        NeroCreatorImpl(const string &name) : NeroCreator(name) {}
        NeroCollection* create(edm::ConsumesCollector & cc,const edm::ParameterSet& iConfig ) override final { return new T(cc,iConfig); } 
};


//Register macros
#define NEROREGISTER(classname)\
    namespace { \
        NeroCreatorImpl<classname> _creator_ (#classname); \
    };
// ----------------------------  factory ----------------

#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
