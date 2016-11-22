import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING LEPTON RECO CONFIGURATION -------- "

nero.info = cms.string("NeroLeptonReco")
nero.minPhoPt  = cms.double (10.),
nero.minPhoId  = cms.string ('none'),
nero.minLepId  = cms.string ('none'),

# right now these are the defaults, but enforce the values
nero.minPhoEta = cms.double (2.5),
nero.minMuPt   = cms.double (10.),
nero.minMuEta  = cms.double (2.4),
nero.maxMuIso  = cms.double (-1),
nero.minElePt  = cms.double (10.),
nero.minEleEta = cms.double (2.5),
nero.maxEleIso = cms.double (-1.),
