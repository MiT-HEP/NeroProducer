// -*- C++ -*-
//
// Package:    SuperClusterFootprintRemoval
// Class:      SuperClusterFootprintRemoval
// 
/**\class SuperClusterFootprintRemoval SuperClusterFootprintRemoval.cc PFIsolation/SuperClusterFootprintRemoval/plugins/SuperClusterFootprintRemoval.cc

 Description: Implements the algo for removal of PF clusters from the SC footprint

 Implementation:
     Runs on AOD.
*/
//
// Original Author:  Marco Peruzzi,32 4-C16,+41227676829,
//         Created:  Sat Sep 29 17:58:21 CEST 2012
// $Id: SuperClusterFootprintRemoval.h,v 1.4 2012/12/12 15:00:30 peruzzi Exp $
//
//


// system include files
#include <memory>
#include <iostream>
#include <cmath>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"
#include "DataFormats/CaloRecHit/interface/CaloCluster.h"
#include "DataFormats/CaloRecHit/interface/CaloClusterFwd.h"
#include "Geometry/EcalAlgo/interface/EcalBarrelGeometry.h"
#include "Geometry/EcalAlgo/interface/EcalEndcapGeometry.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidateFwd.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"
#include "DataFormats/GsfTrackReco/interface/GsfTrack.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/EgammaCandidates/interface/PhotonFwd.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "TRandom3.h"
#include "TGeoTube.h"
#include "TGeoPara.h"
#include "TVector3.h"
#include "TMath.h"
#include "TRotation.h"
#include "TString.h"

#ifndef __SUPERCLUSTERFOOTPRINTREMOVALMINIAOD__HH__
#define __SUPERCLUSTERFOOTPRINTREMOVALMINIAOD__HH__


//
// constants, enums and typedefs
//

typedef struct {
  int nxtals;
  std::vector<TVector3> xtalposition;
  std::vector<float> xtaletawidth;
  std::vector<float> xtalphiwidth;
  std::vector<std::vector<TVector3> > xtalcorners;
} sc_xtal_information;

typedef struct {
  float dR;
  float dEta;
  float dPhi;
} angular_distances_struct;

typedef struct {
  float chargediso;
  float chargediso_primvtx;
  float neutraliso;
  float photoniso;
  float chargediso_rcone;
  float chargediso_primvtx_rcone;
  float neutraliso_rcone;
  float photoniso_rcone;
  float eta_rcone;
  float phi_rcone;
  bool  rcone_isOK;
} PFIsolation_struct;

//
// class declaration
//

class SuperClusterFootprintRemovalMiniAOD {

public:

//    The constructor can take the following arguments from configuration (default value):
//
//    isolation_cone_size_forSCremoval (0.4) : isolation cone used for PF Iso calculation
//    tag_pfCandidates_forSCremoval ("particleFlow") : collection of PF candidates to use
//    tag_jets ("ak5PFJets") : collection of PF jets to use for vetoing around random cone direction
//    rechit_link_enlargement_forSCremoval (0.25) : enlargement of linear dimension of xtals for rechit matching
  SuperClusterFootprintRemovalMiniAOD(
		  edm::ConsumesCollector && cc, 
		  edm::ParameterSet iConfig = edm::ParameterSet()
		  );

  ~SuperClusterFootprintRemovalMiniAOD();

  // Calculate the PF isolation around the object consisting of the SuperCluster sc. Component can be "neutral","charged" or "photon".
  // The vertexforchargediso Ptr should be passed to tell with respect to which vertex we should cut on dxy (0.1 cm) and dz (0.2 cm) of the track associated to charged PF candidate (no cut applied if the Ptr is null). Ideally, this should be the vertex from which the object consisting of the SuperCluster sc comes from.
  PFIsolation_struct PFIsolation(reco::SuperClusterRef sc_, edm::Ptr<reco::Vertex> vertexforchargediso = edm::Ptr<reco::Vertex>());

  void beginRun(edm::Run const &run, edm::EventSetup const &iSetup);

private:

  PFIsolation_struct PFIsolation_worker(reco::SuperClusterRef sc_, edm::Ptr<reco::Vertex> vertexforchargediso = edm::Ptr<reco::Vertex>(), float rotation_phi=0, bool is_recursive_rcone=false);
  TVector3 PropagatePFCandToEcal(int pfcandindex, float position, bool isbarrel);
  sc_xtal_information GetSCXtalInfo(reco::SuperClusterRef sc);
  //bool CheckMatchedPFCandidate(int pfcandindex);
  bool FindCloseJetsAndPhotons(float rotation_phi);
  void RandomConeIsolation(edm::Ptr<reco::Vertex> vertexforchargediso = edm::Ptr<reco::Vertex>(), PFIsolation_struct *output=NULL);
  bool CheckPFCandInFootprint(int pfcandindex, float rotation_phi);
  angular_distances_struct GetPFCandHitDistanceFromSC(int pfcandindex, float rotation_phi);

  CaloSubdetectorGeometry *barrelGeometry;
  CaloSubdetectorGeometry *endcapGeometry;
  TGeoPara eegeom;
  MagneticField *magField;

  TRandom3 *randomgen;

  reco::SuperClusterRef sc;
  sc_xtal_information infos;

  double   photonptMinRC;
  double   jetptMinRC;
  double   leptonptMinRC;


  double global_linkbyrechit_enlargement;
  double global_isolation_cone_size;

  int FindPFCandType(int id);

public:

  // set handles, avoiding to have them in the constructor
  // where only the token should be.
  // I'll do this outsite

  
  void Config( const edm::EventSetup&);

  inline void SetHandles(
  	const edm::Handle<pat::PackedCandidateCollection> &pfCandidatesIn,
 	const edm::Handle<pat::PhotonCollection> & photonHandleIn,
  	const edm::Handle<pat::JetCollection> &jetHandleIn,
  	const edm::Handle<pat::MuonCollection> &muonHandleIn,
  	const edm::Handle<pat::ElectronCollection> &electronHandleIn)
  	{
  	pfCandidates = pfCandidatesIn;
  	photonHandle = photonHandleIn;
  	jetHandle    = jetHandleIn;
  	muonHandle   = muonHandleIn;
  	electronHandle = electronHandleIn;
	}

  edm::Handle<pat::PackedCandidateCollection> pfCandidates;  
  edm::Handle<pat::PhotonCollection> photonHandle;
  edm::Handle<pat::JetCollection> jetHandle;
  edm::Handle<pat::MuonCollection> muonHandle;
  edm::Handle<pat::ElectronCollection> electronHandle;
  
};

#endif

