import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING MONO JET CONFIGURATION -------- "

nero.triggerNames = cms.vstring(['HLT_PFMET170_NoiseCleaned'
                                'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight'])
nero.minJetPt  = cms.double (30.)
nero.minJetEta = cms.double (2.5)
nero.minJetN   = cms.int32  (0)
nero.minJetId  = cms.string ('loose')
nero.minElePt  = cms.double (10.)
nero.minEleEta = cms.double (2.5)
nero.maxEleIso = cms.double (-1.)

nero.minMuPt   = cms.double (10.)
nero.minMuEta  = cms.double (2.4)
nero.maxMuIso  = cms.double (0.2)
   
nero.minLepN   = cms.int32 (0)

nero.minTauPt  = cms.double (18.)
nero.minTauEta = cms.double (2.3)
nero.minTauN   = cms.int32  (0)
nero.minTauId  = cms.string ('decayModeFinding')
nero.maxTauIso = cms.double (5)

nero.minGenParticlePt = cms.double(5.)
nero.minGenJetPt = cms.double(20.)
                      
nero.minPhoPt  = cms.double (15.)
nero.minPhoEta = cms.double (2.5)
nero.minPhoN   = cms.int32  (0)
nero.maxPhoIso = cms.double (-1.)
