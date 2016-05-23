import FWCore.ParameterSet.Config as cms

meNPartonsFilter = cms.EDFilter("MatrixElementNPartons",
		matrixElementPartons= cms.untracked.int(0),
		)
