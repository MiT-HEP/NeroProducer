import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING NERO PHOTONS CONFIGURATION -------- "

nero.info = cms.string("NeroPhotnsjet")

minPhoPt  = cms.double (150.),
minPhoEta = cms.double (2.5),
minPhoN   = cms.int32  (2),
maxPhoIso = cms.double (-1),

