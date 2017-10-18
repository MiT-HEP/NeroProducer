// -*- C++ -*-
//
// Package:    NeroProducer/Nero
// Class:      Nero
// 
/**\class Nero Nero.cc NeroProducer/Nero/plugins/Nero.cc

Description: [one line class summary]

Implementation:
[Notes on implementation]
*/
//
// Original Author:  Andrea Carlo Marini
//         Created:  Tue, 19 May 2015 13:38:17 GMT
//
//

#include "NeroProducer/Nero/interface/Nero.hpp"

#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/NeroEvent.hpp"
#include "NeroProducer/Nero/interface/NeroFatJets.hpp"
#include "NeroProducer/Nero/interface/NeroVertex.hpp"
#include "NeroProducer/Nero/interface/NeroMet.hpp"
#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"
#include "NeroProducer/Nero/interface/NeroAll.hpp"
#include "NeroProducer/Nero/interface/NeroTrigger.hpp"

#define NERO_VERBOSE 2
//#define NERO_VERBOSE 1

//
// constructors and destructor
//
Nero::Nero(const edm::ParameterSet& iConfig) 

{
    #ifdef NERO_VERBOSE
    if (NERO_VERBOSE>0) std::cout<<"[Nero]::[Nero]: Begin constructor"<<std::endl;
    #endif

    tag_  = iConfig.getParameter<string>("tag");
    head_ = iConfig.getParameter<string>("head");
    info_ = iConfig.getParameter<string>("info");
    cmssw_ = iConfig.getParameter<string>("cmssw");

    auto&& cc ( consumesCollector() );


    std::vector<string> objName{"NeroAll","NeroEvent","NeroVertex","NeroFatJets","NeroTaus","NeroLeptons","NeroMet","NeroMonteCarlo","NeroJets","NeroPhotons","NeroTrigger"};


    for (const auto& name : objName)
    {
        #ifdef NERO_VERBOSE
            if (NERO_VERBOSE>1) std::cout<<"[Nero]::[Nero]: Doing: "<< name<<std::endl;
        #endif
        Logger::getInstance().Log("Constructing obj: " + name,2) ;
        NeroCollection *o = NeroFactory::get().create( name, cc, iConfig.getParameterSet(name) );
        if (o==NULL)  Logger::getInstance().Log("Object '" + name+"' unable to construct",0) ;
        obj.push_back(o);
        //if (name=="NeroMet") runObj.push_back(dynamic_cast<NeroRun*>(o));
        if (name=="NeroAll") lumiObj.push_back(dynamic_cast<NeroLumi*>(o));
        if (name == "NeroTrigger") {
            NeroTrigger *tr = dynamic_cast<NeroTrigger*>(o); // TODO what if these are not in the list?
            #ifdef NERO_VERBOSE
            if (NERO_VERBOSE >0){
                cout<<"[Nero]::[Nero]::[Nero Trigger construction] size:" << obj.size()<<" | "<<objName.size()<<endl;
                cout<<"[Nero]::[Nero]::[Nero Trigger construction] Lep: pos=" << std::find (objName.begin(),objName.end(),"NeroLeptons")-objName.begin()<<endl;
                cout<<"[Nero]::[Nero]::[Nero Trigger construction] Jet: pos=" << std::find (objName.begin(),objName.end(),"NeroJets")-objName.begin()<<endl;
                cout<<"[Nero]::[Nero]::[Nero Trigger construction] Tau: pos=" << std::find (objName.begin(),objName.end(),"NeroTaus")-objName.begin()<<endl;
                cout<<"[Nero]::[Nero]::[Nero Trigger construction] Pho: pos=" << std::find (objName.begin(),objName.end(),"NeroPhotons")-objName.begin()<<endl;
            }
            #endif 
            tr -> leps_    = dynamic_cast<NeroLeptons*>( obj[std::find (objName.begin(),objName.end(),"NeroLeptons")-objName.begin()] );
            tr -> jets_    = dynamic_cast<NeroJets*>   ( obj[std::find (objName.begin(),objName.end(),"NeroJets")-objName.begin()] ) ;
            tr -> taus_    = dynamic_cast<NeroTaus*>   ( obj[std::find (objName.begin(),objName.end(),"NeroTaus")-objName.begin()] );
            tr -> photons_ = dynamic_cast<NeroPhotons*>( obj[std::find (objName.begin(),objName.end(),"NeroPhotons")-objName.begin()] );
        }
    }


    // set the collection it needs to be awared of
    // this step is needed in order to insure trigger matching is done on the saved collection
    //
    #ifdef NERO_VERBOSE
    if (NERO_VERBOSE>0) std::cout<<"[Nero]::[Nero]: end constructor"<<std::endl;
    #endif

}


Nero::~Nero()
{

    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
    // for(unsigned int i=0;i< obj.size();++i)
    // {
    //        delete obj[i];
    // }
    // obj.clear();

}


//
// member functions
//

// ------------ method called for each event  ------------
    void
Nero::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
    using namespace edm;
    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- begin event --------"<<endl;
    #endif

    for(auto o : obj)
        o->clear();

    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- analyze event --------"<<endl;
    #endif

    for(auto o : obj)
    {
        #ifdef NERO_VERBOSE
            if (NERO_VERBOSE > 1) { cout<<"[Nero]::[analyze] going to analyze "<<o->name() <<endl; }
            if (NERO_VERBOSE){sw_.Reset(); sw_.Start();}
        #endif

        if (o->analyze(iEvent, iSetup) ) return; // analyze return 0 on success (VTX ..)

        #ifdef NERO_VERBOSE
            if (NERO_VERBOSE)
            {
                sw_.Stop(); 
                times_[o->name()] += sw_.CpuTime() ;
            }
            if(NERO_VERBOSE>1)
            {
                cout<< "[Nero]::[analyze] object "<<o->name()
                    <<" took:"<< sw_.CpuTime()<< "CPU Time and "
                    <<sw_.RealTime()<<"RealTime"<<endl;
            }
        #endif
    }

    // compress double precision to float precision
    for(auto o : obj)
    {
        o->compress();
    }

    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- fill event --------"<<endl;
    #endif

    tree_->Fill();
    #ifdef NERO_VERBOSE
        if (NERO_VERBOSE>1) cout<<"------- end event (success) --------"<<endl;
        if (NERO_VERBOSE){
            times_[ "counter" ] +=1;
            if(times_[ "counter"] > 3000 )
            {
                cout<< " --- CPU TIMES ----" <<endl;
                for(auto &x : times_)
                {
                    cout << x.first <<": "<<x.second<<endl;
                    x.second = 0;
                }
                cout<< " ------------" <<endl;
                times_[ "counter"] = 0;
            }
        } // end NERO_VERBOSE
    #endif

}


// ------------ method called once each job just before starting event loop  ------------
    void 
Nero::beginJob()
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" >>>>>>> BEGIN JOB <<<<<<<<<"<<endl;
    #endif

    tree_    = fileService_ -> make<TTree>("events", "events");
    all_     = fileService_ -> make<TTree>("all", "all");	  
    hXsec_   = fileService_ -> make<TH1F>("xSec", "xSec",20,-0.5,19.5); hXsec_ ->Sumw2();

    hD_["Events"]   = fileService_ -> make<TH1D>("hDEvents", "hDEvents",1,0,1); hD_["Events"] ->Sumw2();
    hD_["TotalMcWeight"]   = fileService_ -> make<TH1D>("hDTotalMcWeight", "hDTotalMcWeight",1,0,1); hD_["TotalMcWeight"] ->Sumw2();
    hD_["Pileup"]   = fileService_ -> make<TH1D>("hDPileup", "hDPileup",100,0,100); hD_["Pileup"] ->Sumw2();

    hD_["scaleReweightSums"]  = fileService_ -> make<TH1D>("scaleReweightSums", "", 6, 0., 6.);

    hD_["scaleReweightSums"]->Sumw2();
    int iBin = 0;
    for (TString label : {"r1f2", "r1f5", "r2f1", "r2f2", "r5f1", "r5f5"})
        hD_["scaleReweightSums"]->GetXaxis()->SetBinLabel(++iBin, label);

    hD_["pdfReweightSums"] = fileService_ -> make<TH1D> ("pdfReweightSums","", 500,0.,500.);

    fileService_ ->make<TNamed>("tag",tag_.c_str() );
    fileService_ ->make<TNamed>("head",head_.c_str() );
    fileService_ ->make<TNamed>("info",info_.c_str() );
    fileService_ ->make<TNamed>("cmssw",cmssw_.c_str() );

    // FILL TRIGGER NAMES INFO
    string myString = "";
    for(auto o : obj)
        if (dynamic_cast<NeroTrigger*>(o) != NULL) 
        {
            NeroTrigger* tr = dynamic_cast<NeroTrigger*>(o) ;
            for( string n : *tr->triggerNames)
                myString +=  n + ",";
        }

    fileService_ -> make<TNamed>("triggerNames",myString.c_str());

    // define branches
    for(auto o : obj)
    {
        if (dynamic_cast<NeroAll*> (o) !=NULL ) { continue ; }  // otherwise I will have also the branch in the main tree.
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE) cout<<" -> Calling defineBranch for "<<o->name()<<endl;
        #endif
        o -> defineBranches(tree_);
    }

    for(auto o : lumiObj)
    {
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE) cout<<" -> Calling defineBranch for "<<o->name()<<endl;
        #endif
        o -> defineBranches(all_);
    }
    
    //set histogram map
    for(auto o : lumiObj){
        if (dynamic_cast<NeroAll*> (o) !=NULL ) {
            #ifdef NERO_VERBOSE
                if(NERO_VERBOSE) cout<<" -> Setting Histograms in "<<o->name()<<endl;
            #endif
            NeroAll * na = dynamic_cast<NeroAll*> (o);
            na->hDEvents =  hD_["Events"];
            na->hDTotalMcWeight = hD_["TotalMcWeight"];
            na->hDPileup = hD_["Pileup"];
            na->hDscaleReweightSums = hD_["scaleReweightSums"];
            na->hDpdfReweightSums = hD_["pdfReweightSums"];
        } // end if
        } // end for o in lumiObj
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" -- end of begin job "<<endl;
    #endif
} //end beginJob


// ------------ method called once each job just after ending the event loop  ------------
    void 
Nero::endJob() 
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" >>>>>>> END JOB <<<<<<<<<"<<endl;
    #endif
}

// ------------ method called when starting to processes a run  ------------

    void 
Nero::beginRun(edm::Run const&iRun, edm::EventSetup const&)
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout<<" ======= BEGIN RUN ======="<<endl;
    #endif
}


// ------------ method called when ending the processing of a run  ------------

    void 
Nero::endRun(edm::Run const&iRun, edm::EventSetup const&iSetup)
{ // this should be finalized at the end of the run
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout <<" ---- end run macros --- "<<endl;
    #endif

    for(auto o : runObj )
    {
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE> 1) cout<<"[Nero]::[endRun]::[DEBUG] calling object"<<o->name()<<endl;
        #endif
        o->analyzeRun(iRun, hXsec_);
    }
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout <<" ======== END RUN ======="<<endl;
    #endif

}


// ------------ method called when starting to processes a luminosity block  ------------

    void 
Nero::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout <<" -------- BEGIN LUMI --------"<<endl;
    #endif

}


// ------------ method called when ending the processing of a luminosity block  ------------

    void 
Nero::endLuminosityBlock(edm::LuminosityBlock const&iLumi, edm::EventSetup const&)
{
    for(auto o :lumiObj)
    {
        #ifdef NERO_VERBOSE
            if(NERO_VERBOSE>1) cout<<"[Nero]::[endLuminosityBlock]::[DEBUG] calling object"<<o->name()<<endl;
        #endif

        o->analyzeLumi(iLumi,all_);
    }

    #ifdef NERO_VERBOSE
        if(NERO_VERBOSE) cout <<" -------- END LUMI --------"<<endl;
    #endif
}


// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
Nero::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Nero);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
