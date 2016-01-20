#include "NeroProducer/Nero/interface/NeroPuppiJets.hpp"
#include "NeroProducer/Nero/interface/Nero.hpp"
#include "NeroProducer/Nero/interface/NeroJets.hpp" // JetId

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


        float charge =  0.;
        float charge_den = 0.;

        for( size_t idx =0; idx < j.numberOfDaughters() ; ++idx)
        {
            pat::PackedCandidate *cand = ( pat::PackedCandidate* ) j.daughter(idx) ; 
            if (cand->charge() !=0 ) {  
                charge     += cand->charge() * cand->puppiWeight() * ( j.px()*cand->px() + j.py()*cand->py() + j.pz()*cand->pz()  ) ;
                charge_den +=                  cand->puppiWeight() * ( j.px()*cand->px() + j.py()*cand->py() + j.pz()*cand->pz()  ) ;
            }
        }

        if ( charge_den == 0 ) { charge=0.0 ; charge_den =1.0;}  //  guard, if no jet id

        // Fill output object	
        new ( (*p4)[p4->GetEntriesFast()]) TLorentzVector(j.px(), j.py(), j.pz(), j.energy());
        rawPt  -> push_back (j.pt()*j.jecFactor("Uncorrected"));
        bDiscr -> push_back( j.bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") );
        unsigned bits=0;
        bits |=  (1 * JetBaseline);
        bits |= JetId(j,"monojet") * mjId;
        bits |= JetId(j,"monojetloose") * mjIdLoose;
        bits |= JetId(j,"monojet2015") * mjId2015;
        bits |= JetId(j,"loose") * JetLoose;
        bits |= JetId(j,"tight") * JetTight;

        selBits -> push_back( bits);
        Q -> push_back (charge/charge_den);
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
    //int NumNeutralParticle =j.neutralMultiplicity(); 
    //float eta = j.eta();

    jetid= NeroJets::JetId(j,id);

    return jetid;
}

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
