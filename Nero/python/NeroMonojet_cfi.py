import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING MONO JET CONFIGURATION -------- "
raise ValueError("Out of date")

nero.info = cms.string("NeroMonojet")

nero.triggerNames = cms.vstring([
                                 'HLT_PFMET170_NoiseCleaned_v',                      # MET
                                 'HLT_PFMET170_JetIdCleaned_v',
                                 'HLT_PFMET170_HBHECleaned_v',
                                 'HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_v',
                                 'HLT_PFMETNoMu100_PFMHTNoMu100_IDTight_v',
                                 'HLT_PFMETNoMu110_PFMHTNoMu110_IDTight_v',
                                 'HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v',
                                 'HLT_IsoMu18_v',                                    # MUON
                                 'HLT_IsoMu20_v',
                                 'HLT_IsoMu22_v',
                                 'HLT_IsoMu24'
                                 'HLT_IsoMu27_v',
                                 'HLT_IsoTkMu18_v',
                                 'HLT_IsoTkMu24_v',
                                 'HLT_Mu8_TrkIsoVVL_v',
                                 'HLT_Mu17_TrkIsoVVL_v',
                                 'HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',
                                 'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',
                                 'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',
                                 'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',
                                 'HLT_Ele22_eta2p1_WPLoose_Gsf_v',                    # ELECTRON
                                 'HLT_Ele23_WPLoose_Gsf_v',
                                 'HLT_Ele27_WPLoose_Gsf_v',
                                 'HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',
                                 'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',
                                 'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Ele17_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v',
                                 'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v',
                                 'HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_v',
                                 'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v',
                                 'HLT_Photon135_PFMET100_v',                          # PHOTON
                                 'HLT_Photon165_HE10_v',
                                 'HLT_Photon175_v',
                                 ])
nero.minJetPt  = cms.double (15.)
nero.minJetEta = cms.double (2.5)

nero.minTauId  = cms.string ('decayModeFinding')
nero.maxTauIso = cms.double (5)
nero.extendTau = cms.bool(False)
nero.extendMet = cms.bool(True)
nero.minPhoId = cms.string ('monoph')
