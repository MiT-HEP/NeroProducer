import FWCore.ParameterSet.Config as cms

NjettinessGroomed = cms.EDProducer("NjettinessGroomed",
                                   srcJets= cms.InputTag("slimmedJetsAK8"),
                                   )
