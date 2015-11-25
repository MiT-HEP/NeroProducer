#include "NeroProducer/Core/interface/BareMonteCarlo.hpp"
#include "NeroProducer/Core/interface/BareFunctions.hpp"

BareMonteCarlo::BareMonteCarlo() : BareP4(){
}

BareMonteCarlo::~BareMonteCarlo(){
    BareFunctions::Delete(jetP4);
    BareFunctions::Delete(pdgId);
    BareFunctions::Delete(pdfRwgt);
    BareFunctions::Delete(flags);
    BareFunctions::Delete(genIso);
    BareFunctions::Delete(genIsoFrixione);
}

void BareMonteCarlo::init(){
    BareP4::init();

    BareFunctions::New(jetP4);
    BareFunctions::New(pdgId);
    BareFunctions::New(pdfRwgt);
    BareFunctions::New(flags);
    BareFunctions::New(genIso);
    BareFunctions::New(genIsoFrixione);
}

void BareMonteCarlo::clear(){
    // This function clear all the internal storage and init it to an arbitrary value
    BareP4::clear();
    pdgId -> clear();
    flags -> clear();
    jetP4 -> Clear();
    pdfRwgt->clear();
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
    r2f1 = -1.;
    r5f1 = -1.;
    r1f2 = -1.;
    r2f2 = -1.;
    r1f5 = -1.;
    r5f5 = -1.;
    genIso -> clear();
    genIsoFrixione -> clear();
}

void BareMonteCarlo::defineBranches(TTree *t){
    //
    BareP4::defineBranches(t, "gen" );
    //
    t->Branch("genjetP4","TClonesArray", &jetP4, 128000, 0);
    //
    t->Branch("genPdgId","vector<int>", &pdgId);
    t->Branch("genFlags","vector<unsigned>", &flags);
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
    t->Branch("r2f1", &r2f1, "r2f1/F");
    t->Branch("r5f1", &r5f1, "r5f1/F");
    t->Branch("r1f2", &r1f2, "r1f2/F");
    t->Branch("r2f2", &r2f2, "r2f2/F");
    t->Branch("r1f5", &r1f5, "r1f5/F");
    t->Branch("r5f5", &r5f5, "r5f5/F");
    t->Branch("pdfRwgt", "vector<float>", &pdfRwgt);
    t->Branch("genIso","vector<float>", &genIso);
    t->Branch("genIsoFrixione","vector<float>", &genIsoFrixione);
}

void BareMonteCarlo::setBranchAddresses(TTree *t){
    BareP4::setBranchAddresses(t,"gen");
    //
    BareFunctions::SetBranchAddress(t,"genjetP4"	, &jetP4 );
    BareFunctions::SetBranchAddress(t,"genPdgId"	, &pdgId);
    BareFunctions::SetBranchAddress(t,"genFlags"	, &flags);

    BareFunctions::SetBranchAddress(t,"puTrueInt"	,&puTrueInt	);
    BareFunctions::SetBranchAddress(t,"mcWeight"	,&mcWeight	);
    BareFunctions::SetBranchAddress(t,"pdfQscale"	,&qScale	);
    BareFunctions::SetBranchAddress(t,"pdfAlphaQED"	,&alphaQED	);
    BareFunctions::SetBranchAddress(t,"pdfAlphaQCD"	,&alphaQCD	);
    BareFunctions::SetBranchAddress(t,"pdfX1"	,&x1		);
    BareFunctions::SetBranchAddress(t,"pdfX2"	,&x2		);
    BareFunctions::SetBranchAddress(t,"pdfId1"	,&pdf1Id	);
    BareFunctions::SetBranchAddress(t,"pdfId2"	,&pdf2Id	);
    BareFunctions::SetBranchAddress(t,"pdfScalePdf"	,&scalePdf	);
    BareFunctions::SetBranchAddress(t, "r2f1", &r2f1);
    BareFunctions::SetBranchAddress(t, "r5f1", &r5f1);
    BareFunctions::SetBranchAddress(t, "r1f2", &r1f2);
    BareFunctions::SetBranchAddress(t, "r2f2", &r2f2);
    BareFunctions::SetBranchAddress(t, "r1f5", &r1f5);
    BareFunctions::SetBranchAddress(t, "r5f5", &r5f5);
    BareFunctions::SetBranchAddress(t,"pdfRwgt", &pdfRwgt);
    BareFunctions::SetBranchAddress(t,"genIso", &genIso);
    BareFunctions::SetBranchAddress(t,"genIsoFrixione", &genIsoFrixione);
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
