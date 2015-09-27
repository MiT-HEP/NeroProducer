from MitAna.TreeMod.bambu import mithep, analysis
from MitAna.TreeMod.sequenceable import Chain, Bundle
import os

mitdata = os.environ['MIT_DATA']

def switchBX(case25, case50):
    global analysis
    return case25 if analysis.custom['bx'] == '25ns' else case50

jecVersion = switchBX('25nsV2', '50nsV5')

if analysis.isRealData:
    jecPattern = 'Summer15_' + jecVersion + '_DATA_{level}_{jettype}.txt'
    jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute', 'L2L3Residual']

else:
    jecPattern = 'Summer15_' + jecVersion + '_MC_{level}_{jettype}.txt'
    jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute']

from MitPhysics.SelMods.BadEventsFilterMod import badEventsFilterMod
from MitPhysics.Mods.GoodPVFilterMod import goodPVFilterMod
from MitPhysics.Mods.SeparatePileUpMod import separatePileUpMod
from MitPhysics.Mods.FatJetExtenderMod import fatJetExtenderMod

generator = mithep.GeneratorMod(
    IsData = False,
    CopyArrays = False,
    MCMETName = "GenMet",
    FillHist = True
)

jetCorrection = mithep.JetCorrectionMod(
    InputName = 'AKt4PFJetsCHS',
    CorrectedJetsName = 'CorrectedJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)

metCorrection = mithep.MetCorrectionMod('MetCorrection',
    InputName = 'PFMet',
    OutputName = 'PFType1CorrectedMet',
    JetsName = 'AKt4PFJetsCHS',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll,
    MaxEMFraction = 0.9,
    SkipMuons = True
)
metCorrection.ApplyType0(False)
metCorrection.ApplyType1(True)
metCorrection.ApplyShift(False)
metCorrection.IsData(analysis.isRealData)

metCorrectionJESUp = metCorrection.clone('MetCorrectionJESUp',
    OutputName = 'PFType1CorrectedMetJESUp',
    JESUncertaintySigma = 1.
)
metCorrectionJESDown = metCorrection.clone('MetCorrectionJESDown',
    OutputName = 'PFType1CorrectedMetJESDown',
    JESUncertaintySigma = -1.
)

for level in jecLevels:
    repl = {'level': level, 'jettype': 'AK4PFchs'}
    jetCorrection.AddCorrectionFromFile(mitdata + '/JEC/' + jecPattern.format(**repl))
    metCorrection.AddJetCorrectionFromFile(mitdata + '/JEC/' + jecPattern.format(**repl))
    metCorrectionJESUp.AddJetCorrectionFromFile(mitdata + '/JEC/' + jecPattern.format(**repl))
    metCorrectionJESDown.AddJetCorrectionFromFile(mitdata + '/JEC/' + jecPattern.format(**repl))

repl = {'level': 'Uncertainty', 'jettype': 'AK4PFchs'}
metCorrectionJESUp.AddJetCorrectionFromFile(mitdata + '/JEC/' + jecPattern.format(**repl))
metCorrectionJESDown.AddJetCorrectionFromFile(mitdata + '/JEC/' + jecPattern.format(**repl))

# Will not use PU jet ID here (MVA values to be stored in Nero tree)
goodAK4Jets = mithep.JetIdMod('AK4JetId',
    InputName = jetCorrection.GetOutputName(),
    OutputName = 'GoodAK4Jets',
    PFId = mithep.JetTools.kPFLoose,
    MVATrainingSet = mithep.JetIDMVA.nMVATypes,
    PtMin = 15.,
    EtaMax = 5.,
    MaxChargedEMFraction = 0.99,
    MaxNeutralEMFraction = 0.99,
    MaxNeutralHadronFraction = 0.99,
    MaxMuonFraction = 0.80,
    MinNPFCandidates = 2,
    MinNChargedPFCandidates = 1
)

looseTaus = mithep.PFTauIdMod('PFTauId',
    InputName = mithep.Names.gkHPSTauBrn,
    OutputName = 'LooseTaus',
    PtMin = 18.,
    EtaMax = 2.3
)
looseTaus.AddDiscriminator(mithep.PFTau.kDiscriminationByDecayModeFindingNewDMs)
looseTaus.AddCutDiscriminator(mithep.PFTau.kDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits, 5., False)

looseElectrons = mithep.ElectronIdMod('FiducialElectrons',
    InputName = mithep.Names.gkElectronBrn,
    OutputName = 'FiducialElectrons',
    IdType = mithep.ElectronTools.kNoId,
    IsoType = mithep.ElectronTools.kNoIso,
    ApplyEcalFiducial = True,
    ApplyD0Cut = False,
    ApplyDZCut = False,
    WhichVertex = 0,
    PtMin = 10.,
    EtaMax = 2.5,
    ConversionsName = 'Conversions'    
)

electronLooseId = looseElectrons.clone('ElectronLooseId',
    IsFilterMode = False,
    InputName = looseElectrons.GetOutputName(),
    OutputName = 'FiducialElectronId'
)

electronVetoId = electronLooseId.clone('ElectronVetoId',
    OutputName = 'VetoElectronId',
    ApplyD0Cut = True,
    ApplyDZCut = True,
    IdType = switchBX(mithep.ElectronTools.kSummer15Veto, mithep.ElectronTools.kSummer15Veto50ns),
    IsoType = switchBX(mithep.ElectronTools.kSummer15VetoIso, mithep.ElectronTools.kSummer15Veto50nsIso),
)

electronFakeId = electronVetoId.clone('ElectronFakeId',
    OutputName = 'FakeElectronId',
    IdType = switchBX(mithep.ElectronTools.kSummer15Fake, mithep.ElectronTools.kSummer15Fake50ns),
    IsoType = switchBX(mithep.ElectronTools.kSummer15FakeIso, mithep.ElectronTools.kSummer15Fake50nsIso),
)

electronMediumId = electronVetoId.clone('ElectronMediumId',
    OutputName = 'MediumElectronId',
    IdType = switchBX(mithep.ElectronTools.kSummer15Medium, mithep.ElectronTools.kSummer15Medium50ns),
    IsoType = switchBX(mithep.ElectronTools.kSummer15MediumIso, mithep.ElectronTools.kSummer15Medium50nsIso),
)

electronTightId = electronVetoId.clone('ElectronTightId',
    OutputName = 'TightElectronId',
    IdType = switchBX(mithep.ElectronTools.kSummer15Tight, mithep.ElectronTools.kSummer15Tight50ns),
    IsoType = switchBX(mithep.ElectronTools.kSummer15TightIso, mithep.ElectronTools.kSummer15Tight50nsIso),
)

veryLooseMuons = mithep.MuonIdMod('FiducialMuons',
    InputName = mithep.Names.gkMuonBrn,
    OutputName = 'FiducialMuons',
    MuonClassType = mithep.MuonTools.kAll,
    IdType = mithep.MuonTools.kNoId,
    IsoType = mithep.MuonTools.kNoIso,
    PFNoPileupCandidatesName = 'pfNoPU',
    PFPileupCandidatesName = 'pfPU',
    ApplyD0Cut = False,
    ApplyDZCut = False,
    PtMin = 3.,
    EtaMax = 2.4
)

muonVetoId = veryLooseMuons.clone('MuonVetoId',
    IsFilterMode = False,
    InputName = veryLooseMuons.GetOutputName(),
    OutputName = 'VetoMuonId',
    MuonClassType = mithep.MuonTools.kGlobal,
    ApplyD0Cut = True,
    ApplyDZCut = True,
    PtMin = 10.
)

muonPrivSoftId = veryLooseMuons.clone('MuonPrivSoftId',
    IsFilterMode = False,
    InputName = veryLooseMuons.GetOutputName(),
    OutputName = 'SoftMuonId',
    MuonClassType = mithep.MuonTools.kSoftMuon
)

muonFakeId = muonVetoId.clone('MuonFakeId',
    OutputName = 'FakeMuonId',
    IdType = mithep.MuonTools.kMuonPOG2012CutBasedIdTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrected
)

muon2012TightId = muonVetoId.clone('Muon2012TightId',
    OutputName = '2012TightMuonId',
    IdType = mithep.MuonTools.kMuonPOG2012CutBasedIdTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedTight
)

muonLooseId = muonVetoId.clone('MuonLooseId',
    OutputName = 'LooseMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    ApplyD0Cut = False,
    ApplyDZCut = False,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedLoose,
)

muonMediumId = muonVetoId.clone('MuonMediumId',
    OutputName = 'MediumMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    IdType = mithep.MuonTools.kMedium,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedLoose
)

muonTightId = muonVetoId.clone('MuonTightId',
    OutputName = 'TightMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobal,
    IdType = mithep.MuonTools.kTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedTight
)

tightMuons = mithep.MaskCollectionMod('TightMuons',
    InputName = veryLooseMuons.GetOutputName(),
    MaskName = muonTightId.GetOutputName(),
    OutputName = 'TightMuons'
)

ak8JetCorrection = mithep.JetCorrectionMod('AK8JetCorrection',
    InputName = 'AKt8FatJetsCHS',
    CorrectedJetsName = 'CorrectedAK8Jets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)

for level in jecLevels:
    ak8JetCorrection.AddCorrectionFromFile(mitdata + '/JEC/' + jecPattern.format(level = level, jettype = 'AK8PFchs'))

goodAK8Jets = goodAK4Jets.clone('GoodAK8Jets',
    InputName = ak8JetCorrection.GetOutputName(),
    OutputName = 'GoodAK8Jets'
)

ak8JetExtender = fatJetExtenderMod.clone('AK8JetExtender',
    InputName = goodAK8Jets.GetOutputName(),
    OutputName = 'XlAK8Jets',
    ConeSize = 0.8,
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    VertexesName = goodPVFilterMod.GetOutputName(),
    UseSoftDropLib = False,
    SoftDropR0 = 0.8,
    SoftDropZCut = 0.1,
    QGTaggingOn = True,
    DoShowerDeconstruction = False,
    DoECF = False,
    DoQjets = False,
    BeVerbose = False
)
ak8JetExtender.SetSubJetTypeOn(mithep.XlSubJet.kSoftDrop)

goodCA15Jets = goodAK4Jets.clone('GoodCA15Jets',
    InputName = 'CA15FatJetsCHS',
    OutputName = 'GoodCA15Jets'
)

ca15JetExtender = fatJetExtenderMod.clone('CA15JetExtender',
    InputName = goodCA15Jets.GetOutputName(),
    OutputName = 'XlCA15Jets',
    ConeSize = 1.5,
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    VertexesName = goodPVFilterMod.GetOutputName(),
    UseSoftDropLib = False,
    SoftDropR0 = 1.5,
    SoftDropZCut = 0.2,
    QGTaggingOn = True,
    DoShowerDeconstruction = False,
    DoECF = False,
    DoQjets = False,
    BeVerbose = False
)
ca15JetExtender.SetSubJetTypeOn(mithep.XlSubJet.kSoftDrop)

loosePhotons = mithep.PhotonIdMod('LoosePhotons',
    InputName = mithep.Names.gkPhotonBrn,
    OutputName = 'LoosePhotons',
    IdType = mithep.PhotonTools.kSummer15Loose,
    IsoType = mithep.PhotonTools.kSummer15LooseIso,
    ApplyCSafeElectronVeto = True,
    PtMin = 15.,
    EtaMax = 2.5
)

photonMediumId = loosePhotons.clone('PhotonMediumId',
    IsFilterMode = False,
    InputName = loosePhotons.GetOutputName(),
    OutputName = 'PhotonMediumId',
    IdType = mithep.PhotonTools.kSummer15Medium,
    IsoType = mithep.PhotonTools.kSummer15MediumIso
)

photonTightId = photonMediumId.clone('PhotonTightId',
    OutputName = 'PhotonTightId',
    IdType = mithep.PhotonTools.kSummer15Tight,
    IsoType = mithep.PhotonTools.kSummer15TightIso
)

head = 'HEAD'
tag = 'BAMBU_042_V0002'

fillers = []

fillers.append(mithep.nero.EventFiller(
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
))

fillers.append(mithep.nero.VertexFiller(
    VerticesName = goodPVFilterMod.GetOutputName()
))

fillers.append(mithep.nero.JetsFiller(
    JetsName = goodAK4Jets.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    JetIdCutWP = mithep.JetIDMVA.kLoose,
    JetIdMVATrainingSet = mithep.JetIDMVA.k53BDTCHSFullPlusRMS,
    JetIdMVAWeightsFile = mitdata + '/TMVAClassification_5x_BDT_chsFullPlusRMS.weights.xml',
    JetIdCutsFile = mitdata + '/jetIDCuts_121221.dat'
))

fillers.append(mithep.nero.TausFiller(
    TausName = looseTaus.GetOutputName()
))

fillers.append(mithep.nero.LeptonsFiller(
    MuonsName = veryLooseMuons.GetOutputName(),
    VetoMuonIdName = muonVetoId.GetOutputName(),
    FakeMuonIdName = muonFakeId.GetOutputName(),
    LooseMuonIdName = muonLooseId.GetOutputName(),
    MediumMuonIdName = muonMediumId.GetOutputName(),
    TightMuonIdName = muonTightId.GetOutputName(),
    ElectronsName = looseElectrons.GetOutputName(),
    VetoElectronIdName = electronVetoId.GetOutputName(),
    FakeElectronIdName = electronFakeId.GetOutputName(),
    LooseElectronIdName = electronLooseId.GetOutputName(),
    MediumElectronIdName = electronMediumId.GetOutputName(),
    TightElectronIdName = electronTightId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    NoPUPFCandsName = separatePileUpMod.GetPFNoPileUpName(),
    PUPFCandsName = separatePileUpMod.GetPFPileUpName()
))
fillers[-1].SetMuonIdName(9, muonPrivSoftId.GetOutputName())
fillers[-1].SetMuonIdName(10, muon2012TightId.GetOutputName())

fillers.append(mithep.nero.FatJetsFiller(mithep.nero.BaseFiller.kAK8Jets,
    FatJetsName = ak8JetExtender.GetOutputName()
))
#
#fillers.append(mithep.nero.FatJetsFiller(mithep.nero.BaseFiller.kCA15Jets,
#    FatJetsName = ca15JetExtender.GetOutputName()
#))

fillers.append(mithep.nero.MetFiller(
    MetName = metCorrection.GetOutputName(),
    JESUpMetName = metCorrectionJESUp.GetOutputName(),
    JESDownMetName = metCorrectionJESDown.GetOutputName(),
    MuonsName = tightMuons.GetOutputName(),
    GenMetName = generator.GetMCMETName()
))

fillers.append(mithep.nero.PhotonsFiller(
    PhotonsName = loosePhotons.GetOutputName(),
    MediumIdName = photonMediumId.GetOutputName(),
    TightIdName = photonTightId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName()
))

fillers.append(mithep.nero.MonteCarloFiller())

fillers.append(mithep.nero.AllFiller())

triggerFiller = mithep.nero.TriggerFiller()
fillers.append(triggerFiller)

neroMod = mithep.NeroMod(
    Info = 'Nero',
    Head = head,
    Tag = tag,
    FileName = 'nero.root',
    PrintLevel = 0
)
for filler in fillers:
    neroMod.AddFiller(filler)

neroMod.SetCondition(photonTightId)

## SET UP THE SEQUENCE
modules = []

triggers = [
    ('PFMETNoMu90_JetIdCleaned_PFMHTNoMu90_IDTight' if analysis.isRealData and analysis.custom['bx'] == '25ns' else 'PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight', []),
    ('PFMETNoMu120_JetIdCleaned_PFMHTNoMu120_IDTight' if analysis.isRealData and analysis.custom['bx'] == '25ns' else 'PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight', []),
    ('PFMET170_NoiseCleaned', []),
#    ('Ele23_WPLoose_Gsf' if analysis.isRealData else 'Ele23_CaloIdL_TrackIdL_IsoVL', ['']),
    ('Ele27_eta2p1_WPLoose_Gsf' if analysis.isRealData else 'Ele27_eta2p1_WP75_Gsf', ['hltEle27WPLooseGsfTrackIsoFilter']), # filter only matches data
    ('Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ', []),
    ('IsoMu24_eta2p1', ['hltL3crIsoL1sMu20Eta2p1L1f0L2f10QL3f24QL3trkIsoFiltered0p09']),
    ('IsoMu27', ['hltL3crIsoL1sMu25L1f0L2f10QL3f27QL3trkIsoFiltered0p09']),
    ('Photon120', ['hltEG120HEFilter']),
#    ('Photon135_PFMET100_JetIdCleaned', ['hltEG135HEFilter']),
    ('Photon165_HE10', ['hltEG165HE10Filter']),
    ('Photon175', ['hltEG175HEFilter']),
    ('Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ', []),
    ('Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ', []),
    ('Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL', []),
    ('Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL', []),
    ('Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL', []),
    ('Mu23_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL', []),
    ('Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Ele18_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Ele33_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Ele12_CaloIdL_TrackIdL_IsoVL', ['hltEle12CaloIdLTrackIdLIsoVLTrackIsoFilter']),
    ('Ele17_CaloIdL_TrackIdL_IsoVL', ['hltEle17CaloIdLTrackIdLIsoVLTrackIsoFilter']),
    ('Mu8_TrkIsoVVL', ['hltL3fL1sMu5L1f0L2f5L3Filtered8TkIsoFiltered0p4']),
    ('Mu17_TrkIsoVVL', ['hltL3fL1sMu12L1f0L2f12L3Filtered17TkIsoFiltered0p4']),
    ('Mu24_TrkIsoVVL', ['hltL3fL1sMu16L1f0L2f16L3Filtered24TkIsoFiltered0p4']),
    ('Mu34_TrkIsoVVL', ['hltL3fL1sMu20L1f0L2f20L3Filtered34TkIsoFiltered0p4'])
]

if analysis.isRealData:
    hltMod = mithep.HLTMod(
        ExportTrigObjects = False
    )

    for trig in triggers:
        hltMod.AddTrigger('HLT_' + trig[0] + '_v*')

    modules.append(hltMod)

for trig in triggers:
    triggerFiller.AddTriggerName('HLT_' + trig[0] + '_v*')
    for filt in trig[1]:
        triggerFiller.AddFilterName('HLT_' + trig[0] + '_v*', filt)

modules += [
    badEventsFilterMod,
    goodPVFilterMod,
    separatePileUpMod,
    jetCorrection,
    goodAK4Jets,
    metCorrection,
    metCorrectionJESUp,
    metCorrectionJESDown,
    looseTaus,
    veryLooseMuons,
    muonVetoId,
    muonPrivSoftId,
    muonFakeId,
    muon2012TightId,
    muonLooseId,
    muonMediumId,
    muonTightId,
    tightMuons,
    looseElectrons,
    electronLooseId,
    electronVetoId,
    electronFakeId,
    electronMediumId,
    electronTightId,
    loosePhotons,
    photonMediumId,
    photonTightId,
    ak8JetCorrection,
    goodAK8Jets,
#    goodCA15Jets,
    ak8JetExtender,
#    ca15JetExtender
]

if not analysis.isRealData:
    # run the generator mod for MC
    modules.append(generator)

# neroMod must be independent of the main chain
# to ensure that the all events tree is filled properly
sequence = Chain(modules)

analysis.setSequence(sequence + neroMod)
