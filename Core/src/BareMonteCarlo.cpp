#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareMonteCarlo::BareMonteCarlo() : BareP4(){
}

BareMonteCarlo::~BareMonteCarlo(){
    delete jetP4;
    delete pdgId;
}

void BareMonteCarlo::init(){
    BareP4::init();

    if (!jetP4)
        jetP4 = new TClonesArray("TLorentzVector", 20);
    if (!pdgId)
        pdgId = new vector<int>;
}

void BareMonteCarlo::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
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
    BareP4::defineBranches(t, "gen" );
    //
    t->Branch("genjetP4","TClonesArray", &jetP4, 128000, 0);
    //
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

void BareMonteCarlo::setBranchAddresses(TTree *t){
    BareP4::setBranchAddresses(t,"gen");
    //
    if (t->GetBranchStatus("genjetP4"))
        t->SetBranchAddress("genjetP4"	, &jetP4 );
    if (t->GetBranchStatus("genPdgId"))
        t->SetBranchAddress("genPdgId"	, &pdgId);
    //
    if (t->GetBranchStatus("puTrueInt"))
        t->SetBranchAddress("puTrueInt"	,&puTrueInt	);
    if (t->GetBranchStatus("mcWeight"))
        t->SetBranchAddress("mcWeight"	,&mcWeight	);
    if (t->GetBranchStatus("pdfQscale"))
        t->SetBranchAddress("pdfQscale"	,&qScale	);
    if (t->GetBranchStatus("pdfAlphaQED"))
        t->SetBranchAddress("pdfAlphaQED"	,&alphaQED	);
    if (t->GetBranchStatus("pdfAlphaQCD"))
        t->SetBranchAddress("pdfAlphaQCD"	,&alphaQCD	);
    if (t->GetBranchStatus("pdfX1"))
        t->SetBranchAddress("pdfX1"	,&x1		);
    if (t->GetBranchStatus("pdfX2"))
        t->SetBranchAddress("pdfX2"	,&x2		);
    if (t->GetBranchStatus("pdfId1"))
        t->SetBranchAddress("pdfId1"	,&pdf1Id	);
    if (t->GetBranchStatus("pdfId2"))
        t->SetBranchAddress("pdfId2"	,&pdf2Id	);
    if (t->GetBranchStatus("pdfScalePdf"))
        t->SetBranchAddress("pdfScalePdf"	,&scalePdf	);
}

void BareMonteCarlo::compress(){
    BareP4::compress();
	for(int i=0;i<jetP4->GetEntries();++i)
		BareFunctions::Compress( * (TLorentzVector*) jetP4->At(i)  );
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
