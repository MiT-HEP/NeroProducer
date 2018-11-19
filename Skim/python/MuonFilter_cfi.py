import FWCore.ParameterSet.Config as cms

MuonFilter = cms.EDFilter("MuonFilter",
        muons = cms.InputTag("slimmedMuons"),
        minPt = cms.double(18),
        nMuons = cms.double(2),
        )
