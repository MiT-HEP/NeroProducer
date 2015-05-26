import FWCore.ParameterSet.Config as cms

#------------------------------------------------------
process.nero = cms.EDAnalyzer("Nero",
    vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    muons = cms.InputTag("slimmedMuons"),
    electrons = cms.InputTag("slimmedElectrons"),
    taus = cms.InputTag("slimmedTaus"),
    photons = cms.InputTag("slimmedPhotons"),
    jets = cms.InputTag("slimmedJets"),
    fatjets = cms.InputTag("slimmedJetsAK8"),
    mets = cms.InputTag("slimmedMETs"),
    # gen
    pileup = cms.InputTag("addPileupInfo"),
    generator = cms.InputTag("generator"),
    genruninfo = cms.InputTag("generator"),
    genjets = cms.InputTag("slimmedGenJets"),
    prunedgen = cms.InputTag("prunedGenParticles"),
    packedgen = cms.InputTag("packedGenParticles"),
    ### ID MAP
    eleMediumIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-medium"),
    eleTightIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-PHYS14-PU20bx25-V2-standalone-tight"),
    phoMediumIdMap = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-PHYS14-PU20bx25-V2-standalone-medium"),
    ### ISO MAP
    phoChargedIsolation = cms.InputTag("photonIDValueMapProducer:phoChargedIsolation"),
    phoNeutralHadronIsolation = cms.InputTag("photonIDValueMapProducer:phoNeutralHadronIsolation"),
    phoPhotonIsolation = cms.InputTag("photonIDValueMapProducer:phoPhotonIsolation"),
)
#------------------------------------------------------
