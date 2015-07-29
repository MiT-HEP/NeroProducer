from MitAna.TreeMod.bambu import mithep, analysis
import os

mitdata = os.environ['MIT_DATA']

from MitPhysics.Mods.GoodPVFilterMod import goodPVFilterMod
from MitPhysics.Mods.JetCorrectionMod import jetCorrectionMod
from MitPhysics.Mods.JetIdMod import jetIdMod
from MitPhysics.Mods.PFTauIdMod import pfTauIdMod
pfTauIdMod.AddCutDiscriminator(mithep.PFTau.kDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits, 5., False)
from MitPhysics.Mods.ElectronIdMod import electronIdMod
from MitPhysics.Mods.MuonIdMod import muonIdMod
from MitPhysics.Mods.PhotonIdMod import photonIdMod
from MitPhysics.Mods.SeparatePileUpMod import separatePileUpMod

generatorMod = mithep.GeneratorMod(
    IsData = False,
    CopyArrays = False,
    MCMETName = "GenMet"
)

electronTightId = electronIdMod.clone('ElectronTightId',
    IsFilterMode = False,
    InputName = electronIdMod.GetOutputName(),
    OutputName = 'TightElectronId',
    IdType = mithep.ElectronTools.kPhys14Tight,
    IsoType = mithep.ElectronTools.kPhys14TightIso
)

muonTightId = muonIdMod.clone('MuonTightId',
    IsFilterMode = False,
    InputName = muonIdMod.GetOutputName(),
    OutputName = 'TightMuonId',
    IdType = mithep.MuonTools.kMuonPOG2012CutBasedIdTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrected
)

muonTightIdMask = mithep.MaskCollectionMod('TightMuons',
    InputName = muonIdMod.GetOutputName(),
    MaskName = muonTightId.GetOutputName(),
    OutputName = 'TightMuons'
)

fatJetCorrectionMod = mithep.JetCorrectionMod('FatJetCorrection',
    InputName = 'AKt8PFJetsCHS',
    CorrectedJetsName = 'CorrectedFatJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)
if analysis.isRealData:
    fatJetCorrectionMod.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L1FastJet_AK8PFchs.txt")
    fatJetCorrectionMod.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L2Relative_AK8PFchs.txt")
    fatJetCorrectionMod.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L3Absolute_AK8PFchs.txt")
    fatJetCorrectionMod.AddCorrectionFromFile(mitdata + "/74X_dataRun2_Prompt_v1_L2L3Residual_AK8PFchs.txt")
else:
    fatJetCorrectionMod.AddCorrectionFromFile(mitdata + "/MCRUN2_74_V9_L1FastJet_AK8PFchs.txt")
    fatJetCorrectionMod.AddCorrectionFromFile(mitdata + "/MCRUN2_74_V9_L2Relative_AK8PFchs.txt")
    fatJetCorrectionMod.AddCorrectionFromFile(mitdata + "/MCRUN2_74_V9_L3Absolute_AK8PFchs.txt")

fatJetIdMod = jetIdMod.clone('FatJetId',
    InputName = fatJetCorrectionMod.GetOutputName(),
    OutputName = 'GoodFatJets',
    MVATrainingSet = mithep.JetIDMVA.nMVATypes
)

photonMediumId = photonIdMod.clone('PhotonMediumId',
    IsFilterMode = False,
    InputName = photonIdMod.GetOutputName(),
    OutputName = 'PhotonMediumId',
    IdType = mithep.PhotonTools.kPhys14Medium,
    IsoType = mithep.PhotonTools.kPhys14Medium
)

photonTightId = photonMediumId.clone('PhotonTightId',
    OutputName = 'PhotonTightId',
    IdType = mithep.PhotonTools.kPhys14Tight,
    IsoType = mithep.PhotonTools.kPhys14Tight
)

sequence = goodPVFilterMod
if not analysis.isRealData:
    sequence *= generatorMod

sequence *= separatePileUpMod * \
    jetCorrectionMod * \
    jetIdMod * \
    pfTauIdMod * \
    electronIdMod * \
    muonIdMod * \
    photonIdMod * \
    electronTightId * \
    muonTightId * \
    muonTightIdMask * \
    fatJetCorrectionMod * \
    fatJetIdMod * \
    photonMediumId * \
    photonTightId

triggers = []
hltMods = {}
def addTrigger(path):
    global mithep
    global triggers
    global hltMods
    global sequence

    pathNoV = path.replace('_v*', '')
    hltMod = mithep.HLTMod(pathNoV + 'Mod',
        AbortIfNotAccepted = False,
        AbortIfNoData = False
    )
    hltMod.AddTrigger(path)
    hltMods[path] = hltMod

    triggers.append(path)
    sequence = hltMod * sequence


addTrigger('HLT_PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight_v*')

fillers = []

fillers.append(mithep.nero.EventFiller(
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
))

fillers.append(mithep.nero.VertexFiller(
    VerticesName = goodPVFilterMod.GetOutputName()
))

fillers.append(mithep.nero.JetsFiller(
    JetsName = jetIdMod.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    JetIDMVA = jetIdMod.GetJetIDMVA()
))

fillers.append(mithep.nero.TausFiller(
    TausName = pfTauIdMod.GetOutputName()
))

fillers.append(mithep.nero.LeptonsFiller(
    ElectronsName = electronIdMod.GetOutputName(),
    MuonsName = muonIdMod.GetOutputName(),
    ElectronIdsName = electronTightId.GetOutputName(),
    MuonIdsName = muonTightId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    NoPUPFCandsName = separatePileUpMod.GetPFNoPileUpName(),
    PUPFCandsName = separatePileUpMod.GetPFPileUpName()
))

fillers.append(mithep.nero.FatJetsFiller(
    FatJetsName = fatJetIdMod.GetOutputName()
))

fillers.append(mithep.nero.MetFiller(
    MetName = 'PFType1CorrectedMet',
    MuonsName = muonTightIdMask.GetOutputName(),
    GenMetName = generatorMod.GetMCMETName()
))

fillers.append(mithep.nero.PhotonsFiller(
    PhotonsName = photonIdMod.GetOutputName(),
    MediumIdName = photonMediumId.GetOutputName(),
    TightIdName = photonTightId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName()
))

fillers.append(mithep.nero.MonteCarloFiller())

fillers.append(mithep.nero.AllFiller())

triggerFiller = mithep.nero.TriggerFiller()

for path in triggers:
    triggerFiller.AddTriggerName(path)
    triggerFiller.SetTriggerObjectsName(path, hltMods[path].GetTrigObjsName())

fillers.append(triggerFiller)

def makeNeroMod(catName, category):
    global mithep
    global fillers

    head = 'HEAD'
    tag = 'BAMBU_041'
    
    neroMod = mithep.NeroMod('Nero' + catName,
        Info = 'Nero',
        Head = head,
        Tag = tag,
        FileName = 'nero_' + catName + '.root',
        PrintLevel = 0
    )
    for filler in fillers:
        neroMod.AddFiller(filler.clone())

    categoryMod = mithep.EventCategoryMod(catName + 'Filter',
        InputName = 'MonoJetEventCategories',
        Category = category
    )

    return categoryMod * neroMod

signal = makeNeroMod('Signal', mithep.MonoJetAnalysisMod.kSignal)
dielectron = makeNeroMod('Dielectron', mithep.MonoJetAnalysisMod.kDielectron)
dimuon = makeNeroMod('Dimuon', mithep.MonoJetAnalysisMod.kDimuon)
singleelectron = makeNeroMod('SingleElectron', mithep.MonoJetAnalysisMod.kSingleElectron)
singlemuon = makeNeroMod('SingleMuon', mithep.MonoJetAnalysisMod.kSingleMuon)
photon = makeNeroMod('Photon', mithep.MonoJetAnalysisMod.kPhoton)

sequence *= (signal + dielectron + dimuon + singleelectron + singlemuon + photon)

analysis.SetAllowNoHLTTree(True)

analysis.setSequence(sequence)
