import FWCore.ParameterSet.Config as cms

import os

#------------------------------------------------------
nero = cms.EDAnalyzer("Nero",
    info = cms.string("Nero"),
    cmssw = cms.string( os.environ['CMSSW_VERSION'] ) , # no need to ship it with the grid option
    head = cms.string(''),
    tag = cms.string(''),
    vertices = cms.InputTag("offlineSlimmedPrimaryVertices"),
    rho = cms.InputTag("fixedGridRhoFastjetAll"),
    muons = cms.InputTag("slimmedMuons"),
    electrons = cms.InputTag("slimmedElectrons"),
    taus = cms.InputTag("slimmedTaus"),
    photons = cms.InputTag("slimmedPhotons"),
    jets = cms.InputTag("slimmedJets"),
    puppijets = cms.InputTag("slimmedJetsPuppi"),

    ## Always re-corrected therefore re-run

    ## default met for nero is the mu+eg cleaned
    #mets = cms.InputTag("slimmedMETsMuEGClean"),
    mets = cms.InputTag("slimmedMETs"),
    metsPuppi = cms.InputTag("slimmedMETsPuppi"),

    ## Directly taken from miniaod
    chsAK8 = cms.InputTag("slimmedJetsAK8"),
    ## Mass Corrections
    chsAK8JEC = cms.string("jec/Summer16_23Sep2016V4"),

    pfCands = cms.InputTag("packedPFCandidates"),
                      
    # gen
    pileup = cms.InputTag("slimmedAddPileupInfo"),
    generator = cms.InputTag("generator"),
    genruninfo = cms.InputTag("generator"),
    lhe = cms.InputTag("externalLHEProducer"),
    genjets = cms.InputTag("slimmedGenJets"),
    prunedgen = cms.InputTag("prunedGenParticles"),
    packedgen = cms.InputTag("packedGenParticles"),
    ### ID MAP, these will become input tag when replacing the %(bx)s field, see testNero.py
    eleVetoIdMap   = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-veto"),
    eleLooseIdMap  = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-loose"),
    eleMediumIdMap = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-medium"),
    eleTightIdMap  = cms.InputTag("egmGsfElectronIDs:cutBasedElectronID-Summer16-80X-%(vs)s-tight"),
    eleHLTIdMap    = cms.InputTag("egmGsfElectronIDs:cutBasedElectronHLTPreselection-Summer16-V1"),
    eleMvaMap = cms.InputTag("electronMVAValueMapProducer:ElectronMVAEstimatorRun2Spring16GeneralPurposeV1Values"),

    phoLooseIdMap  = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-%(vs)s-loose"),
    phoMediumIdMap = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-%(vs)s-medium"),
    phoTightIdMap  = cms.InputTag("egmPhotonIDs:cutBasedPhotonID-Spring16-%(vs)s-tight"),
    ### ISO MAP
    phoChargedIsolation = cms.InputTag("photonIDValueMapProducer:phoChargedIsolation"),
    phoNeutralHadronIsolation = cms.InputTag("photonIDValueMapProducer:phoNeutralHadronIsolation"),
    phoPhotonIsolation = cms.InputTag("photonIDValueMapProducer:phoPhotonIsolation"),
    phoWorstChargedIsolation = cms.InputTag("photonIDValueMapProducer:phoWorstChargedIsolation"),
    eleEA = cms.string("RecoEgamma/ElectronIdentification/data/Summer16/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_80X.txt"),

    ### Rec Hits
    ebRecHits = cms.InputTag("reducedEgamma:reducedEBRecHits"),
    eeRecHits = cms.InputTag("reducedEgamma:reducedEERecHits"),

    ## MET Filters
    metfilterNames = cms.vstring(['Flag_HBHENoiseFilter', 
                                  'Flag_HBHENoiseIsoFilter', 
                                  'Flag_EcalDeadCellTriggerPrimitiveFilter', 
                                  'Flag_goodVertices', 
                                  'Flag_eeBadScFilter',
                                  'Flag_globalTightHalo2016Filter'
                                  ]),
    metFilterToken = cms.InputTag("TriggerResults"),          
    #metFilterToken = cms.InputTag("TriggerResults","","RECO"),          

    BadChCandFilterToken = cms.InputTag("BadChargedCandidateFilter"),                    
    BadPFMuonToken = cms.InputTag("BadPFMuonFilter"),  

    #
    ## TRIGGER
    triggerNMatch = cms.int32(12),
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
	    		## ELECTRONS
				'HLT_Ele27_WPTight_Gsf_v',
				'HLT_Ele30_WPTight_Gsf_v',
				'HLT_Ele27_eta2p1_WPTight_Gsf_v',
				'HLT_Ele32_eta2p1_WPTight_Gsf_v',
				'HLT_Ele35_WPLoose_Gsf_v',
				'HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',
				'HLT_DoubleEle24_22_eta2p1_WPLoose_Gsf_v',
				## MUONS
				'HLT_IsoMu22_v',
	    		'HLT_IsoTkMu22_v',
				'HLT_Mu45_eta2p1_v',
				'HLT_Mu50_v',
				'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_v',
				'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_v',
				'HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v',
				'HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ_v',
				## ELECTRONS-MUONS
				'HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v',
				'HLT_Mu12_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',
				'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v',
				'HLT_Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v',
				'HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_DZ_v',
				'HLT_Mu23_TrkIsoVVL_Ele8_CaloIdL_TrackIdL_IsoVL_v',
				'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_DZ_v',
				'HLT_Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL_v',
				## OTHER LEPTON TRIGGERS
				'HLT_Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL_v',
				'HLT_Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL_v',
				'HLT_Mu17_v',
                                'HLT_ECALHT800_v',
				'HLT_Ele23_CaloIdL_TrackIdL_IsoVL_v',
				'HLT_IsoMu20_v',
	    			'HLT_IsoTkMu20_v',
				'HLT_IsoMu27_v',
				'HLT_IsoTkMu27_v',
				'HLT_Ele22_eta2p1_WP75_Gsf_v',
				'HLT_Ele23_WPLoose_Gsf_v',
				'HLT_Ele27_WP85_Gsf_v',
				'HLT_Ele27_WPLoose_Gsf_v',
                                'HLT_Ele105_CaloIdVT_GsfTrkIdT_v',
				## TAU
                                'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80', ## not always available, Tau, NOT _v because there is noise cleaned
                                'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET90', ## not always available, Tau
                                'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET110', ## not always available, Tau
                                'HLT_LooseIsoPFTau50_Trk30_eta2p1_MET120', ## unprescaled , Tau, Match
				'HLT_LooseIsoPFTau50_Trk30_eta2p1_v', ## Prescaled, Tau, for turn on studies, Match
				'HLT_VLooseIsoPFTau120_Trk50_eta2p1_v', ## partially prescaled, Match
				'HLT_VLooseIsoPFTau140_Trk50_eta2p1_v', ## unprescaled, Match
				'HLT_CaloMHTNoPU90_PFMET90_PFMHT90_IDTight_BTagCSV_p067_v',
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
			        'HLT_Photon36_R9Id90_HE10_IsoM_v',
			        'HLT_Photon50_R9Id90_HE10_IsoM_v',
			        'HLT_Photon75_R9Id90_HE10_IsoM_v',
			        'HLT_Photon90_R9Id90_HE10_IsoM_v',
			        'HLT_Photon120_R9Id90_HE10_IsoM_v',
			        'HLT_Photon165_R9Id90_HE10_IsoM_v',
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
                                'Flag_eeBadScFilter',
                                'Flag_HBHENoiseFilter',
                                'Flag_HBHENoiseIsoFilter',
				### L1  ## these needs to be implemented in the code as well
				'L1_ET80',
				'L1_ET90',
				'L1_ET100',
				]),
    prescales = cms.InputTag('patTrigger'),
    trigger = cms.InputTag("TriggerResults","","HLT"),
    objects = cms.InputTag('selectedPatTrigger'),
    ## CUSTOM CONFIGURATION
    onlyMc = cms.bool (False),  ## not completely supported yet

    #evt
    extendEvent = cms.untracked.bool(False),

    extendMet = cms.bool(True),
                      
    minJetPt  = cms.double (15.),
    minJetEta = cms.double (4.7),
    minJetN   = cms.int32  (0),
    minJetId  = cms.string ('none'),
    matchJet  = cms.bool (True),
    matchJetDr = cms.double(0.3),

    ## PUPPI
    minPuppiJetPt  = cms.double (25.),
    minPuppiJetEta = cms.double (4.7),
    minPuppiJetN   = cms.int32  (0),
    minPuppiJetId  = cms.string ('loose'),
    
    ## CHS AK8
    minAK8CHSPt  = cms.double (100.),
    minAK8CHSEta = cms.double (4.7),
    minAK8CHSN   = cms.int32  (0),
    minAK8CHSId  = cms.string ('loose'),
    AK8CHSName   = cms.string ('AK8CHS'),

    ## Puppi AK8
    minAK8PuppiPt  = cms.double (100.),
    minAK8PuppiEta = cms.double (4.7),
    minAK8PuppiN   = cms.int32  (0),
    minAK8PuppiId  = cms.string ('loose'),
    AK8PuppiName   = cms.string ('AK8Puppi'),

    ## LEPTONS
    minElePt  = cms.double (10.),
    minEleEta = cms.double (2.5),
    maxEleIso = cms.double (-1.),

    minMuPt   = cms.double (5.),
    minMuEta  = cms.double (2.4),
    maxMuIso  = cms.double (-1),
    minLepId  = cms.string ('any'), 
   
    minLepN   = cms.int32 (0),
    matchLep  = cms.bool (True),
    matchLepDr = cms.double (0.1),

    minTauPt  = cms.double (18.),
    minTauEta = cms.double (2.3),
    minTauN   = cms.int32  (0),
    minTauId  = cms.string ('decayModeFindingNewDMs'),
    maxTauIso = cms.double (-1),
    extendTau = cms.bool(True),
    matchTau  = cms.bool (True),
    matchTauDr = cms.double (0.1),

    minGenParticlePt = cms.double(5.),
    minGenJetPt = cms.double(20.),
    particleGun = cms.untracked.bool(False),

    # PHOTONS
    extendPhotons = cms.bool(True),
    minPhoPt  = cms.double (15.),
    minPhoEta = cms.double (2.5),
    minPhoN   = cms.int32  (0),
    minPhoId = cms.string ('loose'),
    maxPhoIso = cms.double (-1.),
    matchPho  = cms.bool (False),
    matchPhoDr = cms.double (0.3),
)
#------------------------------------------------------


