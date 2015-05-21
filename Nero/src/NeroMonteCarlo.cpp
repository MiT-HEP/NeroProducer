#include "NeroProducer/Nero/interface/NeroMonteCarlo.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

#include "TStopwatch.h"
#define VERBOSE 0

NeroMonteCarlo::NeroMonteCarlo(){
}

NeroMonteCarlo::~NeroMonteCarlo(){
}

void NeroMonteCarlo::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	pdgId -> clear();
	jetP4 -> Clear();
	puTrueInt = -999;
	mcWeight = 1.0;
	qScale = -999;
	alphaQED = -999;
	alphaQCD = -999;
	x1 = -999;
	x2 = -999;
	pdf1Id = -999;
	pdf2Id = -999;
	scalePdf = -999;
}

void NeroMonteCarlo::defineBranches(TTree *t){
	//
	jetP4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("genjetP4","TClonesArray", &jetP4, 128000, 0);
	//
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("genP4","TClonesArray", &p4, 128000, 0);
	//
	pdgId = new vector<int>;
	t->Branch("genPdgId","vector<int>", &pdgId);
	//
	t->Branch("puTrueInt",&puTrueInt,"puTrueInt/I");
	t->Branch("mcWeight",&mcWeight,"mcWeight/F");
	t->Branch("pdfQscale",&qScale,"pdfQscale/F");
	t->Branch("pdfAlphaQED",&alphaQED,"pdfAlphaQED/F");
	t->Branch("pdfAlphaQCD",&alphaQCD,"pdfAlphaQCD/F");
	t->Branch("pdfX1",&x1,"pdfX1/F");
	t->Branch("pdfX2",&x2,"pdfX2/F");
	t->Branch("pdfId1",&pdf1Id,"pdfId1/I");
	t->Branch("pdfId2",&pdf2Id,"pdfId2/I");
	t->Branch("pdfScalePdf",&scalePdf,"pdfScalePdf/F");
}

int NeroMonteCarlo::analyze(const edm::Event& iEvent){

	TStopwatch sw;
	if(VERBOSE)sw.Start();
	// maybe handle should be taken before
	iEvent.getByToken(info_token, info_handle);
	iEvent.getByToken(packed_token, packed_handle);
	iEvent.getByToken(pruned_token, pruned_handle);
	iEvent.getByToken(pu_token, pu_handle);
	iEvent.getByToken(jet_token, jet_handle);

	if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] getToken took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset(); sw.Start();}
	// INFO
	mcWeight = info_handle -> weight();
	//weights() 
	qScale   = info_handle -> qScale();
	alphaQED = info_handle -> alphaQED();
	alphaQCD = info_handle -> alphaQCD();
	x1       = info_handle -> pdf() -> x.first;
	x2       = info_handle -> pdf() -> x.second;
	pdf1Id   = info_handle -> pdf() -> id.first;
	pdf2Id   = info_handle -> pdf() -> id.second;
	scalePdf = info_handle -> pdf() -> scalePDF;
		
	//PU
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
	TLorentzVector genmet(0,0,0,0);
	//for ( auto & gen : *packed_handle)
	for ( unsigned int i=0;i < packed_handle->size() ;++i)
		{
		const auto gen  = & (*packed_handle)[i];
		if (gen->pt()  < 5 ) continue;
		int pdg = gen->pdgId();
		int apdg = abs(pdg);

		//neutrinos
		if (apdg != 12 and apdg !=14 and apdg !=16
			and apdg <1000000 //SUSY
			and fabs(gen->eta() ) <4.7 
			)
			{ 
			 TLorentzVector tmp( gen->px(),gen->py(),gen->pz(),gen->energy() ); 
			 genmet += tmp;
			}

		
		//FILL
		//    e mu photons
		if ( apdg == 11 or apdg == 13 or apdg == 22)
			{
			new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(gen->px(), gen->py(), gen->pz(), gen->energy());
			pdgId -> push_back( pdg );
			}
		
		} //end packed


	if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] packed took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset(); sw.Start();}
	// LOOP over PRUNED PARTICLES
	//for (auto & gen : *pruned_handle)
	for (unsigned int i=0;i<pruned_handle->size() ;++i)
		{
		const auto gen = &(*pruned_handle)[i];
		if (gen->pt()  < 5 ) continue;
		int pdg = gen->pdgId();
		int apdg = abs(pdg);
		if (gen->status() == 1) continue; //packed

		// tau Z (23)  W (24) H (25) H+ (37)
		if ( apdg == 15 or 
			(apdg >= 23 and apdg <26 ) or 
			apdg == 37)
			{
			new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(gen->px(), gen->py(), gen->pz(), gen->energy());
			pdgId -> push_back( pdg );
			}
		}

	if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] pruned took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset(); sw.Start();}
	// GEN JETS
	for (const auto & j : *jet_handle)
		{
		if (j.pt() < 20 ) continue;
		// --- FILL
		new ( (*jetP4)[jetP4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
		}
	if(VERBOSE){ sw.Stop() ; cout<<"[NeroMonteCarlo]::[analyze] jets took "<<sw.CpuTime()<<" Cpu and "<<sw.RealTime()<<" RealTime"<<endl; sw.Reset();}
	return 0;
}


int NeroMonteCarlo::crossSection(edm::Run const & iRun, TH1F* h)
{
	iRun.getByToken( runinfo_token, runinfo_handle);
	cout<<"in begin Run  intXS/extXSLO/extXSNLO "<<runinfo_handle->internalXSec().value()<<"/"<<runinfo_handle->externalXSecLO().value()<<"/"<<runinfo_handle->externalXSecNLO().value()<<endl;	

	// Internal xSec =  h(0) / h(1) 
  	h->Fill(0. ,runinfo_handle->internalXSec().value()/pow(runinfo_handle->internalXSec().error(),2)    );
  	h->Fill(1 ,1./pow(runinfo_handle->internalXSec().error(),2) );
  	
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

