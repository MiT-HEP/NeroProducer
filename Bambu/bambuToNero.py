from MitAna.TreeMod.bambu import mithep, analysis
import os

mitdata = os.environ['MIT_DATA']

from MitPhysics.Mods.GoodPVFilterMod import goodPVFilterMod
from MitPhysics.Mods.JetIdMod import jetIdMod
from MitPhysics.Mods.PFTauIdMod import pfTauIdMod
from MitPhysics.Mods.ElectronIdMod import electronIdMod
from MitPhysics.Mods.MuonIdMod import muonIdMod
from MitPhysics.Mods.PhotonIdMod import photonIdMod
from MitPhysics.Mods.SeparatePileUpMod import separatePileUpMod

def addTrigger(path):
    global mithep
    global sequence
    global triggerFiller

    pathNoV = path.replace('_v*', '')
    hltMod = mithep.HLTMod(pathNoV + 'Mod',
        AbortIfNotAccepted = False,
        AbortIfNoData = False
    )
    hltMod.AddTrigger(path)

    sequence = hltMod * sequence
    triggerFiller.AddTriggerName(pathNoV)
    triggerFiller.SetTriggerObjectsName(pathNoV, hltMod.GetTrigObjsName())


generator = mithep.GeneratorMod(
    IsData = False,
    CopyArrays = False,
    MCMETName = "GenMet"
)

jetCorrection = mithep.JetCorrectionMod(
    InputName = 'AKt4PFJetsCHS',
    CorrectedJetsName = 'CorrectedJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)
if analysis.isRealData:
    jecSources = [
        "74X_dataRun2_Prompt_v1_L1FastJet_AK4PFchs.txt",
        "74X_dataRun2_Prompt_v1_L2Relative_AK4PFchs.txt",
        "74X_dataRun2_Prompt_v1_L3Absolute_AK4PFchs.txt",
        "74X_dataRun2_Prompt_v1_L2L3Residual_AK4PFchs.txt"
    ]
else:
    jecSources = [
        "MCRUN2_74_V9_L1FastJet_AK4PFchs.txt",
        "MCRUN2_74_V9_L2Relative_AK4PFchs.txt",
        "MCRUN2_74_V9_L3Absolute_AK4PFchs.txt"
    ]

for jec in jecSources:
    jetCorrection.AddCorrectionFromFile(mitdata + '/' + jec)

# Will use independent JetIDMVA that can be used by Nero too
jetId = jetIdMod.clone(
    MVATrainingSet = mithep.JetIDMVA.nMVATypes
)

jetIdMVA = mithep.JetIDMVA()
jetIdMVA.Initialize(jetIdMod.GetMVACutWP(), mithep.JetIDMVA.kLoose, jetIdMod.GetMVAWeightsFile(), jetIdMod.GetMVACutsFile())

jetId.SetJetIDMVA(jetIdMVA)

metCorrection = mithep.MetCorrectionMod('MetCorrection',
    InputName = 'PFMet',
    OutputName = 'PFType1CorrectedMet',
    JetsName = 'AKt4PFJets',
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
if analysis.isRealData:
    jecSourcesMET = [
        "74X_dataRun2_Prompt_v1_L1FastJet_AK4PF.txt",
        "74X_dataRun2_Prompt_v1_L2Relative_AK4PF.txt",
        "74X_dataRun2_Prompt_v1_L3Absolute_AK4PF.txt",
        "74X_dataRun2_Prompt_v1_L2L3Residual_AK4PF.txt"
    ]
    jecUncert = "74X_dataRun2_Prompt_v1_Uncertainty_AK4PF.txt"
else:
    jecSourcesMET = [
        "MCRUN2_74_V9_L1FastJet_AK4PF.txt",
        "MCRUN2_74_V9_L2Relative_AK4PF.txt",
        "MCRUN2_74_V9_L3Absolute_AK4PF.txt"
    ]
    jecUncert = "MCRUN2_74_V9_Uncertainty_AK4PF.txt"

for jec in jecSourcesMET:
    metCorrection.AddJetCorrectionFromFile(mitdata + '/' + jec)
    metCorrectionJESUp.AddJetCorrectionFromFile(mitdata + '/' + jec)
    metCorrectionJESDown.AddJetCorrectionFromFile(mitdata + '/' + jec)

metCorrectionJESUp.AddJetCorrectionFromFile(mitdata + '/' + jecUncert)
metCorrectionJESDown.AddJetCorrectionFromFile(mitdata + '/' + jecUncert)

pfTauId = pfTauIdMod.clone('PFTauId')
pfTauId.AddCutDiscriminator(mithep.PFTau.kDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits, 5., False)

electronLooseId = electronIdMod.clone('ElectronLooseId')

electronTightId = electronIdMod.clone('ElectronTightId',
    IsFilterMode = False,
    InputName = electronLooseId.GetOutputName(),
    OutputName = 'TightElectronId',
    IdType = mithep.ElectronTools.kPhys14Tight,
    IsoType = mithep.ElectronTools.kPhys14TightIso
)

muonLooseId = muonIdMod.clone('MuonLooseId')

muonTightId = muonIdMod.clone('MuonTightId',
    IsFilterMode = False,
    InputName = muonLooseId.GetOutputName(),
    OutputName = 'TightMuonId',
    IdType = mithep.MuonTools.kMuonPOG2012CutBasedIdTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrected
)

muonTightIdMask = mithep.MaskCollectionMod('TightMuons',
    InputName = muonLooseId.GetOutputName(),
    MaskName = muonTightId.GetOutputName(),
    OutputName = 'TightMuons'
)

fatJetCorrection = mithep.JetCorrectionMod('FatJetCorrection',
    InputName = 'AKt8FatJetsCHS',
    CorrectedJetsName = 'CorrectedFatJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)
if analysis.isRealData:
    fatJetCorrection.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L1FastJet_AK8PFchs.txt")
    fatJetCorrection.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L2Relative_AK8PFchs.txt")
    fatJetCorrection.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L3Absolute_AK8PFchs.txt")
    fatJetCorrection.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L2L3Residual_AK8PFchs.txt")
else:
    fatJetCorrection.AddCorrectionFromFile(mitdata + "/MCRUN2_74_V9_L1FastJet_AK8PFchs.txt")
    fatJetCorrection.AddCorrectionFromFile(mitdata + "/MCRUN2_74_V9_L2Relative_AK8PFchs.txt")
    fatJetCorrection.AddCorrectionFromFile(mitdata + "/MCRUN2_74_V9_L3Absolute_AK8PFchs.txt")

fatJetId = jetIdMod.clone('FatJetId',
    InputName = fatJetCorrection.GetOutputName(),
    OutputName = 'GoodFatJets',
    MVATrainingSet = mithep.JetIDMVA.nMVATypes
)

photonLooseId = photonIdMod.clone('PhotonLooseId')

photonMediumId = photonIdMod.clone('PhotonMediumId',
    IsFilterMode = False,
    InputName = photonLooseId.GetOutputName(),
    OutputName = 'PhotonMediumId',
    IdType = mithep.PhotonTools.kPhys14Medium,
    IsoType = mithep.PhotonTools.kPhys14Medium
)

photonTightId = photonMediumId.clone('PhotonTightId',
    OutputName = 'PhotonTightId',
    IdType = mithep.PhotonTools.kPhys14Tight,
    IsoType = mithep.PhotonTools.kPhys14Tight
)

head = 'HEAD'
tag = 'BAMBU_041'

fillers = []

fillers.append(mithep.nero.EventFiller(
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
))

fillers.append(mithep.nero.VertexFiller(
    VerticesName = goodPVFilterMod.GetOutputName()
))

fillers.append(mithep.nero.JetsFiller(
    JetsName = jetId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    JetIDMVA = jetIdMVA
))

fillers.append(mithep.nero.TausFiller(
    TausName = pfTauId.GetOutputName()
))

fillers.append(mithep.nero.LeptonsFiller(
    ElectronsName = electronLooseId.GetOutputName(),
    MuonsName = muonLooseId.GetOutputName(),
    ElectronIdsName = electronTightId.GetOutputName(),
    MuonIdsName = muonTightId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    NoPUPFCandsName = separatePileUpMod.GetPFNoPileUpName(),
    PUPFCandsName = separatePileUpMod.GetPFPileUpName()
))

fillers.append(mithep.nero.FatJetsFiller(
    FatJetsName = fatJetId.GetOutputName()
))

fillers.append(mithep.nero.MetFiller(
    MetName = metCorrection.GetOutputName(),
    JESUpMetName = metCorrectionJESUp.GetOutputName(),
    JESDownMetName = metCorrectionJESDown.GetOutputName(),
    MuonsName = muonTightIdMask.GetOutputName(),
    GenMetName = generator.GetMCMETName()
))

fillers.append(mithep.nero.PhotonsFiller(
    PhotonsName = photonLooseId.GetOutputName(),
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

sequence = goodPVFilterMod
if not analysis.isRealData:
    sequence *= generator
sequence *= separatePileUpMod * \
    jetCorrection * \
    jetId * \
    metCorrection * \
    metCorrectionJESUp * \
    metCorrectionJESDown * \
    pfTauId * \
    electronLooseId * \
    muonLooseId * \
    photonLooseId * \
    electronTightId * \
    muonTightId * \
    muonTightIdMask * \
    photonMediumId * \
    photonTightId * \
    fatJetCorrection * \
    fatJetId

addTrigger('HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v*')
addTrigger('HLT_PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight_v*')
addTrigger('HLT_IsoMu27_v*')
addTrigger("HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ_v*")
addTrigger("HLT_Ele27_eta2p1_WPLoose_Gsf_v*")
addTrigger("HLT_Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v*")
addTrigger('HLT_PFMET170_NoiseCleaned_v*')
addTrigger("HLT_Photon175_v*")

# neroMod must be independent of the main chain
# to ensure that the all events tree is filled properly
sequence += neroMod

analysis.SetAllowNoHLTTree(True)

analysis.setSequence(sequence)
