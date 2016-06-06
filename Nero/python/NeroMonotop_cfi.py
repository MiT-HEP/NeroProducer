import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING MONO TOP CONFIGURATION -------- "

nero.info = cms.string("NeroMonotop")

nero.minJetPt         = cms.double (20.)
nero.minJetEta        = cms.double (4.7)
nero.minJetId         = cms.string ('loose')
nero.minPuppiJetPt    = cms.double (20.)
nero.minPuppiJetEta   = cms.double (4.7)
nero.minPuppiJetId    = cms.string ('loose')
nero.minCA15PuppiPt   = cms.double (150.)
nero.minCA15PuppiEta  = cms.double (2.5)
nero.minCA15PuppiId   = cms.string ('loose')
nero.puppiCA15        = cms.InputTag('packedPatJetsPFCA15Puppi')
nero.minTauId         = cms.string ('decayModeFinding')
nero.maxTauIso        = cms.double (5)
nero.extendTau        = cms.bool(True)
nero.extendMet        = cms.bool(True)
nero.doReclustering   = cms.bool(True)
nero.doCA15           = cms.bool(True)
nero.doPuppi          = cms.bool(True)
nero.metsPuppi        = cms.InputTag("type1PuppiMET")
nero.metsPuppiUncorrected = cms.InputTag("pfMETPuppi")
