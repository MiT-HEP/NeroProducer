import FWCore.ParameterSet.Config as cms

#------------------------------------------------------
nero = cms.EDAnalyzer("Nero",
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
    #
    ## eleChargedIsolation = cms.InputTag("electronIDValueMapProducer:eleChargedIsolation"),
    ## eleNeutralHadronIsolation = cms.InputTag("electronIDValueMapProducer:eleNeutralHadronIsolation"),
    ## elePhotonIsolation = cms.InputTag("electronIDValueMapProducer:elePhotonIsolation"),
    ## CUSTOM CONFIGURATION
    onlyMc = cms.bool (False),  ## not completely supported yet
    minJetPt = cms.double (20.),
    minJetN = cms.int32 (0),
    minLepPt = cms.double (20.),
    minLepN = cms.int32 (0),
    minTauPt = cms.double (20.),
    minTauN = cms.int32 (0),
    minGenParticlePt = cms.double(5.),
    minGenJetPt = cms.double(20.),
    minPhoPt = cms.double (20.),
    maxPhoIso = cms.double (-1.),
)
#------------------------------------------------------