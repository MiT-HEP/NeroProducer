import FWCore.ParameterSet.Config as cms

HPlusFilter = cms.EDFilter("HPlusFilter",
		taus= cms.InputTag("slimmedTaus"),
		muons= cms.InputTag("slimmedMuons"),
		electrons= cms.InputTag("slimmedElectrons"),
		)
