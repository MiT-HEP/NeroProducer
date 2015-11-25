#include "NeroProducer/Nero/interface/NeroPuppiJets.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"

//JES


NeroPuppiJets::NeroPuppiJets() : 
        NeroCollection(),
        BarePuppiJets()
{
    mMinPt = 25.;
    mMinNjets = 0;
    mMinEta = 2.5;
    mMinId = "all";
}

NeroPuppiJets::~NeroPuppiJets(){
}


int NeroPuppiJets::analyze(const edm::Event& iEvent, const edm::EventSetup &iSetup){

    if ( mOnlyMc  ) return 0;

    // maybe handle should be taken before
    iEvent.getByToken(token, handle);

    if ( not handle.isValid() ) cout<<"[NeroPuppiJets]::[analyze]::[ERROR] handle is not valid"<<endl;

    for (const pat::Jet& j : *handle)
    {

        if (j.pt() < mMinPt ) continue;
        if ( fabs(j.eta()) > mMinEta ) continue;
        if ( !JetId(j,mMinId) ) continue;

        // Fill output object	
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
        rawPt  -> push_back (j.pt()*j.jecFactor("Uncorrected"));
        bDiscr -> push_back( j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") );
        unsigned bits=0;
        bits |=  (1 * JetBaseline);

        selBits -> push_back( bits);
    }

    if ( int(rawPt -> size()) < mMinNjets ) return 1;

    return 0;
}

bool NeroPuppiJets::JetId(const pat::Jet &j, std::string id)
{

    bool jetid = false;

    //float NHF    = j.neutralHadronEnergyFraction();
    //float NEMF   = j.neutralEmEnergyFraction();
    //float CHF    = j.chargedHadronEnergyFraction();
    //float MUF    = j.muonEnergyFraction();
    //float CEMF   = j.chargedEmEnergyFraction();
    //int NumConst = j.chargedMultiplicity()+j.neutralMultiplicity();
    //int CHM      = j.chargedMultiplicity();

    if (id=="all") return true; // fake Id, pass all

    return jetid;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
