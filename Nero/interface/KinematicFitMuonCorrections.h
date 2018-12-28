#ifndef KINEMATIC_FIT_MUON_CORRECTIONS
#define KINEMATIC_FIT_MUON_CORRECTIONS

// Kinematic fit class 
// PB - 10.09.2018
// More info about KinFit in https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideKinematicVertexFit 

#include "TrackingTools/PatternTools/interface/ClosestApproachInRPhi.h"
#include "TrackingTools/PatternTools/interface/TSCBLBuilderNoMaterial.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"

#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "MagneticField/ParametrizedEngine/src/OAEParametrizedMagneticField.h"

#include "RecoVertex/KinematicFitPrimitives/interface/ParticleMass.h"
#include "RecoVertex/KinematicFitPrimitives/interface/MultiTrackKinematicConstraint.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParticleFactoryFromTransientTrack.h"
#include "RecoVertex/KinematicFit/interface/KinematicConstrainedVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/TwoTrackMassKinematicConstraint.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleVertexFitter.h"
#include "RecoVertex/KinematicFit/interface/KinematicParticleFitter.h"
#include "RecoVertex/KinematicFit/interface/MassKinematicConstraint.h"
#include "RecoVertex/KinematicFitPrimitives/interface/RefCountedKinematicParticle.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicVertex.h"
#include "RecoVertex/KinematicFitPrimitives/interface/KinematicParametersError.h"
#include "RecoVertex/VertexTools/interface/VertexDistance3D.h"
#include "RecoVertex/VertexTools/interface/VertexDistanceXY.h"

class KinematicVertexFitter {

  public: 
    KinematicVertexFitter() {};
    virtual ~KinematicVertexFitter() {};

    //The reconstructed muon trajectories 
    reco::TransientTrack getTransientTrack(const reco::TrackRef& trackRef) {
      reco::TransientTrack transientTrack(trackRef, paramField);
      return transientTrack;
    }
 
    RefCountedKinematicTree Fit( const pat::MuonCollection & pat_muons) {

      //Creating a KinematicParticleFactory
      KinematicParticleFactoryFromTransientTrack pFactory;
      
      //Making particles
      std::vector<RefCountedKinematicParticle> muons_for_kinematic_fit;

      //Passing transient muon tracks to the fitter
      std::vector<reco::TransientTrack> muon_tracks;
      for (pat::MuonCollection::const_iterator imu = pat_muons.begin(); imu != pat_muons.end(); ++imu){
        muons_for_kinematic_fit.push_back(pFactory.particle(getTransientTrack(imu->track()), muon_mass, chi, ndf, muon_mass_sigma));
      }

      //Fitting.
      KinematicConstrainedVertexFitter kvFitterDiMuons;
      RefCountedKinematicTree X2MuMuKinFitTree = kvFitterDiMuons.fit(muons_for_kinematic_fit);

      //Return a kinematic fit tree. Careful on manipulating it: see https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideKinematicVertexFit      
      return X2MuMuKinFitTree;
    }

  private:

    OAEParametrizedMagneticField *paramField = new OAEParametrizedMagneticField("3_8T");
    // The mass of a muon and the insignificant mass sigma to avoid singularities in the covariance matrix.
    float const muon_mass = 0.105658367; 
    float muon_mass_sigma = muon_mass * 1.e-6;
    // initial chi2 and ndf before kinematic fits. The chi2 of the reconstruction is not considered
    float chi = 0.;
    float ndf = 0.; 


};

#endif //#ifndef
