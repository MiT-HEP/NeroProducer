#include "NeroProducer/Nero/interface/NeroTrigger.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

#include "DataFormats/HLTReco/interface/TriggerTypeDefs.h"

#define VERBOSE 0 

NeroTrigger::NeroTrigger(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
        BareTrigger()
{
    leps_=NULL;
    jets_=NULL;
    taus_=NULL;
    photons_=NULL;

    token = cc.consumes< edm::TriggerResults >( iConfig.getParameter<edm::InputTag>("trigger"));
    prescale_token = cc.consumes<pat::PackedTriggerPrescales>( edm::InputTag("patTrigger") );
    object_token = cc.consumes< pat::TriggerObjectStandAloneCollection > ( iConfig.getParameter<edm::InputTag> ("objects") );
    token_l1EtSum= cc.consumes< BXVector< l1t::EtSum > > (edm::InputTag("caloStage2Digis:EtSum"));
    mNMatch = iConfig.getParameter<int>("triggerNMatch");
    *(triggerNames) =  iConfig.getParameter<std::vector<std::string> > ("triggerNames");

    cout<<"[NeroTrigger]::[NeroTrigger]::[WARNING] Trigger matching based on object and best DR. No official tools yet."<<endl;
}

NeroTrigger::~NeroTrigger(){
}



int NeroTrigger::analyze(const edm::Event& iEvent){

    iEvent.getByToken(token,handle);
    iEvent.getByToken(prescale_token,prescale_handle);

    if ( not handle.isValid() ) cout<<"[NeroTrigger]::[analyze]::[ERROR] handle is not valid"<<endl;
    if ( not prescale_handle.isValid() ) cout<<"[NeroTrigger]::[analyze]::[ERROR] prescale_handle is not valid"<<endl;

    const pat::helper::TriggerMatchHelper matchHelper; 

    triggerFired   ->resize( triggerNames->size(),0);
    triggerPrescale->resize( triggerNames->size(),0);

    const edm::TriggerNames &names = iEvent.triggerNames(*handle);

    if ( handle.isValid() and not handle.failedToGet() ) {
        int ntrigs = (int)names.size();
        for (int i = 0; i < ntrigs; i++) {

            if (VERBOSE > 2){
            std::cout << "Trigger " << names.triggerName(i) << 
                ", prescale " << prescale_handle->getPrescaleForIndex(i) <<
                ": " << (handle->accept(i) ? "PASS" : "fail (or not run)") 
                      << std::endl;
            }
            string name = names.triggerName(i);
            for(unsigned int j=0;j< triggerNames->size() ;++j) // destination loop
            {
                if (name.find( (*triggerNames)[j]) != string::npos)
                {
                    if (handle->accept(i)){
                        ((*triggerFired)[j]) = 1;
                    }
                    (*triggerPrescale)[j] = prescale_handle -> getPrescaleForIndex(i) ; // TODO: what if already set ? 
                }

            } // my trigger end
        } // trigger loop
    } // handle is valid

    // L1
    iEvent.getByToken(token_l1EtSum,handle_l1EtSum);

    if ( not handle_l1EtSum.isValid() ) {
        static int counter =0 ;
        if (counter <10 ){
            counter++;
            cout<<"[NeroTrigger]::[analyze]::[ERROR] handle for l1 Et Sum is not valid"<<endl;
        }
    }

    if (handle_l1EtSum.isValid() and not handle_l1EtSum.failedToGet()){
        int bx=0; // we want only the bx =0 
        size_t size = handle_l1EtSum->size(bx);
        for(size_t trigger =0; trigger< size;++trigger){
            const auto& l1obj = handle_l1EtSum->at(bx,trigger);
            // see https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_X/DataFormats/L1Trigger/interface/EtSum.h#L20-L42
            if (l1obj.getType() == l1t::EtSum::kMissingEt){
                //cout <<"[NeroTrigger]::[analyze]::[INFO] l1 Et miss is "<<l1obj.pt()<<endl;
                if (l1obj.pt() >80) setTrigger("L1_ET80",1); // this will fill the correct position if it exists
                if (l1obj.pt() >90) setTrigger("L1_ET90",1);
                if (l1obj.pt() >100) setTrigger("L1_ET100",1);
            }
        }
    }


    // ---- TRIGGER MATCHING ---
    if (leps_ !=NULL) triggerLeps -> resize(leps_ -> p4 -> GetEntries()  ,0);
    if (jets_ !=NULL) triggerJets -> resize(jets_ -> p4 -> GetEntries()  ,0);
    if (taus_ !=NULL) triggerTaus -> resize(taus_ -> p4 -> GetEntries()  ,0);
    if (taus_ !=NULL) triggerNoneTaus -> resize(taus_ -> p4 -> GetEntries()  ,0);
    if (photons_ !=NULL) triggerPhotons -> resize(photons_ -> p4 -> GetEntries()  ,0);


    iEvent.getByToken(object_token,object_handle);
    if ( not object_handle.isValid() ) cout<<"[NeroTrigger]::[analyze]::[ERROR] object_handle is not valid"<<endl;

    for (pat::TriggerObjectStandAlone obj : *object_handle) { 
        obj.unpackPathNames(names);

        //bool isPhoton  =false;
        //bool isElectron=false;
        bool isMuon    =false;
        bool isTau     =false;
        //bool isMet     =false;
        bool isJet     =false;
        bool isBJet    =false;
        bool isEGamma  =false;

        for (unsigned h = 0; h < obj.filterIds().size(); ++h)  // can be more than one
        {
            // HLT: DataFormats/HLTReco/interface/TriggerTypeDefs.h
            if      ( obj.filterIds()[h] == trigger::TriggerPhoton ) isEGamma=true;//isPhoton=true;
            else if ( obj.filterIds()[h] == trigger::TriggerElectron) isEGamma=true;//isElectron=true;
            else if ( obj.filterIds()[h] == trigger::TriggerMuon ) isMuon=true;
            else if ( obj.filterIds()[h] == trigger::TriggerTau ) isTau=true;
            else if ( obj.filterIds()[h] == trigger::TriggerJet ) isJet=true;
            else if ( obj.filterIds()[h] == trigger::TriggerBJet ) isBJet=true;
            //else if ( obj.filterIds()[h] == trigger::TriggerMET ) isMet=true;
        }
        std::vector<std::string> pathNamesAll  = obj.pathNames(false);
        std::vector<std::string> pathNamesLast = obj.pathNames(true);

        // match : if more than one ? best
        if(VERBOSE)cout<<"[NeroTrigger]::[analize] Matching"<<endl;

        int muonIndex = -1 ; if( isMuon ) muonIndex = match(leps_,obj,13);

        int eleIndex=-1 ; if (isEGamma) eleIndex = match(leps_,obj,11); 

        int jetIndex=-1; if (isJet or isBJet) jetIndex = match(jets_,obj);

        int tauIndex=-1; if(isTau) tauIndex = match(taus_,obj);

        int photonIndex=-1; if (isEGamma) photonIndex = match(photons_,obj);
        //
        // Print all trigger paths, for each one record also if the object is associated to a 'l3' filter (always true for the
        // definition used in the PAT trigger producer) and if it's associated to the last filter of a successfull path (which
        // means that this object did cause this trigger to succeed; however, it doesn't work on some multi-object triggers)
        if(VERBOSE)cout<<"[NeroTrigger]::[analize] Lopping on path names and filling"<<endl;
        for (unsigned h = 0, n = pathNamesAll.size(); h < n; ++h) {
            if(VERBOSE)cout<<"[NeroTrigger]::[analize] pathNames h="<<h<<endl;
            //bool isBoth = obj.hasPathName( pathNamesAll[h], true, true ); 
            bool isL3   = obj.hasPathName( pathNamesAll[h], false, true ); 
            bool isLF   = obj.hasPathName( pathNamesAll[h], true, false ); 
            //bool isNone = obj.hasPathName( pathNamesAll[h], false, false ); 
            //std::cout << "   " << pathNamesAll[h];
            //if (isNone && !isBoth && !isL3 && !isLF) continue;

            for ( unsigned i =0  ; i<triggerNames->size() and i < mNMatch ;++i)
            {
                if(VERBOSE)cout<<"[NeroTrigger]::[analize] triggerNames i="<<i<<endl;
                if (  pathNamesAll[h].find((*triggerNames)[i] ) !=string::npos )
                {
                    if(VERBOSE) cout <<"Considering SUCCESS Path: eleIndex"<< eleIndex <<"/"<<triggerLeps->size()<<" :"<<pathNamesAll[h]<<" and Match string "<<(*triggerNames)[i]<<endl;
                    if(VERBOSE)cout<<"[NeroTrigger]::[analize] ----- Mu "<<muonIndex<<endl;
                    if (muonIndex >=0 and (isL3 or isLF) ) (*triggerLeps)[muonIndex] |= 1<<i;
                    if(VERBOSE)cout<<"[NeroTrigger]::[analize] ----- E "<<eleIndex<<endl;
                    if (eleIndex >=0 and (isL3 or isLF) ) (*triggerLeps)[eleIndex] |= 1<<i;
                    if(VERBOSE)cout<<"[NeroTrigger]::[analize] ----- Tau "<<tauIndex <<endl;
                    if (tauIndex >=0 and (isL3 or isLF) ) (*triggerTaus)[tauIndex] |= 1<<i;
                    if(VERBOSE)cout<<"[NeroTrigger]::[analize] ----- Jet "<< jetIndex<<endl;
                    if (jetIndex >=0 and (isL3 or isLF) ) (*triggerJets)[jetIndex] |= 1<<i;
                    if(VERBOSE)cout<<"[NeroTrigger]::[analize] ----- Phot "<<photonIndex<<endl;
                    if (photonIndex >=0 and (isL3 or isLF) ) (*triggerPhotons)[photonIndex] |= 1<<i;
                    if(VERBOSE)cout<<"[NeroTrigger]::[analize] ------ Tau None"<<endl;
                    if (tauIndex >=0 ) (*triggerNoneTaus)[tauIndex] |= 1<<i;
                    if(VERBOSE)cout<<"[NeroTrigger]::[analize] ----------"<<endl;
                }
            } // end loop for in the trigger names I'm interested in
        } //end lop pathNames in trigg obj
    } // end trigger objects loop
    if(VERBOSE)cout<<"[NeroTrigger]::[analize] === RETURN ==="<<endl;

    return 0;
}

void NeroTrigger::setTrigger(const string& name, int value){

    // reverse, usually L1 is the last
    for (size_t pos = triggerNames->size(); pos>0 ;--pos)  // offset by 1
    { 
        if (triggerNames->at(pos-1) != name) continue;
        (*triggerFired)[pos-1] =1;
        break;// spead up
    }

}

NEROREGISTER(NeroTrigger);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
