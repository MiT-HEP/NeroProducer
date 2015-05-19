#include "NeroProducer/Nero/interface/NeroJets.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

NeroJets::NeroJets(){
	p4 = NULL;
	rawPt = NULL;
	puId = NULL;
	unc = NULL;
	qgl = NULL;
	flavour = NULL;
}

NeroJets::~NeroJets(){
}

void NeroJets::clear(){
	// This function clear all the internal storage and init it to an arbitrary value
	p4 -> Clear();
	rawPt -> clear();
	bDiscr -> clear();
	puId -> clear();
	unc -> clear();
	qgl -> clear();
	flavour -> clear();
}

void NeroJets::defineBranches(TTree *t){
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

int NeroJets::analyze(const edm::Event& iEvent){

	// maybe handle should be taken before
	iEvent.getByToken(token, handle);
	iEvent.getByToken(qg_token,qg_handle);

	int ijetRef = -1;
	for (const pat::Jet& j : *handle)
		{
		ijetRef++;
		if (j.pt() < 20 ) continue;

		// JET ID
		if ( !JetId(j) ) continue;
		
		// GET  ValueMaps
		edm::RefToBase<pat::Jet> jetRef(edm::Ref<pat::JetCollection>(handle, ijetRef) );
        	float qgLikelihood = (*qg_handle)[jetRef];
		
		// Fill output object	
		//p4 -> AddLast(new TLorentzVector(j.px(), j.py(), j.pz(), j.energy())  );
		new ( (*p4)[p4->GetSize()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
		rawPt -> push_back (j.pt()*j.jecFactor("Uncorrected"));
		puId -> push_back( j.userFloat("pileupJetId:fullDiscriminant") );
        	bDiscr -> push_back( j.bDiscriminator("combinedInclusiveSecondaryVertexV2BJetTags") );
        	qgl -> push_back( qgLikelihood );
        	flavour -> push_back( j.partonFlavour() );
		
		}
	return 0;
}

bool NeroJets::JetId(const pat::Jet &j)
{
	// 8TeV JetId recommended for 13 TeV
	// May 2015
   	float chf = j.chargedHadronEnergyFraction();
        float nhf = j.neutralHadronEnergyFraction() + j.HFHadronEnergyFraction();
        float phf = j.neutralEmEnergyFraction();
        float elf = j.chargedEmEnergyFraction(); ;
        float muf = j.muonEnergyFraction();
        int chm   = j.chargedHadronMultiplicity();
        int npr   = j.chargedMultiplicity() + j.neutralMultiplicity();
		// put Twiki link!

	        // Loose -- Tight Jet ID
                // --- Number of Constituents   > 1     > 1
                // --- Neutral Hadron Fraction  < 0.99  < 0.90
                // --- Neutral EM Fraction      < 0.99  < 0.90
                // --- Muon Fraction    < 0.8   < 0.8
                // --- And for -2.4 <= eta <= 2.4 in addition apply
                // --- Charged Hadron Fraction  > 0     > 0
                // --- Charged Multiplicity     > 0     > 0
                // --- Charged EM Fraction      < 0.99  < 0.90 

        if ( npr <= 1 ) return false;
        if ( nhf >= 0.99 ) return false;
        if ( phf >= 0.99 ) return false; // neutral EM
        if ( muf >= 0.8 ) return false;
        if ( fabs(j.eta()) <=2.4)
                {
                if ( chf <=0 ) return false;
                if ( chm <=0 ) return false;
                if ( elf <=0 ) return false;
                }
	return true;	
}
