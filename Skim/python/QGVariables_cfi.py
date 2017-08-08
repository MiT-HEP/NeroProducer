import FWCore.ParameterSet.Config as cms

QGVariables = cms.EDProducer("QGVariables",
		srcJets= cms.InputTag("slimmedJets"),
		srcGenJets= cms.InputTag("slimmedGenJets"),
        isData=cms.bool(False),
        ## dRToProduce
		)
