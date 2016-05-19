import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING MONO JET CONFIGURATION -------- "

nero.info = cms.string("NeroMonojet")

nero.triggerNames = cms.vstring([
                                 'HLT_PFMET170_NoiseCleaned',
                                 'HLT_PFMET170_JetIdCleaned',
                                 'HLT_PFMET170_HBHECleaned',
                                 'HLT_PFMETNoMu120_PFMHTNoMu120_IDTight',
                                 'HLT_PFMETNoMu90_PFMHTNoMu90_IDTight',
                                 'HLT_IsoMu27',
                                 'HLT_IsoMu20'
                                 ])
nero.minJetPt  = cms.double (15.)
nero.minJetEta = cms.double (2.5)

nero.minTauId  = cms.string ('decayModeFinding')
nero.maxTauIso = cms.double (5)
nero.extendTau = cms.bool(False)
nero.extendMet = cms.bool(True)

