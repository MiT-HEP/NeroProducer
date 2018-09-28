import FWCore.ParameterSet.Config as cms

import os

#------------------------------------------------------
nero = cms.EDAnalyzer("Nero",

    info = cms.string("Nero"),
    cmssw = cms.string( os.environ['CMSSW_VERSION'] ) , # no need to ship it with the grid option
    head = cms.string(''),
    tag = cms.string(''),

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
                                  'Flag_eeBadScFilter',
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
        chsAK8 = cms.InputTag("slimmedJetsAK8"),
        ## Mass Corrections
        chsAK8JEC = cms.string("jec/Summer16_23Sep2016V4"),
        minAK8CHSPt  = cms.double (170.),
        minAK8CHSEta = cms.double (4.7),
        minAK8CHSN   = cms.int32  (0),
        minAK8CHSId  = cms.string ('loose'),
        AK8CHSName   = cms.string ('AK8CHS'),
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
            ## TAU WITH MATCHING
            'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET110',
            'HLT_VLooseIsoPFTau140_Trk50_eta2p1_v',
            ## TAG-AND-PROBE STUDIES
            'HLT_Ele25_eta2p1_WPTight_Gsf_v',
            'HLT_Ele27_eta2p1_WPLoose_Gsf_v',
            'HLT_IsoMu24_v',
            'HLT_IsoTkMu24_v',
            'HLT_IsoMu27_v',
            'HLT_IsoTkMu27_v',
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
            'HLT_Mu50_v',
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
    ),

    NeroTrackJets= cms.PSet( 
        jets = cms.InputTag("softActivityJets"),
        minPt = cms.double(0.),
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


