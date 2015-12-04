import FWCore.ParameterSet.Config as cms

DoubleEGFilter = cms.EDFilter("DoubleEGFilter",
		photons= cms.InputTag("slimmedPhotons"),
		)
