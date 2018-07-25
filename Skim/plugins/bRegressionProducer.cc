
#include "DataFormats/Candidate/interface/VertexCompositePtrCandidate.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "FWCore/Framework/interface/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "RecoJets/JetProducers/interface/PileupJetIdAlgo.h"
#include "RecoVertex/VertexPrimitives/interface/ConvertToFromReco.h"
#include "RecoVertex/VertexPrimitives/interface/VertexState.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"


#include <iostream>
#include <string>
#include <vector>

#include "PhysicsTools/TensorFlow/interface/TensorFlow.h"

#define debug 0

using namespace std;
using namespace edm;

class bRegressionProducer : public EDProducer
{
    // original code from: https://github.com/michelif/flashgg/blob/test_reg_94X/Taggers/plugins/flashggbRegressionProducer.cc
    // modified to run on top of pat::Jet.
    // produces a value map. 
    // comptues everything on top of pat::Jet

    // from VarProducer
    std::tuple<float,float,float>
    calculatePtRatioRelSimple(edm::Ptr<reco::Candidate> lep, edm::Ptr<pat::Jet> jet) const {
    
      auto lepp4 = lep->p4();
      auto rawp4 = jet->correctedP4("Uncorrected");
    
      if ((rawp4-lepp4).R()<1e-4) return std::tuple<float,float,float>(1.0,0.0,0.0);
      
      auto jetp4 = jet->p4();//(rawp4 - lepp4*(1.0/jet->jecFactor("L1FastJet")))*(jet->pt()/rawp4.pt())+lepp4;
      auto ptratio = lepp4.pt()/jetp4.pt();
      auto ptrel = lepp4.Vect().Cross((jetp4).Vect().Unit()).R();
      auto ptrelinv = jetp4.Vect().Cross((lepp4).Vect().Unit()).R();
      
      return std::tuple<float,float,float>(ptratio,ptrel,ptrelinv);
    }
    
    template <typename T,typename J> void putInEvent(std::string, const edm::Handle< J >&, std::vector<T>*, edm::Event&);

    public:
        bRegressionProducer( const ParameterSet & );
        ~bRegressionProducer(){};
        void InitJet();
        void SetNNVectorVar();
        std::vector<float> EvaluateNN();
    private:
        void produce( Event &, const EventSetup & ) override;
        //        std::vector<edm::InputTag> inputTagJets_;
        edm::InputTag inputTagJets_;

        EDGetTokenT<View<pat::Jet> > jetToken_;
        edm::EDGetTokenT<double> rhoToken_;        
        edm::EDGetTokenT<std::vector<reco::Vertex>> vtxToken_;
        edm::EDGetTokenT< edm::View<reco::VertexCompositePtrCandidate> > svToken_;

        string bRegressionWeightfileName_;
        double y_mean_,y_std_;

        tensorflow::Session* session;
        tensorflow::GraphDef* graphDef;
        std::vector<float> NNvectorVar_; 
        //add vector of mva for eache jet

        //mva variables
        float Jet_pt ;
        float Jet_eta ;
        float rho ;
        float Jet_mt ;
        float Jet_leadTrackPt ;
        float Jet_leptonPtRel ;
        float Jet_leptonDeltaR ;
        float Jet_neHEF ;
        float Jet_neEmEF ;
        float Jet_vtxPt ;
        float Jet_vtxMass ;
        float Jet_vtx3dL ;
        float Jet_vtxNtrk ;
        float Jet_vtx3deL ;
        float Jet_numDaughters_pt03 ;
        float Jet_energyRing_dR0_em_Jet_e ;
        float Jet_energyRing_dR1_em_Jet_e ;
        float Jet_energyRing_dR2_em_Jet_e ;
        float Jet_energyRing_dR3_em_Jet_e ;
        float Jet_energyRing_dR4_em_Jet_e ;
        float Jet_energyRing_dR0_neut_Jet_e ;
        float Jet_energyRing_dR1_neut_Jet_e ;
        float Jet_energyRing_dR2_neut_Jet_e ;
        float Jet_energyRing_dR3_neut_Jet_e ;
        float Jet_energyRing_dR4_neut_Jet_e ;
        float Jet_energyRing_dR0_ch_Jet_e ;
        float Jet_energyRing_dR1_ch_Jet_e ;
        float Jet_energyRing_dR2_ch_Jet_e ;
        float Jet_energyRing_dR3_ch_Jet_e ;
        float Jet_energyRing_dR4_ch_Jet_e ;
        float Jet_energyRing_dR0_mu_Jet_e ;
        float Jet_energyRing_dR1_mu_Jet_e ;
        float Jet_energyRing_dR2_mu_Jet_e ;
        float Jet_energyRing_dR3_mu_Jet_e ;
        float Jet_energyRing_dR4_mu_Jet_e ;
        float Jet_chHEF;//implement from here
        float Jet_chEmEF;
        float Jet_leptonPtRelInv;
        int isEle;
        int isMu;
        int isOther;
        float Jet_mass;
        float Jet_withPtd;


};


bRegressionProducer::bRegressionProducer( const ParameterSet &iConfig ) :
    inputTagJets_( iConfig.getParameter<edm::InputTag>( "JetTag" )) ,
    rhoToken_( consumes<double>(iConfig.getParameter<edm::InputTag>( "rhoFixedGridCollection" ) ) ),
    vtxToken_ ( consumes<std::vector<reco::Vertex>> (iConfig.getParameter<edm::InputTag>("vtxCollection"))),
    svToken_ ( consumes< edm::View<reco::VertexCompositePtrCandidate>  > ( iConfig.getParameter<edm::InputTag>("svCollection")) ),
    bRegressionWeightfileName_( iConfig.getUntrackedParameter<std::string>("bRegressionWeightfile")),
    y_mean_(iConfig.getUntrackedParameter<double>("y_mean")),
    y_std_(iConfig.getUntrackedParameter<double>("y_std"))
{


    jetToken_= consumes<View<pat::Jet> >(inputTagJets_);

    graphDef= tensorflow::loadGraphDef(edm::FileInPath(bRegressionWeightfileName_.c_str()).fullPath().c_str()); 
    session = tensorflow::createSession(graphDef);


    Jet_pt = 0.;
    Jet_eta = 0.;
    rho = 0.;
    Jet_mt = 0.;
    Jet_leadTrackPt = 0.;
    Jet_leptonPtRel = 0.;
    Jet_leptonDeltaR = 0.;
    Jet_neHEF = 0.;
    Jet_neEmEF = 0.;
    Jet_vtxPt = 0.;
    Jet_vtxMass = 0.;
    Jet_vtx3dL = 0.;
    Jet_vtxNtrk = 0.;
    Jet_vtx3deL = 0.;
    Jet_numDaughters_pt03 = 0;
    Jet_energyRing_dR0_em_Jet_e = 0.;
    Jet_energyRing_dR1_em_Jet_e = 0.;
    Jet_energyRing_dR2_em_Jet_e = 0.;
    Jet_energyRing_dR3_em_Jet_e = 0.;
    Jet_energyRing_dR4_em_Jet_e = 0.;
    Jet_energyRing_dR0_neut_Jet_e = 0.;
    Jet_energyRing_dR1_neut_Jet_e = 0.;
    Jet_energyRing_dR2_neut_Jet_e = 0.;
    Jet_energyRing_dR3_neut_Jet_e = 0.;
    Jet_energyRing_dR4_neut_Jet_e = 0.;
    Jet_energyRing_dR0_ch_Jet_e = 0.;
    Jet_energyRing_dR1_ch_Jet_e = 0.;
    Jet_energyRing_dR2_ch_Jet_e = 0.;
    Jet_energyRing_dR3_ch_Jet_e = 0.;
    Jet_energyRing_dR4_ch_Jet_e = 0.;
    Jet_energyRing_dR0_mu_Jet_e = 0.;
    Jet_energyRing_dR1_mu_Jet_e = 0.;
    Jet_energyRing_dR2_mu_Jet_e = 0.;
    Jet_energyRing_dR3_mu_Jet_e = 0.;
    Jet_energyRing_dR4_mu_Jet_e = 0.;
    Jet_chHEF = 0.;//implement from here
    Jet_chEmEF = 0.;
    Jet_leptonPtRelInv = 0.;
    isEle = 0.;
    isMu = 0.;
    isOther = 0.;
    Jet_mass = 0.;
    Jet_withPtd = 0.;

    produces<edm::ValueMap<float>>("bRegression");
    produces<edm::ValueMap<float>>("bRegressionResolution");
}



void bRegressionProducer::produce( Event &evt, const EventSetup & )
{
    InitJet();
    // input jets
    Handle<View<pat::Jet> > jets;
    evt.getByToken( jetToken_, jets );//just to try get the first one
    //unique_ptr<vector<pat::Jet> > jetColl( new vector<pat::Jet> );
    edm::Handle<double> rhoHandle;
    evt.getByToken( rhoToken_, rhoHandle );
    const double rhoFixedGrd = *( rhoHandle.product() );

    edm::Handle<std::vector<reco::Vertex>> vtxHandle;
    evt.getByToken(vtxToken_, vtxHandle); 

    edm::Handle<edm::View<reco::VertexCompositePtrCandidate>> svHandle;
    evt.getByToken(svToken_, svHandle);
    
    std::vector<float> *bresults = new std::vector<float>();
    std::vector<float> *bresultsRes = new std::vector<float>();

    for( unsigned int i = 0 ; i < jets->size() ; i++ ) {


        Ptr<pat::Jet> pjet = jets->ptrAt( i );
        pat::Jet jet = pat::Jet( *pjet );

        //variables needed for regression
        Jet_pt = jet.pt()*jet.jecFactor("Uncorrected");
        Jet_eta = jet.eta() ;
        rho = rhoFixedGrd;
        Jet_mt = sqrt(jet.energy()*jet.energy()-jet.pz()*jet.pz());//seems correct but check again


        float lepPtRel=0.,lepPtRelInv=0.,lepDeltaR=0.;
        int lepPdgID=0;

        for(const auto & d : jet.daughterPtrVector()){
            if(abs(d->pdgId())==11 || abs(d->pdgId())==13){
                auto res = calculatePtRatioRelSimple(d,pjet);
                //ptratio,ptrel,ptrelinv
                lepPtRel = std::get<1>(res);
                lepPtRelInv = std::get<2>(res);
                lepDeltaR = reco::deltaR(*d,jet);
                lepPdgID=abs(d->pdgId());
            }
        }

        Jet_leptonPtRel = lepPtRel; 
        Jet_leptonDeltaR = lepDeltaR; 
        Jet_leptonPtRelInv = lepPtRelInv;

        Jet_neHEF = jet.neutralHadronEnergyFraction();
        Jet_neEmEF = jet.neutralEmEnergyFraction();
        Jet_chHEF = jet.chargedHadronEnergyFraction();
        Jet_chEmEF = jet.chargedEmEnergyFraction();

        if (abs(lepPdgID)==13){
            isMu=1; 
        }else if (abs(lepPdgID)==11){
            isEle=1;
        }else{
            isOther=1;
        }

        Jet_mass=jet.mass();

        float ptMax=0;
        float sumWeight=0;
        float sumPt=0;
        int   nDaug03=0;

        for(const auto & d : jet.daughterPtrVector()){
            sumWeight+=(d->pt())*(d->pt());
            sumPt+=d->pt();
            if(d->pt()>ptMax) ptMax=d->pt();
            if(d->pt()>0.3) ++nDaug03;
        }
        float ptD = (sumWeight > 0) ? sqrt(sumWeight)/sumPt : 0.; 

        Jet_withPtd=ptD;
        Jet_numDaughters_pt03 = nDaug03;
        Jet_leadTrackPt = ptMax; // seems consistent with code. this is the max pt of all-pfcandidates


        // ----------- secondary vtx info
        VertexDistance3D vdist;
        float maxFoundSignificance=0;

        float vtxPt=0;
        float vtxMass=0;
        float vtx3dL=0;
        float vtx3deL=0;
        float vtxNtrk=0;
        
        const auto & pv = (*vtxHandle)[0];
        for(const auto &sv: *svHandle){
            GlobalVector flightDir(sv.vertex().x() - pv.x(), sv.vertex().y() - pv.y(),sv.vertex().z() - pv.z());
            GlobalVector jetDir(jet.px(),jet.py(),jet.pz());
            if( Geom::deltaR2( flightDir, jetDir ) < 0.09 ){
                Measurement1D dl= vdist.distance(pv,VertexState(RecoVertex::convertPos(sv.position()),RecoVertex::convertError(sv.error())));
                if(dl.significance() > maxFoundSignificance){
                    maxFoundSignificance=dl.significance();
                    vtxPt=sv.pt();
                    vtxMass=sv.p4().M();
                    vtx3dL=dl.value();
                    vtx3deL=dl.error();
                    vtxNtrk=sv.numberOfSourceCandidatePtrs();

                }   
            } 
        }
        Jet_vtxPt=vtxPt;
        Jet_vtxMass = std::max(float(0.),vtxMass);
        Jet_vtxNtrk = std::max(float(0.),vtxNtrk);
        Jet_vtx3dL  = std::max(float(0.),vtx3dL);
        Jet_vtx3deL = std::max(float(0.),vtx3deL);


        // ------------ Energy rings ------------

        float cone_boundaries[] = { 0.05, 0.1, 0.2, 0.3,0.4 }; 
        size_t ncone_boundaries = sizeof(cone_boundaries)/sizeof(float);
        std::vector<float> EmFractionEnergyRings(ncone_boundaries+1,0.);
        std::vector<float> ChFractionEnergyRings(ncone_boundaries+1,0.);
        std::vector<float> NeFractionEnergyRings(ncone_boundaries+1,0.);
        std::vector<float> MuFractionEnergyRings(ncone_boundaries+1,0.);

        for(const auto & d : jet.daughterPtrVector()){
            float candDr   = reco::deltaR(*d,jet);
            size_t icone = std::lower_bound(&cone_boundaries[0],&cone_boundaries[ncone_boundaries],candDr) - &cone_boundaries[0];
            float candEnergy = d->energy();
            int pdgid = abs(d->pdgId()) ;
            if( pdgid == 22 || pdgid == 11 ) {
                EmFractionEnergyRings[icone] += candEnergy;
            } else if ( pdgid == 13 ) { 
                MuFractionEnergyRings[icone] += candEnergy;
            } else if ( d->charge() != 0 ) {
                ChFractionEnergyRings[icone] += candEnergy;
            } else {
                NeFractionEnergyRings[icone] += candEnergy;
            } 
        } // end of jet daughters loop

        auto rawP4 = jet.correctedP4("Uncorrected");
        //if (fjet.emEnergies().size()>0){//since in order to save space we save this info only if the candidate has a minimum pt or eta
        Jet_energyRing_dR0_em_Jet_e = EmFractionEnergyRings[0]/rawP4.energy();//remember to divide by jet energy
        Jet_energyRing_dR1_em_Jet_e = EmFractionEnergyRings[1]/rawP4.energy();
        Jet_energyRing_dR2_em_Jet_e = EmFractionEnergyRings[2]/rawP4.energy();
        Jet_energyRing_dR3_em_Jet_e = EmFractionEnergyRings[3]/rawP4.energy();
        Jet_energyRing_dR4_em_Jet_e = EmFractionEnergyRings[4]/rawP4.energy();
        Jet_energyRing_dR0_neut_Jet_e = NeFractionEnergyRings[0]/rawP4.energy();
        Jet_energyRing_dR1_neut_Jet_e = NeFractionEnergyRings[1]/rawP4.energy();
        Jet_energyRing_dR2_neut_Jet_e = NeFractionEnergyRings[2]/rawP4.energy();
        Jet_energyRing_dR3_neut_Jet_e = NeFractionEnergyRings[3]/rawP4.energy();
        Jet_energyRing_dR4_neut_Jet_e = NeFractionEnergyRings[4]/rawP4.energy();
        Jet_energyRing_dR0_ch_Jet_e = ChFractionEnergyRings[0]/rawP4.energy();
        Jet_energyRing_dR1_ch_Jet_e = ChFractionEnergyRings[1]/rawP4.energy();
        Jet_energyRing_dR2_ch_Jet_e = ChFractionEnergyRings[2]/rawP4.energy();
        Jet_energyRing_dR3_ch_Jet_e = ChFractionEnergyRings[3]/rawP4.energy();
        Jet_energyRing_dR4_ch_Jet_e = ChFractionEnergyRings[4]/rawP4.energy();
        Jet_energyRing_dR0_mu_Jet_e = MuFractionEnergyRings[0]/rawP4.energy();
        Jet_energyRing_dR1_mu_Jet_e = MuFractionEnergyRings[1]/rawP4.energy();
        Jet_energyRing_dR2_mu_Jet_e = MuFractionEnergyRings[2]/rawP4.energy();
        Jet_energyRing_dR3_mu_Jet_e = MuFractionEnergyRings[3]/rawP4.energy();
        Jet_energyRing_dR4_mu_Jet_e = MuFractionEnergyRings[4]/rawP4.energy();

        // output per jet
        std::vector<float> bRegNN(3,-999);


        if(debug){
            cout<<"Jet_pt :"<<Jet_pt <<endl;
            cout<<"Jet_eta :"<<Jet_eta <<endl;
            cout<<"rho :"<<rho <<endl;
            cout<<"Jet_mt :"<<Jet_mt <<endl;
            cout<<"Jet_leadTrackPt :"<<Jet_leadTrackPt <<endl;
            cout<<"Jet_leptonPtRel :"<<Jet_leptonPtRel <<endl;
            cout<<"Jet_leptonDeltaR :"<<Jet_leptonDeltaR <<endl;
            cout<<"Jet_neHEF :"<<Jet_neHEF <<endl;
            cout<<"Jet_neEmEF :"<<Jet_neEmEF <<endl;
            cout<<"Jet_vtxPt :"<<Jet_vtxPt <<endl;
            cout<<"Jet_vtxMass :"<<Jet_vtxMass <<endl;
            cout<<"Jet_vtx3dL :"<<Jet_vtx3dL <<endl;
            cout<<"Jet_vtxNtrk :"<<Jet_vtxNtrk <<endl;
            cout<<"Jet_vtx3deL :"<<Jet_vtx3deL <<endl;
            cout<<"Jet_energyRing_dR0_em_Jet_e :"<<Jet_energyRing_dR0_em_Jet_e <<endl;
            cout<<"Jet_energyRing_dR1_em_Jet_e :"<<Jet_energyRing_dR1_em_Jet_e <<endl;
            cout<<"Jet_energyRing_dR2_em_Jet_e :"<<Jet_energyRing_dR2_em_Jet_e <<endl;
            cout<<"Jet_energyRing_dR3_em_Jet_e :"<<Jet_energyRing_dR3_em_Jet_e <<endl;
            cout<<"Jet_energyRing_dR4_em_Jet_e :"<<Jet_energyRing_dR4_em_Jet_e <<endl;
            cout<<"Jet_energyRing_dR0_neut_Jet_e :"<<Jet_energyRing_dR0_neut_Jet_e <<endl;
            cout<<"Jet_energyRing_dR1_neut_Jet_e :"<<Jet_energyRing_dR1_neut_Jet_e <<endl;
            cout<<"Jet_energyRing_dR2_neut_Jet_e :"<<Jet_energyRing_dR2_neut_Jet_e <<endl;
            cout<<"Jet_energyRing_dR3_neut_Jet_e :"<<Jet_energyRing_dR3_neut_Jet_e <<endl;
            cout<<"Jet_energyRing_dR4_neut_Jet_e :"<<Jet_energyRing_dR4_neut_Jet_e <<endl;
            cout<<"Jet_energyRing_dR0_ch_Jet_e :"<<Jet_energyRing_dR0_ch_Jet_e <<endl;
            cout<<"Jet_energyRing_dR1_ch_Jet_e :"<<Jet_energyRing_dR1_ch_Jet_e <<endl;
            cout<<"Jet_energyRing_dR2_ch_Jet_e :"<<Jet_energyRing_dR2_ch_Jet_e <<endl;
            cout<<"Jet_energyRing_dR3_ch_Jet_e :"<<Jet_energyRing_dR3_ch_Jet_e <<endl;
            cout<<"Jet_energyRing_dR4_ch_Jet_e :"<<Jet_energyRing_dR4_ch_Jet_e <<endl;
            cout<<"Jet_energyRing_dR0_mu_Jet_e :"<<Jet_energyRing_dR0_mu_Jet_e <<endl;
            cout<<"Jet_energyRing_dR1_mu_Jet_e :"<<Jet_energyRing_dR1_mu_Jet_e <<endl;
            cout<<"Jet_energyRing_dR2_mu_Jet_e :"<<Jet_energyRing_dR2_mu_Jet_e <<endl;
            cout<<"Jet_energyRing_dR3_mu_Jet_e :"<<Jet_energyRing_dR3_mu_Jet_e <<endl;
            cout<<"Jet_energyRing_dR4_mu_Jet_e :"<<Jet_energyRing_dR4_mu_Jet_e <<endl;
            cout<<"Jet_numDaughters_pt03 :"<<Jet_numDaughters_pt03 <<endl;



        }

        SetNNVectorVar();
        bRegNN = EvaluateNN();
        NNvectorVar_.clear();

        //FIXME read through file, config is here /afs/cern.ch/user/n/nchernya/public/100M_2018-03-01_job23_rawJetsJECtarget/config.json
        //            float y_mean= 1.0454729795455933;
        //            float y_std = 0.3162831664085388;

        //fjet.addUserFloat("bRegNNCorr", bRegNN[0]*y_std_+y_mean_);
        //fjet.addUserFloat("bRegNNResolution",0.5*(bRegNN[2]-bRegNN[1])*y_std_);

        bresults->push_back( bRegNN[0]*y_std_+y_mean_);
        bresultsRes->push_back( 0.5*(bRegNN[2]-bRegNN[1])*y_std_);

        if (debug) std::cout<<"NN jet "<< int(bresults->size()-1)<<": "<< bresults->at(bresults->size()-1)<< " +/- "<< bresultsRes->at(bresults->size()-1)<<std::endl;


    }

    //evt.put( std::move( jetColl ) );
    putInEvent("bRegression",jets,bresults,evt);
    putInEvent("bRegressionResolution",jets,bresultsRes,evt);
}

void bRegressionProducer::InitJet(){
    Jet_pt = 0.;
    Jet_eta = 0.;
    rho = 0.;
    Jet_mt = 0.;
    Jet_leadTrackPt = 0.;
    Jet_leptonPtRel = 0.;
    Jet_leptonDeltaR = 0.;
    Jet_neHEF = 0.;
    Jet_neEmEF = 0.;
    Jet_vtxPt = 0.;
    Jet_vtxMass = 0.;
    Jet_vtx3dL = 0.;
    Jet_vtxNtrk = 0.;
    Jet_vtx3deL = 0.;
    Jet_numDaughters_pt03 = 0;
    Jet_energyRing_dR0_em_Jet_e = 0.;
    Jet_energyRing_dR1_em_Jet_e = 0.;
    Jet_energyRing_dR2_em_Jet_e = 0.;
    Jet_energyRing_dR3_em_Jet_e = 0.;
    Jet_energyRing_dR4_em_Jet_e = 0.;
    Jet_energyRing_dR0_neut_Jet_e = 0.;
    Jet_energyRing_dR1_neut_Jet_e = 0.;
    Jet_energyRing_dR2_neut_Jet_e = 0.;
    Jet_energyRing_dR3_neut_Jet_e = 0.;
    Jet_energyRing_dR4_neut_Jet_e = 0.;
    Jet_energyRing_dR0_ch_Jet_e = 0.;
    Jet_energyRing_dR1_ch_Jet_e = 0.;
    Jet_energyRing_dR2_ch_Jet_e = 0.;
    Jet_energyRing_dR3_ch_Jet_e = 0.;
    Jet_energyRing_dR4_ch_Jet_e = 0.;
    Jet_energyRing_dR0_mu_Jet_e = 0.;
    Jet_energyRing_dR1_mu_Jet_e = 0.;
    Jet_energyRing_dR2_mu_Jet_e = 0.;
    Jet_energyRing_dR3_mu_Jet_e = 0.;
    Jet_energyRing_dR4_mu_Jet_e = 0.;
    Jet_chHEF = 0.;//implement from here
    Jet_chEmEF = 0.;
    Jet_leptonPtRelInv = 0.;
    isEle = 0.;
    isMu = 0.;
    isOther = 0.;
    Jet_mass = 0.;
    Jet_withPtd = 0.;


}//end InitJet


void bRegressionProducer::SetNNVectorVar(){

    NNvectorVar_.push_back(Jet_pt) ;//0
    NNvectorVar_.push_back(Jet_eta) ;
    NNvectorVar_.push_back(rho) ;
    NNvectorVar_.push_back(Jet_mt) ;
    NNvectorVar_.push_back(Jet_leadTrackPt) ;
    NNvectorVar_.push_back(Jet_leptonPtRel) ;//5
    NNvectorVar_.push_back(Jet_leptonDeltaR) ;
    NNvectorVar_.push_back(Jet_neHEF) ;
    NNvectorVar_.push_back(Jet_neEmEF) ;
    NNvectorVar_.push_back(Jet_vtxPt) ;
    NNvectorVar_.push_back(Jet_vtxMass) ;//10
    NNvectorVar_.push_back(Jet_vtx3dL) ;
    NNvectorVar_.push_back(Jet_vtxNtrk) ;
    NNvectorVar_.push_back(Jet_vtx3deL) ;
    NNvectorVar_.push_back(Jet_numDaughters_pt03) ;//this variable has changed order, in bdt it was last, check why
    NNvectorVar_.push_back(Jet_energyRing_dR0_em_Jet_e) ;//15
    NNvectorVar_.push_back(Jet_energyRing_dR1_em_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR2_em_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR3_em_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR4_em_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR0_neut_Jet_e) ;//20
    NNvectorVar_.push_back(Jet_energyRing_dR1_neut_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR2_neut_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR3_neut_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR4_neut_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR0_ch_Jet_e) ;//25
    NNvectorVar_.push_back(Jet_energyRing_dR1_ch_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR2_ch_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR3_ch_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR4_ch_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR0_mu_Jet_e) ;//30
    NNvectorVar_.push_back(Jet_energyRing_dR1_mu_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR2_mu_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR3_mu_Jet_e) ;
    NNvectorVar_.push_back(Jet_energyRing_dR4_mu_Jet_e) ;
    NNvectorVar_.push_back(Jet_chHEF);//35
    NNvectorVar_.push_back(Jet_chEmEF);
    NNvectorVar_.push_back(Jet_leptonPtRelInv);
    NNvectorVar_.push_back(isEle);
    NNvectorVar_.push_back(isMu);
    NNvectorVar_.push_back(isOther);//40
    NNvectorVar_.push_back(Jet_mass);
    NNvectorVar_.push_back(Jet_withPtd);
    
    if (debug) std::cout<<"NNvector size="<<NNvectorVar_.size()<<std::endl;
}

std::vector<float> bRegressionProducer::EvaluateNN(){
    long long int NN=NNvectorVar_.size();
    tensorflow::Tensor input(tensorflow::DT_FLOAT, {1,NN});
    for (unsigned int i = 0; i < NNvectorVar_.size(); i++){
        //            std::cout<<"i:"<<i<<" x:"<<NNvectorVar_[i]<<std::endl;
        input.matrix<float>()(0,i) =  float(NNvectorVar_[i]);
    }
    std::vector<tensorflow::Tensor> outputs;
    tensorflow::run(session, { { "ffwd_inp:0",input } }, { "ffwd_out/BiasAdd:0" }, &outputs);

    std::vector<float> correction(3);//3 outputs, first value is mean and then other 2 quantiles
    correction[0] = outputs[0].matrix<float>()(0, 0);
    correction[1] = outputs[0].matrix<float>()(0, 1);
    correction[2] = outputs[0].matrix<float>()(0, 2);

    //        std::cout<<correction[0]<<" "<<correction[1]<<" "<<correction[2]<<std::endl;

    return correction;

}//end EvaluateNN

template <typename T,typename J> void bRegressionProducer::putInEvent(std::string name, const edm::Handle< J >& jets, std::vector<T>* product, edm::Event& iEvent){
  std::unique_ptr<edm::ValueMap<T>> out(new edm::ValueMap<T>());
  typename edm::ValueMap<T>::Filler filler(*out);
  filler.insert(jets, product->begin(), product->end());
  filler.fill();
  iEvent.put(std::move(out), name);
  delete product;
}

DEFINE_FWK_MODULE( bRegressionProducer );

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4


