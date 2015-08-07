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
// $Id: SuperClusterFootprintRemoval.cc,v 1.10 2013/05/15 12:57:16 peruzzi Exp $
//
//


#ifndef __SUPERCLUSTERFOOTPRINTREMOVALMINIAOD__CC__
#define __SUPERCLUSTERFOOTPRINTREMOVALMINIAOD__CC__

#include "NeroProducer/Nero/interface/SuperClusterFootprintRemovalMiniAOD.h"

using namespace std;

//
// constructors and destructor
//
void SuperClusterFootprintRemovalMiniAOD::Config(const edm::EventSetup& iSetup){
  edm::ESHandle<CaloGeometry> geometry ;
  iSetup.get<CaloGeometryRecord>().get(geometry);
  barrelGeometry = (CaloSubdetectorGeometry*)(geometry->getSubdetectorGeometry(DetId::Ecal, EcalBarrel));
  endcapGeometry = (CaloSubdetectorGeometry*)(geometry->getSubdetectorGeometry(DetId::Ecal, EcalEndcap));

  edm::ESHandle<MagneticField> magneticField;
  iSetup.get<IdealMagneticFieldRecord>().get(magneticField);
  magField = (MagneticField*)(magneticField.product());

}

SuperClusterFootprintRemovalMiniAOD::SuperClusterFootprintRemovalMiniAOD(
		edm::ConsumesCollector && cc,
		edm::ParameterSet iConfig
		)
{
   //now do what ever initialization is needed

  eegeom = TGeoPara(1.,1.,1.,0.,0.,0.);

  global_isolation_cone_size = iConfig.getUntrackedParameter<double>("isolation_cone_size_forSCremoval",0.3);
  global_linkbyrechit_enlargement = iConfig.getUntrackedParameter<double>("rechit_link_enlargement_forSCremoval",0.25);


  // never set this to 0
  photonptMinRC = 10;
  jetptMinRC = 20;
  leptonptMinRC = 20;
  
  randomgen = new TRandom3(0);

}

void SuperClusterFootprintRemovalMiniAOD::beginRun(edm::Run const &run, edm::EventSetup const &iSetup){
}


SuperClusterFootprintRemovalMiniAOD::~SuperClusterFootprintRemovalMiniAOD()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

  delete randomgen;

}


//
// member functions
//


TVector3 SuperClusterFootprintRemovalMiniAOD::PropagatePFCandToEcal(int pfcandindex, float position, bool isbarrel){
  // WARNING: this propagates until EE+ or EE- at the given TMath::Abs(position.z()) for isbarrel=0, depending on where the candidate is pointing.

  int type = FindPFCandType((*pfCandidates)[pfcandindex].pdgId());

  if (!((*pfCandidates)[pfcandindex].pt()>0)) {
    std::cout << "Warning: called propagation to ECAL for object with negative or zero pt. Returning TVector3(0,0,1e10)." << std::endl;
    return TVector3(0,0,1e10);
  }

  if (type>2) {
    std::cout << "Asking propagation for lepton, not implemented. Returning TVector3(0,0,1e10)." << std::endl;
    return TVector3(0,0,1e10);
  }

  TVector3 pfvertex((*pfCandidates)[pfcandindex].vx(),(*pfCandidates)[pfcandindex].vy(),(*pfCandidates)[pfcandindex].vz());
  TVector3 pfmomentum((*pfCandidates)[pfcandindex].px(),(*pfCandidates)[pfcandindex].py(),(*pfCandidates)[pfcandindex].pz());
  pfmomentum = pfmomentum.Unit();

  TVector3 ecalpfhit(0,0,0);
  if (isbarrel){
    double p[3] = {pfvertex.x(),pfvertex.y(),pfvertex.z()};
    double d[3] = {pfmomentum.x(),pfmomentum.y(),pfmomentum.z()};
    double dist = TGeoTube::DistFromInsideS(p,d,0,position,1e+10);
    ecalpfhit = pfvertex + dist*pfmomentum;
  }
  else { // EE
    double dim[6]={1e+10,1e+10,fabs(position),0,0,0};
    double p[3] = {pfvertex.x(),pfvertex.y(),pfvertex.z()};
    double d[3] = {pfmomentum.x(),pfmomentum.y(),pfmomentum.z()};
    eegeom.SetDimensions(dim);
    double dist = eegeom.DistFromInside(p,d);
    ecalpfhit = pfvertex + dist*pfmomentum;
  }

  if (type==1){ // APPROXIMATE helix propagation
    float field = magField->inTesla(GlobalPoint(0.,0.,0.)).z();
    float charge = (*pfCandidates)[pfcandindex].charge();
    float curvature = ((*pfCandidates)[pfcandindex].pt())/(0.3*field*charge)*100; // curvature radius in cm
    float final_radius = ecalpfhit.Perp();
    float addphi = -TMath::ASin(final_radius/curvature/2);

    TRotation r;
    r.RotateZ(addphi);
    ecalpfhit *= r;

  }

  return ecalpfhit;

}

sc_xtal_information SuperClusterFootprintRemovalMiniAOD::GetSCXtalInfo(reco::SuperClusterRef sc){

  sc_xtal_information out;

  
  std::vector<DetId> cristal;   
      
  for (reco::CaloCluster_iterator bc=sc->clustersBegin(); bc!=sc->clustersEnd(); ++bc){
    const std::vector< std::pair<DetId, float> > & seedrechits = (*bc)->hitsAndFractions();
    for (unsigned int i=0; i<seedrechits.size(); i++) cristal.push_back(seedrechits[i].first);
    sort(cristal.begin(),cristal.end());
    std::vector<DetId>::iterator it;
    it = unique(cristal.begin(),cristal.end());
    cristal.resize(it-cristal.begin());
  }
            
  unsigned int i=0;
  for (i=0; i<cristal.size(); i++){

    if (cristal.at(i).subdetId()!=EcalBarrel && cristal.at(i).subdetId()!=EcalEndcap) continue;
    bool isbarrel = (cristal.at(i).subdetId()==EcalBarrel);

    CaloCellGeometry *cellGeometry = NULL;
    EBDetId ebDetId;
    EEDetId eeDetId;
    if (isbarrel) {
      ebDetId = cristal.at(i); 
      cellGeometry = (CaloCellGeometry*)(barrelGeometry->getGeometry(ebDetId));
    }
    else {
      eeDetId = cristal.at(i);
      cellGeometry = (CaloCellGeometry*)(endcapGeometry->getGeometry(eeDetId));
    }
    TVector3 xtal_position(cellGeometry->getPosition().x(),cellGeometry->getPosition().y(),cellGeometry->getPosition().z());

    float dphi; float deta;
    if (isbarrel){
    dphi=(dynamic_cast<const EcalBarrelGeometry*>(barrelGeometry))->deltaPhi(ebDetId);
    deta=(dynamic_cast<const EcalBarrelGeometry*>(barrelGeometry))->deltaEta(ebDetId);
    }
    else {
    dphi=(dynamic_cast<const EcalEndcapGeometry*>(endcapGeometry))->deltaPhi(eeDetId);
    deta=(dynamic_cast<const EcalEndcapGeometry*>(endcapGeometry))->deltaEta(eeDetId);
    }
    
    const CaloCellGeometry::CornersVec& cellCorners (cellGeometry->getCorners());
    std::vector<TVector3> corners;
    for (int k=0; k<4; k++){
      TVector3 thiscorner((float)(cellCorners[k].x()),(float)(cellCorners[k].y()),(float)(cellCorners[k].z()));
      corners.push_back(thiscorner);
    }
    out.xtalposition.push_back(xtal_position);
    out.xtaletawidth.push_back(deta);
    out.xtalphiwidth.push_back(dphi);
    out.xtalcorners.push_back(corners);
  }
  out.nxtals=i;

  assert (out.nxtals==(int)out.xtalposition.size());

  return out;

}

/*
bool SuperClusterFootprintRemovalMiniAOD::CheckMatchedPFCandidate(int i){

  if ((*pfCandidates)[i].pdgId()==22) {
    if ((*pfCandidates)[i].mva_nothing_gamma()>0){
      if( (*pfCandidates)[i].superClusterRef()==sc) {
	return true;
      }
    }
  }
  
  return false;

}
*/

bool SuperClusterFootprintRemovalMiniAOD::CheckPFCandInFootprint(int i, float rotation_phi){

  bool isbarrel = (fabs(sc->eta())<1.5);

  //check BUT was done in R1 for footprint removal INSIDE true photon supercluster
  //now with GED that step should be obsolete
  //if (CheckMatchedPFCandidate(i)) return true;

  if (!((*pfCandidates)[i].pt()>0)) return false;

    int type = FindPFCandType((*pfCandidates)[i].pdgId());
    if (type>2) return false;

    bool inside=false;

    for (int j=0; j<infos.nxtals; j++){
      
      TVector3 xtal_position = infos.xtalposition[j];
      if (rotation_phi!=0) {
	TRotation r; r.RotateZ(rotation_phi);
	xtal_position *= r;
      }
      
      TVector3 ecalpfhit = PropagatePFCandToEcal(i,isbarrel ? xtal_position.Perp() : xtal_position.z(), isbarrel);

      if (ecalpfhit.Perp()==0) continue;
      
      if (isbarrel){
	float xtalEtaWidth = infos.xtaletawidth[j]*(1.0+global_linkbyrechit_enlargement);
	float xtalPhiWidth = infos.xtalphiwidth[j]*(1.0+global_linkbyrechit_enlargement);
	float deta = ecalpfhit.Eta()-xtal_position.Eta();
	float dphi = reco::deltaPhi(ecalpfhit.Phi(),xtal_position.Phi());
	if (fabs(deta)<xtalEtaWidth/2 && fabs(dphi)<xtalPhiWidth/2) inside=true;
      }
      else { // EE
	if (ecalpfhit.z()*xtal_position.z()>0){
	  TVector3 xtal_corners[4];
	  for (int k=0; k<4; k++) xtal_corners[k] = infos.xtalcorners[j][k];
	  float hitx = ecalpfhit.x();
	  float hity = ecalpfhit.y();
	  float polx[5];
	  float poly[5];
	  for (int k=0; k<4; k++) polx[k] = xtal_corners[k].x();
	  for (int k=0; k<4; k++) poly[k] = xtal_corners[k].y();
	  polx[4]=polx[0]; poly[4]=poly[0]; // closed polygon
	  float centerx = (polx[0]+polx[1]+polx[2]+polx[3])/4;
	  float centery = (poly[0]+poly[1]+poly[2]+poly[3])/4;
	  hitx = centerx + (hitx-centerx)/(1.0+global_linkbyrechit_enlargement);
	  hity = centery + (hity-centery)/(1.0+global_linkbyrechit_enlargement);
	  if (TMath::IsInside(hitx,hity,5,polx,poly)) inside=true;
	}
      }
      
    }

    return inside;

}


angular_distances_struct SuperClusterFootprintRemovalMiniAOD::GetPFCandHitDistanceFromSC(int pfindex, float rotation_phi){

  int type = FindPFCandType((*pfCandidates)[pfindex].pdgId());

  if (type>2) {
    std::cout << "propagation not implemented for lepton objects!!!" << std::endl;
    angular_distances_struct out;
    out.dR=999; out.dEta=999; out.dPhi=999;
    return out;
  }

  bool isbarrel = (fabs(sc->eta())<1.5);

  TVector3 sc_position = TVector3(sc->x(),sc->y(),sc->z());
  if (rotation_phi!=0) {
    TRotation r; r.RotateZ(rotation_phi);
    sc_position *= r;
  }

  TVector3 pfvertex((*pfCandidates)[pfindex].vx(),(*pfCandidates)[pfindex].vy(),(*pfCandidates)[pfindex].vz());
  TVector3 pfmomentum((*pfCandidates)[pfindex].px(),(*pfCandidates)[pfindex].py(),(*pfCandidates)[pfindex].pz());
  pfmomentum = pfmomentum.Unit();

  TVector3 ecalpfhit = PropagatePFCandToEcal(pfindex,isbarrel ? sc_position.Perp() : sc_position.z(),isbarrel);

  if (ecalpfhit.Perp()==0 or ecalpfhit.Perp() >  1.e+10 or ecalpfhit.Pz() > 1e+10){ // I had   -> ecalpfhit.Perp()2.36219e+29 in data ?!?  pz = inf ?!?
    std::cout << "GetPFCandHitDistanceFromSC: impact position found in the origin of the transverse plane. Returning error state." << std::endl;
    angular_distances_struct out;
    out.dR = 999;
    out.dEta = 999;
    out.dPhi = 999;
    return out;
  }

  angular_distances_struct out;
  out.dR = reco::deltaR(sc_position.Eta(),sc_position.Phi(),ecalpfhit.Eta(),ecalpfhit.Phi());
  out.dEta = ecalpfhit.Eta()-sc_position.Eta();
  out.dPhi = reco::deltaPhi(ecalpfhit.Phi(),sc_position.Phi());

  return out;

}

int SuperClusterFootprintRemovalMiniAOD::FindPFCandType(int id){

  int type = -1;

  if (id==111 || id==130 || id==310 || id==2112) type=0; //neutral hadrons
  if (fabs(id)==211 || fabs(id)==321 || id==999211 || fabs(id)==2212) type=1; //charged hadrons
  if (id==22) type=2; //photons
  if (fabs(id)==11) type=3; //electrons
  if (fabs(id)==13) type=4; //muons

  return type;
}

PFIsolation_struct SuperClusterFootprintRemovalMiniAOD::PFIsolation(reco::SuperClusterRef sc_, edm::Ptr<reco::Vertex> vertexforchargediso){
  return PFIsolation_worker(sc_,vertexforchargediso);
}

PFIsolation_struct SuperClusterFootprintRemovalMiniAOD::PFIsolation_worker(reco::SuperClusterRef sc_, edm::Ptr<reco::Vertex> vertexforchargediso, float rotation_phi, bool is_recursive_rcone){

  if (!is_recursive_rcone){
    sc = sc_;
    infos = GetSCXtalInfo(sc);
  }
  
  PFIsolation_struct out;
  out.chargediso=0;
  out.chargediso_primvtx=0;
  out.neutraliso=0;
  out.photoniso=0;
  out.chargediso_rcone=999;
  out.chargediso_primvtx_rcone=999;
  out.neutraliso_rcone=999;
  out.photoniso_rcone=999;
  out.eta_rcone=999;
  out.phi_rcone=999;
  out.rcone_isOK=false;
  
  bool isbarrel = (fabs(sc->eta())<1.5);
  
  for (unsigned int i=0; i<pfCandidates->size(); i++){

    if (!((*pfCandidates)[i].pt()>0)) continue;

    float pfcandabseta = fabs((*pfCandidates)[i].eta());
    if (pfcandabseta>2.5) continue;
    if (isbarrel && pfcandabseta>1.4442) continue;
    if (!isbarrel && pfcandabseta<1.566) continue;

    int type = FindPFCandType((*pfCandidates)[i].pdgId());
    if (type>2) continue;

    angular_distances_struct distance = GetPFCandHitDistanceFromSC(i,rotation_phi);
    if (distance.dR>global_isolation_cone_size) continue;

    if (CheckPFCandInFootprint(i,rotation_phi)) continue;

    if (type==0) out.neutraliso+=(*pfCandidates)[i].pt(); 
    if (type==1) out.chargediso+=(*pfCandidates)[i].pt();
    if (type==2) out.photoniso+=(*pfCandidates)[i].pt();
	
    float dxy=0;
    float dz=0;

    if (type==1 && vertexforchargediso.isNonnull()){
      TVector3 pfvertex((*pfCandidates)[i].vx(),(*pfCandidates)[i].vy(),(*pfCandidates)[i].vz());
      if((*pfCandidates)[i].bestTrack()==NULL){
        //can calculate dxy,dz only with available info, for candidates with track info available both functions lead to the same result
        dxy=(*pfCandidates)[i].dxy(vertexforchargediso.get()->position());
        dz=(*pfCandidates)[i].dz(vertexforchargediso.get()->position());
      }else{
        TVector3 vtxmom((*pfCandidates)[i].bestTrack()->px(),(*pfCandidates)[i].bestTrack()->py(),(*pfCandidates)[i].bestTrack()->pz());
        TVector3 phovtx(vertexforchargediso.get()->x(),vertexforchargediso.get()->y(),vertexforchargediso.get()->z());
        dxy = ( -(pfvertex.x()-phovtx.x())*vtxmom.y() +(pfvertex.y()-phovtx.y())*vtxmom.x() ) / vtxmom.Perp();
        dz = (pfvertex.z()-phovtx.z()) - ( (pfvertex.x()-phovtx.x())*vtxmom.x() + (pfvertex.y()-phovtx.y())*vtxmom.y() ) / vtxmom.Perp() * vtxmom.z() / vtxmom.Perp();
      }

      dxy=fabs(dxy);
      dz=fabs(dz);
      if (dz<0.2 && dxy<0.1) out.chargediso_primvtx+=(*pfCandidates)[i].pt();
    } // end type==1

  }

  if (!is_recursive_rcone) RandomConeIsolation(vertexforchargediso,&out);
  return out;

}

bool SuperClusterFootprintRemovalMiniAOD::FindCloseJetsAndPhotons(float rotation_phi){

  TVector3 photon_position = TVector3(sc->x(),sc->y(),sc->z());
  if (rotation_phi!=0) {
    TRotation r; r.RotateZ(rotation_phi);
    photon_position *= r;
  }
  double eta = photon_position.Eta();
  double phi = photon_position.Phi();
  
  const float mindR = 0.8;
  bool found=false;

  for (pat::JetCollection::const_iterator jet=jetHandle->begin(); jet!=jetHandle->end(); jet++){
    if (jet->pt()<jetptMinRC) continue;
    float dR = reco::deltaR(eta,phi,jet->eta(),jet->phi());
    if (dR<mindR) found=true;
  }

  for (pat::PhotonCollection::const_iterator pho=photonHandle->begin(); pho!=photonHandle->end(); pho++){
    if (pho->pt()<photonptMinRC) continue;
    float dR = reco::deltaR(eta,phi,pho->eta(),pho->phi());
    if (dR<mindR) found=true;
  }
  for (pat::MuonCollection::const_iterator mu=muonHandle->begin(); mu!=muonHandle->end(); mu++){
    if (mu->pt()<leptonptMinRC) continue;
    float dR = reco::deltaR(eta,phi,mu->eta(),mu->phi());
    if (dR<mindR) found=true;
  }
  for (pat::ElectronCollection::const_iterator el=electronHandle->begin(); el!=electronHandle->end(); el++){
    if (el->pt()<leptonptMinRC) continue;
    float dR = reco::deltaR(eta,phi,el->eta(),el->phi());
    if (dR<mindR) found=true;
  }

  return found;

}

void SuperClusterFootprintRemovalMiniAOD::RandomConeIsolation(edm::Ptr<reco::Vertex> vertexforchargediso, PFIsolation_struct *output){

  const double pi = TMath::Pi();
  double rotation_phi = pi/2;

  bool isok = !(FindCloseJetsAndPhotons(rotation_phi));
  if (!isok) {
    rotation_phi = -pi/2;
    isok=!(FindCloseJetsAndPhotons(rotation_phi));
  }

  int count=0;
  while (!isok && count<20) {
    rotation_phi = randomgen->Uniform(0.8,2*pi-0.8);
    isok=!(FindCloseJetsAndPhotons(rotation_phi));
    count++;
  }

  if (count==20){
    std::cout << "It was not possible to find a suitable direction for the random cone in this event. This is not a problem."  << std::endl;
    return;
  };

  PFIsolation_struct temp = PFIsolation_worker(sc,vertexforchargediso,rotation_phi,true);
  output->chargediso_rcone = temp.chargediso;
  output->chargediso_primvtx_rcone = temp.chargediso_primvtx;
  output->neutraliso_rcone = temp.neutraliso;
  output->photoniso_rcone = temp.photoniso;
  
  output->eta_rcone=TVector3(sc->x(),sc->y(),sc->z()).Eta();
  float newphi = TVector3(sc->x(),sc->y(),sc->z()).Phi()+rotation_phi;
  while (newphi>pi) newphi-=2*pi;
  while (newphi<-pi) newphi+=2*pi;
  output->phi_rcone=newphi;
  output->rcone_isOK = true;

}

#endif
