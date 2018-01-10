#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

#include "TStopwatch.h"

#include <map>

#define VERBOSE 0

NeroMonteCarlo::NeroMonteCarlo(edm::ConsumesCollector & cc,edm::ParameterSet iConfig):
    NeroCollection(cc, iConfig),
    BareMonteCarlo(),
    NeroRun()
{
    mParticleGun = false;
    mMinGenParticlePt = 5.;
    mMinGenJetPt = 20.;
    isRealData = 0;

    SetExtend ( iConfig.getUntrackedParameter<bool>("extendMonteCarlo",false) );
    packed_token = cc.consumes<edm::View<pat::PackedGenParticle> >(iConfig.getParameter<edm::InputTag>("packedgen"));
    pruned_token = cc.consumes<edm::View<reco::GenParticle> >(iConfig.getParameter<edm::InputTag>("prunedgen")) ;
    info_token   = cc.consumes<GenEventInfoProduct>(iConfig.getParameter<edm::InputTag>("generator"));
    lhe_token   = cc.mayConsume<LHEEventProduct>(iConfig.getParameter<edm::InputTag>("lhe"));//LHEEventProduct_externalLHEProducer__LHE
    pu_token     = cc.consumes<std::vector<PileupSummaryInfo> >(iConfig.getParameter<edm::InputTag>("pileup"));
    jet_token    = cc.consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjets"));
    jetAK8_token    = cc.consumes<reco::GenJetCollection>(iConfig.getParameter<edm::InputTag>("genjetsAK8"));

    runinfo_token = cc.consumes<GenRunInfoProduct,edm::InRun>(iConfig.getParameter<edm::InputTag>("genruninfo") );
    mMinGenParticlePt = iConfig.getParameter<double>("minGenParticlePt");
    mMinGenJetPt = iConfig.getParameter<double>("minGenJetPt");
    mParticleGun = iConfig.getUntrackedParameter<bool>("particleGun",false);

    genBHadFlavour_token = cc.consumes<std::vector<int> > (edm::InputTag("matchGenBHadron", "genBHadFlavour"));
    genCHadJetIndex_token = cc.consumes<std::vector<int> > (edm::InputTag("matchGenCHadron", "genCHadJetIndex"));
    genCHadBHadronId_token = cc.consumes<std::vector<int> > (edm::InputTag("matchGenCHadron", "genCHadBHadronId"));
    genTtbarId_token = cc.consumes<int> (edm::InputTag("categorizeGenTtbar", "genTtbarId"));
}

NeroMonteCarlo::~NeroMonteCarlo(){
}

int NeroMonteCarlo::analyze(const edm::Event& iEvent){

    if ( iEvent.isRealData() ) return 0;
    isRealData = iEvent.isRealData() ? 1 : 0 ; // private, not the one in the tree

    std::map<const reco::Candidate *,unsigned int> genIndices;
    std::vector<const reco::Candidate *> savedParticles;

    TStopwatch sw;
    if(VERBOSE)sw.Start();
    // maybe handle should be taken before
    iEvent.getByToken(info_token, info_handle);
    iEvent.getByToken(lhe_token, lhe_handle);
    iEvent.getByToken(packed_token, packed_handle);
    iEvent.getByToken(pruned_token, pruned_handle);
    iEvent.getByToken(pu_token, pu_handle);
    iEvent.getByToken(jet_token, jet_handle);
    iEvent.getByToken(jetAK8_token, jetAK8_handle);
    iEvent.getByToken(genBHadFlavour_token,genBHadFlavour_handle);
    iEvent.getByToken(genCHadJetIndex_token,genCHadJetIndex_handle);
    iEvent.getByToken(genCHadBHadronId_token,genCHadBHadronId_handle);

    iEvent.getByToken(genTtbarId_token,genTtbarId_handle);

    if ( not info_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::[ERROR] info_handle is not valid"<<endl;
    if ( not packed_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::[ERROR] packed_handle is not valid"<<endl;
    if ( not pruned_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::[ERROR] pruned_handle is not valid"<<endl;
    if ( not pu_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::[ERROR] pu_handle is not valid"<<endl;
    if ( not jet_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::[ERROR] jet_handle is not valid"<<endl;
    if ( not jetAK8_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::[ERROR] jetAK8_handle is not valid"<<endl;
    if (not genBHadFlavour_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::ERROR genBHadFlavour_handle is not valid"<<endl;
    if (not genCHadJetIndex_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::ERROR genCHadJetIndex_handle is not valid"<<endl;
    if (not genCHadBHadronId_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::ERROR genCHadBHadronId_handle is not valid"<<endl;
    if (not genTtbarId_handle.isValid() ) cout<<"[NeroMonteCarlo]::[analyze]::ERROR genTtbarId_handle is not valid"<<endl;

    if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] getToken took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset(); sw.Start();}
    // INFO
    if(VERBOSE>1) cout<<"[NeroMonteCarlo]::[analyze]::[DEBUG] mcWeight="<<endl;
    mcWeight = info_handle -> weight();
    if(VERBOSE>1) cout<<"                                     mcWeight="<<mcWeight<<endl;
    //weights() 
    //---  scale
    if ( lhe_handle.isValid() and  lhe_handle->weights().size() >=9){
        r1f2 = double(lhe_handle -> weights() . at(1) . wgt); 
        r1f5 = double(lhe_handle -> weights() . at(2) . wgt); 
        r2f1 = double(lhe_handle -> weights() . at(3) . wgt); 
        r2f2 = double(lhe_handle -> weights() . at(4) . wgt); 
        r5f1 = double(lhe_handle -> weights() . at(6) . wgt); 
        r5f5 = double(lhe_handle -> weights() . at(8) . wgt);  
    }

    if (lhe_handle.isValid() and  lhe_handle->weights().size() >109)
        for( int pdfw = 9 ; pdfw<109 ;++pdfw)
        {
        pdfRwgt -> push_back( double(lhe_handle -> weights() . at(pdfw) . wgt ) );    
        }

    if (lhe_handle.isValid() )  // save lhe particles
    {
        for ( unsigned ilhe =0 ; ilhe < lhe_handle->hepeup().PUP.size(); ++ilhe )
        {
          TLorentzVector fourMom;
          fourMom.SetPxPyPzE( lhe_handle->hepeup().PUP[ilhe][0],lhe_handle->hepeup().PUP[ilhe][1],lhe_handle->hepeup().PUP[ilhe][2],lhe_handle->hepeup().PUP[ilhe][3] );
          new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( fourMom );
          pdgId -> push_back( lhe_handle->hepeup().IDUP[ilhe] );
          flags -> push_back( LHE );
          // doing the following for dressed leptons as well so vectors stay in sync
          genIndices[NULL] = pdgId->size()-1; // save gen particle pointers
          savedParticles.push_back(NULL);     //  no navigation for LHE
          genIso -> push_back (0.) ;
        }
    }

    // --- fill pdf Weights
    //
    if(VERBOSE>1) cout<<"[NeroMonteCarlo]::[analyze]::[DEBUG] PDF="<<endl;
    if ( mParticleGun ) {
        qScale   = -999 ;
        alphaQED = -999 ;
        alphaQCD = -999 ;
        x1       = -999 ;
        x2       = -999 ;
        pdf1Id   = -999 ;
        pdf2Id   = -999 ;
        scalePdf = -999 ;
    }
    else {
        qScale   = info_handle -> qScale();
        alphaQED = info_handle -> alphaQED();
        alphaQCD = info_handle -> alphaQCD();
        x1       = info_handle -> pdf() -> x.first;
        x2       = info_handle -> pdf() -> x.second;
        pdf1Id   = info_handle -> pdf() -> id.first;
        pdf2Id   = info_handle -> pdf() -> id.second;
        scalePdf = info_handle -> pdf() -> scalePDF;
    }

    //PU
    if(VERBOSE>1){ cout<<endl<<"[NeroMonteCarlo]::[analyze] PU LOOP"<<endl;}
    puTrueInt = 0;
    for(const auto & pu : *pu_handle)
    {
        //Intime
        if (pu.getBunchCrossing() == 0)
            puTrueInt += pu.getTrueNumInteractions();
        //puInt += getPU_NumInteractions(); //old
        //Out-of-time
    }

    if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] pu&info took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset(); sw.Start();}
    // GEN PARTICLES
    //TLorentzVector genmet(0,0,0,0);
    //for ( auto & gen : *packed_handle)
    for ( unsigned int i=0;i < packed_handle->size() ;++i)
    {
        const auto gen  = & (*packed_handle)[i];
        if (gen->pt()  < 5 ) continue;
        if (gen->pt() < mMinGenParticlePt ) continue;
        int pdg = gen->pdgId();
        int apdg = abs(pdg);

        //neutrinos
        // --- if ( (apdg != 12 and apdg !=14 and apdg != 16
        // ---       and apdg > 1000000  neutrinos and neutralinos
        // ---      )//SUSY
        // ---         and fabs(gen->eta() ) <4.7 
        // ---    )
        // --- { 
        // ---     TLorentzVector tmp( gen->px(),gen->py(),gen->pz(),gen->energy() ); 
        // ---     genmet += tmp;
        // --- }
        // --- genmet = -genmet;


        //FILL
        //    e mu photons
        if ( apdg == 11 or apdg == 13 or apdg == 22  // e - mu - gamma
                or (apdg >=12 and apdg<=16) // neutrinos
                or apdg > 1000000  // susy neutrinos and neutralinos
            )
        {
            new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(gen->px(), gen->py(), gen->pz(), gen->energy());
            pdgId -> push_back( pdg );
            flags -> push_back( ComputeFlags( *gen ) );
            genIndices[gen] = pdgId->size()-1; // save gen particle pointers
            savedParticles.push_back(gen);     // so we can associate mothers later
            // compute ISOLATION
            float iso=0;

            if (apdg == 22 or apdg ==11 or apdg ==13)
            {

                    TLorentzVector g1(gen->px(),gen->py(),gen->pz(),gen->energy());
                    for ( unsigned int j=0;j < packed_handle->size() ;++j)
                    {
                        if (i==j) continue;
                        const auto gen2  = & (*packed_handle)[j];
                        if ( gen2->pt() ==0 ) continue;
                        if (gen2->pz() > 10e8 ) continue; // inf
                        TLorentzVector g2(gen2->px(),gen2->py(),gen2->pz(),gen2->energy());
                        if (g2.DeltaR(g1) <0.4){
                            iso += g2.Pt();
                         }
                    }
            }
            genIso -> push_back(iso);
            // computed dressed objects
            //

            if (apdg == 11 or apdg == 13) { // only for final state muons and electrons
                    TLorentzVector dressedLepton(gen->px(),gen->py(),gen->pz(),gen->energy());
                    TLorentzVector lepton(dressedLepton); //original lepton for dR
                    for ( unsigned int j=0;j < packed_handle->size() ;++j)
                    {
                        const auto gen2  = & (*packed_handle)[j];
                        TLorentzVector photon(gen2->px(),gen2->py(),gen2->pz(),gen2->energy());
                        if (i != j and abs( gen->pdgId() ) ==22  and lepton.DeltaR( photon ) <0.1 ) dressedLepton += photon;
                    }
                    new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector( dressedLepton );
                    pdgId -> push_back( pdg );
                    flags -> push_back( Dressed );
                    // doing the following for dressed leptons as well so vectors stay in sync
                    genIndices[gen] = pdgId->size()-1; // save gen particle pointers
                    savedParticles.push_back(gen);     // so we can associate mothers later
                    genIso -> push_back (0.) ;
                    // --- end of dressing
            }
             
        }

    } //end packed


    if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] packed took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset(); sw.Start();}
    // LOOP over PRUNED PARTICLES
    //for (auto & gen : *pruned_handle)
    for (unsigned int i=0;i<pruned_handle->size() ;++i)
    {
        const auto gen = &(*pruned_handle)[i];
        if (gen->pt()  < 5 ) continue;
        if (gen->pt()  < mMinGenParticlePt ) continue;
        int pdg = gen->pdgId();
        int apdg = abs(pdg);
        int mpdg = gen->mother(0)->pdgId();
        float mpt = gen->mother(0)->pt();
        if (pdg==mpdg and mpt>5)continue;
        if (gen->status() == 1) continue; //packed

        unsigned flag = ComputeFlags(*gen);

    
        if ( apdg == 15 or  // tau (15)
                (apdg >= 23 and apdg <26 ) or   // Z(23) W(24) H(25)
                apdg == 37 or // chHiggs: H+(37)
                apdg <= 6 or // quarks up (2) down (1)  charm (4) strange (3) top (6) bottom (5)
                apdg == 21 or // gluons (21)
                apdg > 1000000 // susy neutrinos,neutralinos, charginos ...  lightest neutralinos (1000022)
                or ( apdg == 11 and  ( flag &  HardProcessBeforeFSR) )
                or ( apdg == 11 and  ( flag &  HardProcess) )
                or ( apdg == 13 and  ( flag &  HardProcessBeforeFSR) )
                or ( apdg == 13 and  ( flag &  HardProcess) )
                )
        {
            new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(gen->px(), gen->py(), gen->pz(), gen->energy());
            pdgId -> push_back( pdg );
            flags -> push_back( flag );
            genIndices[gen] = pdgId->size()-1; // save gen particle pointers
            savedParticles.push_back(gen);     // so we can associate mothers later
            genIso -> push_back (0.) ;
        }
    }

    for ( unsigned int i=0;i < savedParticles.size() ;++i) // repeat loop to associate parents
                                                           // this loop is O(N*logN) where N is the number of saved particles
    {
        const auto gen = savedParticles[i];
        if ( gen == NULL or gen->numberOfMothers()==0) {
            parent->push_back(-1);
            continue;
        }         
        int motherIdx=-1;
        const reco::Candidate *gen_parent = gen;
        while (gen_parent->numberOfMothers()>0) {
            gen_parent = gen_parent->mother(0);
            auto gen_parent_ = genIndices.find(gen_parent);
            if (gen_parent_==genIndices.end())
                continue;
            motherIdx=gen_parent_->second;
            break;
        }
        parent->push_back(motherIdx);
    }

    if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] pruned took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset(); sw.Start();}
    // GEN JETS AK4
    for (const auto & j : *jet_handle)
    {
        if (j.pt() < 20 ) continue;
        if (j.pt() < mMinGenJetPt ) continue;
        // --- FILL
        new ( (*jetP4)[jetP4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
    }

    // GEN JETS AK8
    for (const auto & j : *jet_handle)
    {
        if (j.pt() < 100 ) continue;
        if (j.pt() < mMinGenJetPt ) continue;
        // --- FILL
        new ( (*jetAK8P4)[jetAK8P4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
    }

    // Counting number of different b hadrons https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_20_patchX/PhysicsTools/JetMCAlgos/test/matchGenHFHadrons.cc#L216
    for(size_t hadronId = 0; hadronId < genBHadFlavour_handle->size(); ++hadronId) {
        const int flavour = genBHadFlavour_handle->at(hadronId);
        const int flavourAbs = std::abs(flavour);
    // 0 - not from top decay; 1 - from top decay; 2 - not identified;
        nBHadrons++;
        if(flavourAbs==25) nBHadronsHiggs++;
        if(flavourAbs==37) nBHadronsHiggs++; // I consider higgs both H and H+
        if(flavourAbs==6) nBHadronsTop++;
    }
    // Counting number of different c hadrons
    for(size_t hadronId = 0; hadronId < genCHadJetIndex_handle->size(); ++hadronId) {
        // Skipping c hadrons that are coming from b hadrons
        if(genCHadBHadronId_handle->at(hadronId) >= 0) continue;
        nCHadrons++;
        
    }

    genTtbarId = *genTtbarId_handle ;

    if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] jets took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset();}
    return 0;
}


int NeroMonteCarlo::crossSection(edm::Run const & iRun, TH1F* h)
{
    // will be run at the end of run
    if ( isRealData ) return 0; // if the first run has no events ?  TODO

    iRun.getByToken( runinfo_token, runinfo_handle);
    //iRun.getByLabel( "generator", runinfo_handle);

    if ( not runinfo_handle . isValid() ) cout<<"[NeroMonteCarlo]::[crossSection]::[WARNING] runinfo_handle is not valid. Ignore if running on data."<<endl;
    if ( not runinfo_handle . isValid() ) return 0;

    cout<<"in begin Run  intXS/extXSLO/extXSNLO "<<runinfo_handle->internalXSec().value()<<"/"<<runinfo_handle->externalXSecLO().value()<<"/"<<runinfo_handle->externalXSecNLO().value()<<endl;	

    // Internal xSec =  h(0) / h(1) 
    if ( runinfo_handle->internalXSec().error() != 0 )
    {
        h->Fill(0. ,runinfo_handle->internalXSec().value()/pow(runinfo_handle->internalXSec().error(),2)    );
        h->Fill(1 ,1./pow(runinfo_handle->internalXSec().error(),2) );
    }
    else 
    {
        cout <<" Warning: ERROR on xSec is 0. Setting it to 1"<<endl;
        h->Fill(0. ,runinfo_handle->internalXSec().value()    );
        h->Fill(1 ,1.);
    }

    // External xSec =  h(2) / h(3) 
    h->Fill(2 ,runinfo_handle->externalXSecLO().value()/pow(runinfo_handle->externalXSecLO().error(),2)  );
    h->Fill(3 ,1./pow(runinfo_handle->externalXSecLO().error(),2)  );

    h->Fill(4 ,runinfo_handle->externalXSecNLO().value()/pow(runinfo_handle->externalXSecNLO().error(),2) );
    h->Fill(5 ,1./pow(runinfo_handle->externalXSecNLO().error(),2) );

    h->Fill(6 , 1 );

    h->Fill(7 ,pow(runinfo_handle->internalXSec().value(),1) );
    h->Fill(6 ,pow(runinfo_handle->externalXSecLO().value(),1)  );
    h->Fill(9 ,pow(runinfo_handle->externalXSecNLO().value(),1) );

    h->Fill(10 ,pow(runinfo_handle->internalXSec().value(),2) );
    h->Fill(11 ,pow(runinfo_handle->externalXSecLO().value(),2)  );
    h->Fill(12 ,pow(runinfo_handle->externalXSecNLO().value(),2) );

    return 0;
}

// ----- TEMPLATE SPECIFICATION
template<> 
unsigned NeroMonteCarlo::ComputeFlags<const pat::PackedGenParticle>(const pat::PackedGenParticle &p)
{
    // some of the template calls make no sense for teh packed gen particles
    unsigned flag=0;
    if (p.isPromptFinalState() ) flag |= PromptFinalState; //OK
    //if (p.isPromptDecayed() ) flag |= PromptDecayed;
    if (p.isDirectPromptTauDecayProductFinalState() ) flag |= DirectPromptTauDecayProductFinalState; //OK
    //if (p.isHardProcess() ) flag |= HardProcess;
    //if (p.fromHardProcessBeforeFSR() ) flag |= HardProcessBeforeFSR;
    //if (p.fromHardProcessDecayed() ) flag |= HardProcessDecayed;
    //if (p.isLastCopy() ) flag |= LastCopy;
    //if (p.isLastCopyBeforeFSR() ) flag |= LastCopyBeforeFSR;
    return flag;
}

NEROREGISTER(NeroMonteCarlo);

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
