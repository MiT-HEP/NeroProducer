import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING MONO TOP CONFIGURATION -------- "

nero.info = cms.string("NeroMonotop")

nero.triggerNames = cms.vstring(['HLT_PFMET170_NoiseCleaned',
                                 'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight',
                                 'HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight'])
nero.minJetPt         = cms.double (20.)
nero.minJetEta        = cms.double (2.5)
nero.minJetId         = cms.string ('monojet')
nero.minPuppiJetPt    = cms.double (20.)
nero.minPuppiJetEta   = cms.double (2.5)
nero.minPuppiJetId    = cms.string ('monojet')
nero.minCA15PuppiId   = cms.string ('monojet')
nero.puppiCA15        = cms.InputTag('packedPatJetsPFCA15Puppi')
#nero.puppiCA15        = cms.InputTag('CA15PuppiJetsPacked')
nero.minTauId         = cms.string ('decayModeFinding')
nero.maxTauIso        = cms.double (5)
nero.extendTau        = cms.bool(False)
nero.extendMet        = cms.bool(True)

