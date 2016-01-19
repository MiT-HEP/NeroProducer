import FWCore.ParameterSet.Config as cms

InfoProducer = cms.EDProducer("InfoProducer",
		generator= cms.InputTag("generator")
		)
