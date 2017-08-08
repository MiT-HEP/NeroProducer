import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING LEPTON RECO CONFIGURATION -------- "

nero.info = cms.string("NeroLeptonReco")
nero.NeroPhotons.minPt  = cms.double (10.),
nero.NeroPhotons.minId  = cms.string ('none'),
nero.NeroLeptons.minLepId  = cms.string ('none'),

# Match all the triggers
nero.NeroTrigger.triggerNMatch = cms.int32(32),

# right now these are the defaults, but enforce the values
nero.NeroPhotons.minEta = cms.double (2.5),
nero.NeroLeptons.minMuPt   = cms.double (10.),
nero.NeroLeptons.minMuEta  = cms.double (2.4),
nero.NeroLeptons.maxMuIso  = cms.double (-1),
nero.NeroLeptons.minElePt  = cms.double (10.),
nero.NeroLeptons.minEleEta = cms.double (2.5),
nero.NeroLeptons.maxEleIso = cms.double (-1.),
