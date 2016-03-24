import FWCore.ParameterSet.Config as cms

MonoJetFilter = cms.EDFilter("MonoJetFilter",
                             met = cms.InputTag("slimmedMETs"),
                             muons = cms.InputTag("slimmedMuons"),
                             electrons = cms.InputTag("slimmedElectrons"),
                             photons = cms.InputTag("slimmedPhotons"),
                             keepWjets = cms.bool(True),
                             keepZll = cms.bool(True),
                             keepPhotons = cms.bool(True),
                             minMet = cms.double(175)
                             )
