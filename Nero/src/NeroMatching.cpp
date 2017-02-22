#include "NeroProducer/Nero/interface/NeroMatching.hpp"
#include <cmath>

//#define VERBOSE 1

void Matcher::match(TClonesArray*a,TClonesArray*b,vector<int>&r, vector<int> *pdgIdB , int pdgId)
{
    r.clear();	
    int sizeA = a->GetEntries();
    r.reserve( sizeA );
    int sizeB = b->GetEntries();

    if(pdgIdB != NULL ) 
        {
        if (pdgIdB -> size()  != (unsigned)sizeB ) 
            cout<<"[Matcher]::[match]::[ERROR] pdgIdB size is "
                <<pdgIdB->size()
                <<" while clones array has size "<<sizeB <<endl;
        }

    for( int i=0; i < sizeA ; ++i )
    {
        int  match = -1;
        TLorentzVector* Ap4 =  (TLorentzVector*) ( (*a)[i] );
        bool zeroPt=false;
        if (Ap4->Pt()<1e-8) zeroPt=true;
        if (std::isnan(Ap4->Pt())) zeroPt=true;

#ifdef VERBOSE
        if(VERBOSE>0)cout<<"[Matcher]::[match]::[DEBUG] A Pt is = "<<Ap4->Pt()<<" zeroPt="<<zeroPt<<endl;
#endif

        for(int j=0;j<sizeB and not zeroPt;++j)
        {
            TLorentzVector* Bp4 =  (TLorentzVector*) ( (*b)[j] );
            
            if (Bp4->Pt() <1.e-8) continue;
            if (std::isnan(Bp4->Pt())) continue;
#ifdef VERBOSE
        if(VERBOSE>0)cout<<"[Matcher]::[match]::[DEBUG] B Pt is = "<<Bp4->Pt()<<endl;
#endif
            if (Ap4->DeltaR(*Bp4) >= dR_ ) continue;

            if (pdgIdB != NULL and pdgId !=0 )
                {
                if( abs(pdgIdB->at(j)) != pdgId ) continue;
                }

            if (match>=0 ) { match =-2; break;}
            if (match == -1) match = j;
        }
        r.push_back(match);
    }
#ifdef VERBOSE
        if(VERBOSE>0)cout<<"[Matcher]::[match]::[DEBUG] returning  "<<endl;
#endif
    return;
}

//vector<int>  *pdgId;
//

void Matcher::match(BareP4*a,BareP4*b,vector<int>&r)
{
    match( a->p4, b->p4, r);
    return;
}

int NeroMatching::analyze(const edm::Event& iEvent)
{
    #ifdef VERBOSE
        if (VERBOSE) cout <<"[NeroMatching]::[analyze] Matching jets"<<endl;
    #endif
    if (jets_->doMatch()){
        matcher_ . SetDr( mJetDr );
        matcher_ . match( jets_ -> p4, mc_ -> jetP4 , * (jets_->match) );
    }
    // -------------
    #ifdef VERBOSE
        if (VERBOSE) cout <<"[NeroMatching]::[analyze] Matching leptons"<<endl;
    #endif
    if (leps_->doMatch()) {
        matcher_ . SetDr( mLepDr );
        vector<int> ele;
        vector<int> muon;
        matcher_ . match( leps_ -> p4, mc_ -> p4 , ele, mc_ -> pdgId , 11 );
        matcher_ . match( leps_ -> p4, mc_ -> p4 , muon, mc_ -> pdgId , 13 );
        for(unsigned i=0;i< ele.size() ;++i)
            {
                if( ele[i] == -2 or muon[i] == -2) leps_->match->push_back(-2);
                else if(ele[i] >=0 and muon[i] ==-1 ) leps_->match->push_back( ele[i] );
                else if(muon[i] >=0 and ele[i] == -1 ) leps_->match->push_back(muon[i]) ;
                else if(muon[i] >=0 and ele[i] >=0 ) leps_->match->push_back(-2);
                else leps_->match->push_back(-1);
            }
        }

    // -------------
    #ifdef VERBOSE
        if (VERBOSE) cout <<"[NeroMatching]::[analyze] Matching photons"<<endl;
    #endif
    if (phos_->doMatch()){
        matcher_ . SetDr( mPhoDr );
        matcher_ . match( phos_ -> p4, mc_ -> p4 , * (phos_->match) , mc_->pdgId, 22);
    }

    // -------------
    #ifdef VERBOSE
        if (VERBOSE) cout <<"[NeroMatching]::[analyze] Matching taus"<<endl;
    #endif
    if (taus_->doMatch()){
        matcher_ . SetDr( mTauDr );
        matcher_ . match( taus_ -> p4, mc_ -> p4 , * (taus_->match) , mc_->pdgId, 15);
    }

    // -------------
    return 0;

}


// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
