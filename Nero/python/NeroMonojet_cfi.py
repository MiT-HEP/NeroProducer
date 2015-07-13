import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING MONO JET CONFIGURATION -------- "

nero.info = cms.string("NeroMonojet")

nero.triggerNames = cms.vstring(['HLT_PFMET170_NoiseCleaned'
                                'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight'])
nero.minJetPt  = cms.double (30.)
nero.minJetEta = cms.double (2.5)

nero.minTauId  = cms.string ('decayModeFinding')
nero.maxTauIso = cms.double (5)
nero.extendTau = cms.bool(False)

