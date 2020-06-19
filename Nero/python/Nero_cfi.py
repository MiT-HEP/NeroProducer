import FWCore.ParameterSet.Config as cms

import os

#------------------------------------------------------
nero = cms.EDAnalyzer("Nero",

    info = cms.string("Nero"),
    cmssw = cms.string( os.environ['CMSSW_VERSION'] ) , # no need to ship it with the grid option
    head = cms.string(''),
    tag = cms.string(''),

    modules=cms.vstring([
        "NeroAll",
        "NeroEvent",
        "NeroVertex",
        "NeroFatJets",
        "NeroTaus",
        "NeroLeptons",
        "NeroMet",
        "NeroMonteCarlo",
        "NeroJets",
        "NeroPhotons",
        "NeroTrigger",
        #"NeroTrackJets",
        ]),

    NeroVertex = cms.PSet(
        vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
        ),
    
    NeroEvent = cms.PSet(
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        metFilterToken = cms.InputTag("TriggerResults","","RECO"),          
        #BadChCandFilterToken = cms.InputTag("BadChargedCandidateFilter"),                    
        #BadPFMuonToken = cms.InputTag("BadPFMuonFilter"),  
        metfilterNames = cms.vstring(['Flag_HBHENoiseFilter', 
                                  'Flag_HBHENoiseIsoFilter', 
                                  'Flag_EcalDeadCellTriggerPrimitiveFilter', 
                                  'Flag_goodVertices', 
                                  'Flag_globalTightHalo2016Filter',
                                  'Flag_globalSuperTightHalo2016Filter',
                                  'Flag_BadPFMuonFilter',
                                  'Flag_BadChargedCandidateFilter',
                                  'Flag_ecalBadCalibFilter',
                                  'Flag_eeBadScFilter', ## not in full rec
                                  ]),
        ),

    NeroAll = cms.PSet(
        generator = cms.InputTag("generator"),
        lhe = cms.InputTag("externalLHEProducer"),
        ),


    NeroFatJets = cms.PSet(
        AK8Jet = cms.InputTag("slimmedJetsAK8"),
        ## Mass Corrections
        AK8JEC = cms.string("jec/Summer16_23Sep2016V4"),
        AK8JECMC = cms.string("jec/Fall17_17Nov2017_V32"),
        minAK8Pt  = cms.double (170.),
        minAK8Eta = cms.double (4.7),
        minAK8Id  = cms.string ('loose'),
        AK8Name   = cms.string ('AK8WithDeepTag'),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
        ),

    # gen
    NeroMonteCarlo = cms.PSet ( 
        pileup = cms.InputTag("slimmedAddPileupInfo"),
        generator = cms.InputTag("generator"),
        genruninfo = cms.InputTag("generator"),
        genjets = cms.InputTag("slimmedGenJets"),
        genjetsAK8 = cms.InputTag("slimmedGenJetsAK8"),
        prunedgen = cms.InputTag("prunedGenParticles"),
        packedgen = cms.InputTag("packedGenParticles"),
        lhe = cms.InputTag("externalLHEProducer"),
        minGenParticlePt = cms.double(5.),
        minGenJetPt = cms.double(20.),
        particleGun = cms.untracked.bool(False),
        minPdfId=cms.string("1682"),# PDF4LHC15_nlo_30
        maxPdfId=cms.string("1714"),
        ## PDF4LH15_nlo_100 1476, 1578
        minMEId=cms.string("fs0_0p00"),# AQGC
        maxMEId=cms.string("ft9_2p50"),
    ),

    #
    ## TRIGGER
    NeroTrigger = cms.PSet ( 
        trigger = cms.InputTag("TriggerResults","","HLT"),
        objects = cms.InputTag('slimmedPatTrigger'),
        triggerNMatch = cms.int32(14),
        triggerNames = cms.vstring([
            ## LEPTON FAKES
            'HLT_Mu8_TrkIsoVVL_v',
            'HLT_Mu17_TrkIsoVVL_v',
            'HLT_Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30_v',
            'HLT_Ele17_CaloIdL_TrackIdL_IsoVL_PFJet30_v',
            'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30_v',
            ## TAG-AND-PROBE STUDIES
            'HLT_Ele25_eta2p1_WPTight_Gsf_v',
            'HLT_Ele27_eta2p1_WPLoose_Gsf_v',
            'HLT_IsoMu24_v',
            'HLT_IsoTkMu24_v',
            'HLT_IsoMu27_v',
            'HLT_IsoTkMu27_v',
            'HLT_Mu50_v',
            'HLT_TrkMu100_v',
            ### NO  TRIGGER MATCHING ###
            ## ELECTRONS
            'HLT_Ele27_WPTight_Gsf_v',
            'HLT_Ele30_WPTight_Gsf_v',
            'HLT_Ele27_eta2p1_WPTight_Gsf_v',
            'HLT_Ele32_eta2p1_WPTight_Gsf_v',
            'HLT_Ele35_WPLoose_Gsf_v',
            'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',
            'HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf_v',
            ## MUONS
            'HLT_IsoMu20_v',
            'HLT_IsoMu22_v',
            'HLT_IsoMu30_v',
            'HLT_IsoTkMu20_v',
            'HLT_IsoTkMu22_v',
            'HLT_Mu19_TrkIsoVVL_v',
            'HLT_Mu45_eta2p1_v',
            ## TAU 
            'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET110',
            'HLT_VLooseIsoPFTau140_Trk50_eta2p1_v',
            ## dimuon
            #
            'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',
            'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',
            'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',
            'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',
            'HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_DZ_v',
            'HLT_Mu19_TrkIsoVVL_Mu9_TrkIsoVVL_v',
            ## ELECTRONS-MUONS
            'HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v',
            'HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',
            'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_v',
            'HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v',
            'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_DoubleIsoMu20_eta2p1_v',
            'HLT_DoubleIsoMu24_eta2p1_v',
            ## OTHER LEPTON TRIGGERS
            'HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_Mu17_v',
            'HLT_ECALHT800_v',
            'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_Ele22_eta2p1_WP75_Gsf_v',
            'HLT_Ele23_WPLoose_Gsf_v',
            'HLT_Ele27_WP85_Gsf_v',
            'HLT_Ele27_WPLoose_Gsf_v',
            'HLT_Ele105_CaloIdVT_GsfTrkIdT_v',
            ## TAU
            'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80', ## not always available, Tau, NOT _v because there is noise cleaned
            'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET90', ## not always available, Tau
            'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120', ## unprescaled , Tau, Match
            'HLT_LooseIsoPFTau50_Trk30_eta2p1_v', ## Prescaled, Tau, for turn on studies, Match
            'HLT_VLooseIsoPFTau120_Trk50_eta2p1_v', ## partially prescaled, Match
            #'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_BTagCSV_p067_v',
            ## MET
            'HLT_PFMET170_NoiseCleaned',
            'HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight',
            'HLT_PFMETNoMu110_NoiseCleaned_PFMHTNoMu110_IDTight',
            'HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight',
            'HLT_PFMET170_HBHECleaned',
            'HLT_PFMET170_JetIdCleaned',
            'HLT_PFMET170_NotCleaned',
            'HLT_PFMET170_HBHE_BeamHaloCleaned',
            'HLT_PFMETNoMu90_PFMHTNoMu90_IDTight',
            'HLT_PFMETNoMu100_PFMHTNoMu100_IDTight',
            'HLT_PFMETNoMu110_PFMHTNoMu110_IDTight',
            'HLT_PFMETNoMu120_PFMHTNoMu120_IDTight',
            ## VBF Trigger
            'HLT_DiPFJet40_DEta3p5_MJJ600_PFMETNoMu140_v',
            #### Control triggers, extra
            'HLT_Ele15_IsoVVL_PFHT350_PFMET70',  ## Unprescaled, SingleElectron
            'HLT_Mu15_IsoVVVL_PFHT350_PFMET70',  ## Unprescaled ,SingleMuon
            'HLT_PFMET120_PFMHT120_IDTight',## unprescaled, MET, control on tau turn on
            #### Photon triggers
            #'HLT_Photon36_R9Id90_HE10_IsoM_v',
            #'HLT_Photon50_R9Id90_HE10_IsoM_v',
            #'HLT_Photon75_R9Id90_HE10_IsoM_v',
            #'HLT_Photon90_R9Id90_HE10_IsoM_v',
            #'HLT_Photon120_R9Id90_HE10_IsoM_v',
            #'HLT_Photon165_R9Id90_HE10_IsoM_v',
            'HLT_Photon175',
            'HLT_Photon165_HE10',
            ### Hadronic Triggers
            'HLT_PFHT_475_v',
            'HLT_PFHT_800_v',
            'HLT_AK8PFJet360_TrimMass30_v',
            'HLT_AK8PFHT650_TrimR0p1PT0p3Mass50_v',
            ### DoubleMu / DoubleE cross check and sync with Z
            'HLT_Ele17_CaloIdL_TrackIdL_IsoVL_v',
            'HLT_CaloMET200_NoiseCleaned',
            #'Flag_eeBadScFilter',
            #'Flag_HBHENoiseFilter',
            #'Flag_HBHENoiseIsoFilter',
            ### L1  ## these needs to be implemented in the code as well
            'L1_ET80',
            'L1_ET90',
            'L1_ET100',
            ### More Hadronic Triggers ( 2016 - Dataset JetHT - usedChHiggs)
            'HLT_PFHT900_v',
            'HLT_AK8PFJet450_v',
            'HLT_AK8PFHT700_TrimR0p1PT0p03Mass50_v',
            'HLT_AK8PFJet360_TrimMass30_v',
            'HLT_PFHT400_SixJet30_DoubleBTagCSV_p056_v',
            'HLT_PFHT450_SixJet40_BTagCSV_p056_v',
            # 2016 to try ( Dataset JetHT )
            'HLT_AK8PFHT750_TrimMass50_v',
            'HLT_AK8DiPFJet250_200_TrimMass30_BTagCSV_p20_v',
            'HLT_AK8DiPFJet300_200_TrimMass30_v',
            'HLT_AK8DiPFJet280_200_TrimMass30_BTagCSV_p087_v',
            'HLT_AK8DiPFJet250_200_TrimMass30_BTagCSV_p20_v',
            'HLT_DiCentralPFJet430_v',
            'HLT_DiJetVBF_PassThrough_v',
            'HLT_PFHT650_WideJetMJJ900DEtaJJ1p5_v',
            'HLT_PFHT650_WideJetMJJ950DEtaJJ1p5_v',
            'HLT_PFJet450_v',
            # 2016 to try ( Dataset BTagCSV )
            'HLT_DoubleJetsC100_DoubleBTagCSV_p014_DoublePFJetsC100MaxDeta1p6_v',
            'HLT_DoubleJetsC100_DoubleBTagCSV_p026_DoublePFJetsC160_v',
            'HLT_DoubleJetsC112_DoubleBTagCSV_p014_DoublePFJetsC112MaxDeta1p6_v',
            'HLT_DoubleJetsC112_DoubleBTagCSV_p026_DoublePFJetsC172_v',
            'HLT_QuadPFJet_BTagCSV_p016_VBF_Mqq460_v',
            'HLT_QuadPFJet_BTagCSV_p016_p11_VBF_Mqq200_v',
            ### More Hadronic Triggers ( 2017 and 2018 - Dataset JetHT )
            'HLT_PFHT1050_v',
            'HLT_AK8PFJet500_v',
            'HLT_AK8PFJet400_TrimMass30_v',
            'HLT_AK8PFHT800_TrimMass50_v',
            'HLT_PFJet500_v',
            # 2017-2018 to try ( Dataset JetHT )
            'HLT_AK8PFJetFwd400_v',
            'HLT_PFJetFwd400_v',
            # 2017 to try ( Dataset JetHT )
            'HLT_PFHT380_SixPFJet32_DoublePFBTagCSV_2p2_v',
            'HLT_PFHT380_SixPFJet32_DoublePFBTagDeepCSV_2p2_v',
            'HLT_PFHT430_SixPFJet40_PFBTagCSV_1p5_v',
            # 2017 to try ( Dataset BTagCSV )
            'HLT_AK8PFJet330_PFAK8BTagCSV_p17_v',
            'HLT_AK8PFJet330_PFAK8BTagCSV_p1_v',
            'HLT_DoublePFJets100MaxDeta1p6_DoubleCaloBTagCSV_p33_v',
            'HLT_QuadPFJet103_88_75_15_BTagCSV_p013_VBF2_v',
            'HLT_QuadPFJet103_88_75_15_DoubleBTagCSV_p013_p08_VBF1_v',
            'HLT_QuadPFJet105_88_76_15_BTagCSV_p013_VBF2_v',
            'HLT_QuadPFJet105_90_76_15_DoubleBTagCSV_p013_p08_VBF1_v',
            'HLT_QuadPFJet111_90_80_15_BTagCSV_p013_VBF2_v',
            'HLT_QuadPFJet111_90_80_15_DoubleBTagCSV_p013_p08_VBF1_v',
            'HLT_QuadPFJet98_83_71_15_BTagCSV_p013_VBF2_v',
            'HLT_QuadPFJet98_83_71_15_DoubleBTagCSV_p013_p08_VBF1_v',
            # 2018 to try ( Dataset JetHT )
            'HLT_AK8PFJet330_TrimMass30_PFAK8BoostedDoubleB_np2_v',
            'HLT_PFHT330PT30_QuadPFJet_75_60_45_40_TriplePFBTagDeepCSV_4p5_v',
            'HLT_PFHT400_FivePFJet_100_100_60_30_30_DoublePFBTagDeepCSV_4p5_v',
            'HLT_PFHT400_FivePFJet_120_120_60_30_30_DoublePFBTagDeepCSV_4p5_v',
            'HLT_PFHT400_SixPFJet32_DoublePFBTagDeepCSV_2p94_v',
            'HLT_PFHT450_SixPFJet36_PFBTagDeepCSV_1p59_v',
            'HLT_QuadPFJet103_88_75_15_DoublePFBTagDeepCSV_1p3_7p7_VBF1_v',
            'HLT_QuadPFJet103_88_75_15_PFBTagDeepCSV_1p3_VBF2_v',
            'HLT_QuadPFJet105_88_76_15_DoublePFBTagDeepCSV_1p3_7p7_VBF1_v',
            'HLT_QuadPFJet105_88_76_15_PFBTagDeepCSV_1p3_VBF2_v',
            'HLT_QuadPFJet111_90_80_15_DoublePFBTagDeepCSV_1p3_7p7_VBF1_v',
            'HLT_QuadPFJet111_90_80_15_PFBTagDeepCSV_1p3_VBF2_v',
            # 2017-2018 to try (Dataset MET)
            'HLT_MonoCentralPFJet80_PFMETNoMu120_PFMHTNoMu120_IDTight_v',
            'HLT_PFMET120_PFMHT120_IDTight_PFHT60_v',
            'HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_PFHT60_v',
            'HLT_TripleJet110_35_35_Mjj650_PFMET110_v',
            'HLT_DiJet110_35_Mjj650_PFMET110_v',
            ]),

        ),

    NeroMet = cms.PSet(
        pfCands = cms.InputTag("packedPFCandidates"),
        mets = cms.InputTag("slimmedMETs"),
        metsPuppi = cms.InputTag("slimmedMETsPuppi"),
        extendMet = cms.bool(True),
    ),
    
    NeroJets= cms.PSet( 
        jets = cms.InputTag("slimmedJets"),
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        minPt  = cms.double (15.),
        minEta = cms.double (4.7),
        minN   = cms.int32  (0),
        minId  = cms.string ('none'),
        prunedgen = cms.InputTag("prunedGenParticles"),
        genjets = cms.InputTag("slimmedGenJets"),
        redoPu=cms.bool(False),
    ),

    NeroTrackJets= cms.PSet( 
        jets = cms.InputTag("softActivityJets"),
        minPt = cms.double(1.),
    ),

    ## LEPTONS
    NeroLeptons = cms.PSet(
        pfCands = cms.InputTag("packedPFCandidates"),
        muons = cms.InputTag("slimmedMuons"),
        electrons = cms.InputTag("slimmedElectrons","","nero"),
        minElePt  = cms.double (10.),
        minEleEta = cms.double (2.5),
        maxEleIso = cms.double (-1.),

        minMuPt   = cms.double (5.),
        minMuEta  = cms.double (2.4),
        maxMuIso  = cms.double (-1),
        minLepId  = cms.string ('any'), 
       
        minLepN   = cms.int32 (0),
        vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
        ### ID MAP, these will become input tag when replacing the %(bx)s field, see testNero.py
        #eleHLTIdMap    = cms.InputTag("egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1"),
        #eleMvaMap = cms.InputTag("electronMVAValueMapProducer:ElectronMVAEstimatorRun2Fall17NoIsoV1Values","","nero"),
        #eleEA = cms.string("RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"),
        eleEA = cms.string("RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_92X.txt"),

        #eleVetoIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-veto","","nero"),
        #eleLooseIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-loose","","nero"),
        #eleMediumIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-medium","","nero"),
        #eleTightIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Fall17-94X-V1-tight","","nero"),

        ### Rec Hits
        #ebRecHits = cms.InputTag("reducedEgamma:reducedEBRecHits"),
        #eeRecHits = cms.InputTag("reducedEgamma:reducedEERecHits"),
    ),

    NeroTaus = cms.PSet ( 
        taus = cms.InputTag("slimmedTaus"),
        minPt  = cms.double (18.),
        minEta = cms.double (2.3),
        minN   = cms.int32  (0),
        minId  = cms.string ('decayModeFindingNewDMs'),
        maxIso = cms.double (-1),
        extendTau = cms.bool(True),
    ),


    # PHOTONS
    NeroPhotons = cms.PSet( 
        rho = cms.InputTag("fixedGridRhoFastjetAll"),
        photons = cms.InputTag("slimmedPhotons"),
        extendPhotons = cms.bool(True),
        minPt  = cms.double (15.),
        minEta = cms.double (2.5),
        minN   = cms.int32  (0),
        minId = cms.string ('loose'),
        maxIso = cms.double (-1.),
        ebRecHits = cms.InputTag("reducedEgamma:reducedEBRecHits"),
        eeRecHits = cms.InputTag("reducedEgamma:reducedEERecHits"),
        ),
)
#------------------------------------------------------


