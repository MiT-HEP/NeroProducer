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
        NeroAll():NeroCollection(){}
        NeroAll(edm::ConsumesCollector & cc,edm::ParameterSet iConfig );
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

        edm::EDGetTokenT<std::vector<long> > events_token;
        edm::EDGetTokenT<std::vector<float> > weights_token;
        edm::EDGetTokenT<std::vector<int> > putrue_token;

        // from PAT-MINIAOD
        // needs to be checked. Not using it at the moment
        edm::EDGetTokenT<GenEventInfoProduct> info_token;
        edm::Handle<GenEventInfoProduct> info_handle;

        edm::Handle<LHEEventProduct> lhe_handle;
        edm::EDGetTokenT<LHEEventProduct> lhe_token;

        edm::EDGetTokenT<std::vector<PileupSummaryInfo> > pu_token;
        edm::Handle< std::vector<PileupSummaryInfo> > pu_handle;


        // --- Token
        // this are constructed by tFileservice at beginJob
        TH1D * hDEvents{0};
        TH1D * hDTotalMcWeight{0};
        TH1D * hDPileup{0};
        TH1D * hDscaleReweightSums{0};
        TH1D * hDpdfReweightSums{0};

};


#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
