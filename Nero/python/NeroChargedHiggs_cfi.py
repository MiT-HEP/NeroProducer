import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING CHARGED HIGGS CONFIGURATION -------- "

nero.info = cms.string("NeroChargedHiggs")

nero.triggerNames = cms.vstring(['HLT_PFMET170_NoiseCleaned'
                                'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight'])
nero.minJetPt  = cms.double (25.)
nero.minJetEta = cms.double (4.7)

nero.minTauId  = cms.string ('decayModeFindingNewDMs')
nero.maxTauIso = cms.double (-1)
nero.extendTau = cms.bool(True)

