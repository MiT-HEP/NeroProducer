#ifndef NERO_TRIGGER_H
#define NERO_TRIGGER_H

#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Core/interface/BareTrigger.hpp"

#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/NeroLeptons.hpp"
#include "NeroProducer/Nero/interface/NeroTaus.hpp"
#include "NeroProducer/Nero/interface/NeroPhotons.hpp"
#include "DataFormats/L1Trigger/interface/BXVector.h"
#include "DataFormats/L1Trigger/interface/EtSum.h"
#include "DataFormats/L1Trigger/interface/Tau.h"

#define VERBOSE_TRIGGER 0

class NeroTrigger : virtual public NeroCollection, virtual public BareTrigger
{
    public:
        NeroTrigger();
        ~NeroTrigger();
        int analyze(const edm::Event& iEvent);
        virtual inline string name(){return "NeroTrigger";};

        // --- Handle
        edm::Handle< edm::TriggerResults  > handle;	
        edm::Handle<pat::PackedTriggerPrescales> prescale_handle;
        edm::Handle<pat::TriggerObjectStandAloneCollection > object_handle;
        //  handle for L1
        edm::Handle<BXVector<l1t::EtSum> > handle_l1EtSum;
        edm::Handle<BXVector<l1t::Tau> > handle_l1Tau;

        //  The collections are gmtStage2Digis:Muon, caloStage2Digis:EGamma, caloStage2Digis:EtSum, caloStage2Digis:Jet, caloStage2Digis:Tau 
        //
        //BXVector<l1t::EGamma>                 "caloStage2Digis"           "EGamma"          "RECO"    
        //BXVector<l1t::EtSum>                  "caloStage2Digis"           "EtSum"           "RECO"    
        //BXVector<l1t::Jet>                    "caloStage2Digis"           "Jet"             "RECO"    
        //BXVector<l1t::Tau>                    "caloStage2Digis"           "Tau"             "RECO" 

        // --- Token
        edm::EDGetTokenT< edm::TriggerResults > token;
        edm::EDGetTokenT< pat::PackedTriggerPrescales > prescale_token;
        edm::EDGetTokenT< pat::TriggerObjectStandAloneCollection > object_token;
        edm::EDGetTokenT<BXVector<l1t::EtSum> > token_l1EtSum;
        edm::EDGetTokenT<BXVector<l1t::Tau> > token_l1Tau;

        // --- configuration
        template <class T>
            int match(T *coll, pat::TriggerObjectStandAlone& obj, int pdgId);
        template <class T>
            int match(T *coll, pat::TriggerObjectStandAlone& obj);
        // vector<string> *triggerNames
        //
        NeroLeptons *leps_;
        NeroTaus    *taus_;
        NeroJets    *jets_;
        NeroPhotons    *photons_;

        float mDr ;
        //number of trigger to match
        float mNMatch;

        void setTrigger(const string& name, int value);

};

// -- TEMPLATE in the .H File
    template <class T>
int NeroTrigger::match(T *coll, pat::TriggerObjectStandAlone& obj)
{
    if ( coll == NULL ) return -1;

    if(VERBOSE_TRIGGER)cout<<"[NeroTrigger]::[match] start matching for "<<coll->name()<<endl;

    int index=-1;
    float minDr=100.;


    for(int l = 0 ;l< coll -> p4 -> GetEntries() ;++l)
    {
        float dr = ((TLorentzVector*)(*coll -> p4)[l]) ->DeltaR( TLorentzVector(obj.px(),obj.py(),obj.pz(),obj.energy()) );
        if ( dr < mDr and minDr >= dr)
        {
            index = l;
            minDr = dr;
        }
    }
    if(VERBOSE_TRIGGER)cout<<"\tIndex for coll"<< coll->name() <<" is "<< index <<endl;
    return index;
}

    template <class T>
int NeroTrigger::match(T *coll, pat::TriggerObjectStandAlone& obj,int pdgId)
{
    if (coll==NULL) return -1;	

    if(VERBOSE_TRIGGER)cout<<"[NeroTrigger]::[match] pdgId matching for " <<coll->name()<<endl;

    int index=-1;
    float minDr=100.;

    for(int l = 0 ;l< coll -> p4 -> GetEntries() ;++l)
    {
        float dr = ((TLorentzVector*)(*coll -> p4)[l]) ->DeltaR( TLorentzVector(obj.px(),obj.py(),obj.pz(),obj.energy()) );
        //cout <<"[NeroTrigger]::[match] dr = "<<dr<<" mDr"<<mDr<<" minDr "<<minDr<<" pdgId = "<< coll -> pdgId -> at(l) <<endl;
        if ( abs(coll -> pdgId -> at(l)) == abs(pdgId)  and  dr < mDr and minDr >= dr)
        {
            index = l;
            minDr = dr;
        }
    }
    if(VERBOSE_TRIGGER)cout<<"\t[NeroTrigger]::[match] Index for coll"<< coll->name() <<" is "<< index <<endl;
    return index;

}
#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
