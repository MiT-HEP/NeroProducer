import FWCore.ParameterSet.Config as cms
## FSR Photons ##
extractFSRPhotons = cms.EDProducer("FSRPhotonProducer",
                                           srcCands = cms.InputTag("packedPFCandidates"),
                                           muons = cms.InputTag("slimmedMuons"), 
                                           ptThresh = cms.double(1.0), ## 2 at analysis level
                                           extractMuonFSR = cms.bool(False),
                                           )

import PhysicsTools.PatAlgos.producersLayer1.pfParticleProducer_cfi 
FSRPhotons = PhysicsTools.PatAlgos.producersLayer1.pfParticleProducer_cfi.patPFParticles.clone(
    pfCandidateSource = 'extractFSRPhotons'
    )

PhotonMVA="CAST/FSRPhotons/data/PhotonMVAv4_BDTG1000TreesDY.weights.xml"
FSRRecovery = cms.EDProducer("FSRRecoveryProducer",
                                     pfcands = cms.InputTag("packedPFCandidates"),
                                     muons = cms.InputTag("slimmedMuons"), 
                                     electrons = cms.InputTag("slimmedElectrons"),
                                     photons = cms.InputTag("FSRPhotons"),
                                     weights = cms.string(PhotonMVA)
                                     )

fsrSequence = cms.Sequence(
        extractFSRPhotons 
        *FSRPhotons
        *FSRRecovery 
        )
