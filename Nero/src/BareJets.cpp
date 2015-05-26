#include "NeroProducer/Nero/interface/BareJets.hpp"

BareJets::BareJets(){
	p4 = NULL;
	rawPt = NULL;
	puId = NULL;
	unc = NULL;
	qgl = NULL;
	flavour = NULL;
}

BareJets::~BareJets(){}


void BareJets::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	rawPt -> clear();
	bDiscr -> clear();
	puId -> clear();
	unc -> clear();
	qgl -> clear();
	flavour -> clear();
}

void BareJets::defineBranches(TTree *t){
	//
	p4 = new TClonesArray("TLorentzVector", 20);
	t->Branch("jetP4","TClonesArray", &p4, 128000, 0);
	//
	rawPt = new vector<float>;
	t->Branch("jetRawPt","vector<float>",&rawPt);
	//
	bDiscr = new vector<float>;
	t->Branch("jetBdiscr","vector<float>",&bDiscr);
	//	
	puId = new vector<float>;
	t->Branch("jetPuId","vector<float>",&puId);
	// -- JES uncertainty
	unc = new vector<float>;
	t->Branch("jetUnc","vector<float>",&unc);
	// --QGL
	qgl = new vector<float>;
	t->Branch("jetQGL","vector<float>",&qgl);
	// -- Jet Flavour by PAT
	flavour = new vector<int>;
	t->Branch("jetFlavour","vector<int>",&flavour);
}

void BareJets::setBranchAddresses(TTree*t)
{
	p4 = new TClonesArray("TLorentzVector", 20);
	t->SetBranchAddress("jetP4"	, &p4	);
	t->SetBranchAddress("jetRawPt"	,&rawPt);
	t->SetBranchAddress("jetBdiscr"	,&bDiscr);
	t->SetBranchAddress("jetPuId"	,&puId);
	t->SetBranchAddress("jetUnc"	,&unc);
	t->SetBranchAddress("jetQGL"	,&qgl);
	t->SetBranchAddress("jetFlavour"	,&flavour);
}
