import FWCore.ParameterSet.Config as cms

meHTFilter = cms.EDFilter("MatrixElementHT",
		htMin= cms.double(0),
		htMax= cms.double(100),
		qtMin= cms.double(-1),
		qtMax= cms.double(-1),
		)
