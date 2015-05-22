#include "NeroProducer/Nero/interface/BareMonteCarlo.hpp"

BareMonteCarlo::BareMonteCarlo() {
}

BareMonteCarlo::~BareMonteCarlo(){
	p4 = NULL;
	pdgId = NULL;
	jetP4 = NULL;
}

void BareMonteCarlo::clear(){
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

void BareMonteCarlo::defineBranches(TTree *t){
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

