import FWCore.ParameterSet.Config as cms

meNPartonsFilter = cms.EDFilter("MatrixElementNPartons",
		matrixElementPartons= cms.int32(0),
		)
