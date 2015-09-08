from MitAna.TreeMod.bambu import mithep, analysis
import os

mitdata = os.environ['MIT_DATA']

from MitPhysics.SelMods.BadEventsFilterMod import badEventsFilterMod
from MitPhysics.Mods.GoodPVFilterMod import goodPVFilterMod
from MitPhysics.Mods.JetIdMod import jetIdMod
from MitPhysics.Mods.PFTauIdMod import pfTauIdMod
from MitPhysics.Mods.ElectronIdMod import electronIdMod
from MitPhysics.Mods.MuonIdMod import muonIdMod
from MitPhysics.Mods.PhotonIdMod import photonIdMod
from MitPhysics.Mods.SeparatePileUpMod import separatePileUpMod
from MitPhysics.Mods.FatJetExtenderMod import fatJetExtenderMod

def addTrigger(path):
    global mithep
    global sequence
    global triggerFiller

    hltMod = mithep.HLTMod(path.replace('_v*', 'Mod'),
        AbortIfNotAccepted = False,
        AbortIfNoData = False
    )
    hltMod.AddTrigger(path)

    sequence = hltMod * sequence
    triggerFiller.AddTriggerName(path)
    triggerFiller.SetTriggerObjectsName(path, hltMod.GetTrigObjsName())


generator = mithep.GeneratorMod(
    IsData = False,
    CopyArrays = False,
    MCMETName = "GenMet"
)

jecSources = [
    "Summer15_50nsV2_MC_L1FastJet_AK4PFchs.txt",
    "Summer15_50nsV2_MC_L2Relative_AK4PFchs.txt",
    "Summer15_50nsV2_MC_L3Absolute_AK4PFchs.txt"
]
#jecUncert = "Summer15_50nsV2_MC_Uncertainty_AK4PFchs.txt"

jetCorrection = mithep.JetCorrectionMod(
    InputName = 'AKt4PFJetsCHS',
    CorrectedJetsName = 'CorrectedJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)

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

#metCorrectionJESUp = metCorrection.clone('MetCorrectionJESUp',
#    OutputName = 'PFType1CorrectedMetJESUp',
#    JESUncertaintySigma = 1.
#)
#metCorrectionJESDown = metCorrection.clone('MetCorrectionJESDown',
#    OutputName = 'PFType1CorrectedMetJESDown',
#    JESUncertaintySigma = -1.
#)

for jec in jecSources:
    jetCorrection.AddCorrectionFromFile(mitdata + '/JEC/' + jec)
    metCorrection.AddJetCorrectionFromFile(mitdata + '/JEC/' + jec)
#    metCorrectionJESUp.AddJetCorrectionFromFile(mitdata + '/JEC/' + jec)
#    metCorrectionJESDown.AddJetCorrectionFromFile(mitdata + '/JEC/' + jec)

#metCorrectionJESUp.AddJetCorrectionFromFile(mitdata + '/JEC/' + jecUncert)
#metCorrectionJESDown.AddJetCorrectionFromFile(mitdata + '/JEC/' + jecUncert)

# Will not use PU jet ID here (MVA values to be stored in Nero tree)
jetId = jetIdMod.clone(
    MVATrainingSet = mithep.JetIDMVA.nMVATypes,
    PtMin = 15.
)

pfTauId = pfTauIdMod.clone('PFTauId')
pfTauId.AddCutDiscriminator(mithep.PFTau.kDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits, 5., False)

electronLooseId = electronIdMod.clone('ElectronLooseId')

electronVetoId = electronIdMod.clone('ElectronVetoId',
    IsFilterMode = False,
    InputName = electronLooseId.GetOutputName(),
    OutputName = 'VetoElectronId',
    ApplyD0Cut = True,
    ApplyDZCut = True,
    IdType = mithep.ElectronTools.kSummer15Veto,
    IsoType = mithep.ElectronTools.kSummer15VetoIso
)

electronFakeId = electronIdMod.clone('ElectronFakeId',
    IsFilterMode = False,
    InputName = electronLooseId.GetOutputName(),
    OutputName = 'FakeElectronId',
    ApplyD0Cut = True,
    ApplyDZCut = True,
    IdType = mithep.ElectronTools.kSummer15Fake,
    IsoType = mithep.ElectronTools.kSummer15FakeIso
)

electronMediumId = electronIdMod.clone('ElectronMediumId',
    IsFilterMode = False,
    InputName = electronLooseId.GetOutputName(),
    OutputName = 'MediumElectronId',
    ApplyD0Cut = True,
    ApplyDZCut = True,
    IdType = mithep.ElectronTools.kSummer15Medium,
    IsoType = mithep.ElectronTools.kSummer15MediumIso
)

electronTightId = electronIdMod.clone('ElectronTightId',
    IsFilterMode = False,
    InputName = electronLooseId.GetOutputName(),
    OutputName = 'TightElectronId',
    ApplyD0Cut = True,
    ApplyDZCut = True,
    IdType = mithep.ElectronTools.kSummer15Tight,
    IsoType = mithep.ElectronTools.kSummer15TightIso
)

muonLooseId = muonIdMod.clone('MuonLooseId')

muonVetoId = muonIdMod.clone('MuonVetoId',
    IsFilterMode = False,
    InputName = muonLooseId.GetOutputName(),
    OutputName = 'VetoMuonId',
    MuonClassType = mithep.MuonTools.kGlobal,
    IdType = mithep.MuonTools.kNoId,
    IsoType = mithep.MuonTools.kNoIso,
    ApplyD0Cut = True,
    ApplyDZCut = True,
    PtMin = 10
)

muonSoftId = muonIdMod.clone('MuonSoftId',
    IsFilterMode = False,
    InputName = muonLooseId.GetOutputName(),
    OutputName = 'VetoSoftId',
    MuonClassType = mithep.MuonTools.kSoftMuon,
    IdType = mithep.MuonTools.kNoId,
    IsoType = mithep.MuonTools.kNoIso,
    ApplyD0Cut = False,
    ApplyDZCut = False,
    PtMin = 3
)

muonFakeId = muonIdMod.clone('MuonFakeId',
    IsFilterMode = False,
    InputName = muonLooseId.GetOutputName(),
    OutputName = 'FakeMuonId',
    MuonClassType = mithep.MuonTools.kGlobal,
    IdType = mithep.MuonTools.kMuonPOG2012CutBasedIdTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrected,
    ApplyD0Cut = True,
    ApplyDZCut = True,
    PtMin = 10
)

muonTightId = muonIdMod.clone('MuonTightId',
    IsFilterMode = False,
    InputName = muonLooseId.GetOutputName(),
    OutputName = 'TightMuonId',
    MuonClassType = mithep.MuonTools.kGlobal,
    IdType = mithep.MuonTools.kMuonPOG2012CutBasedIdTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedTight,
    ApplyD0Cut = True,
    ApplyDZCut = True,
    PtMin = 10
)

muonTightIdMask = mithep.MaskCollectionMod('TightMuons',
    InputName = muonLooseId.GetOutputName(),
    MaskName = muonTightId.GetOutputName(),
    OutputName = 'TightMuons'
)

ak8JetCorrection = mithep.JetCorrectionMod('AK8JetCorrection',
    InputName = 'AKt8FatJetsCHS',
    CorrectedJetsName = 'CorrectedAK8Jets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)
if analysis.isRealData:
    ak8JetCorrection.AddCorrectionFromFile(mitdata + "/JEC/74X_dataRun2_Prompt_v1_L1FastJet_AK8PFchs.txt")
    ak8JetCorrection.AddCorrectionFromFile(mitdata + "/JEC/74X_dataRun2_Prompt_v1_L2Relative_AK8PFchs.txt")
    ak8JetCorrection.AddCorrectionFromFile(mitdata + "/JEC/74X_dataRun2_Prompt_v1_L3Absolute_AK8PFchs.txt")
    ak8JetCorrection.AddCorrectionFromFile(mitdata + "/JEC/74X_dataRun2_Prompt_v1_L2L3Residual_AK8PFchs.txt")
else:
    ak8JetCorrection.AddCorrectionFromFile(mitdata + "/JEC/MCRUN2_74_V9_L1FastJet_AK8PFchs.txt")
    ak8JetCorrection.AddCorrectionFromFile(mitdata + "/JEC/MCRUN2_74_V9_L2Relative_AK8PFchs.txt")
    ak8JetCorrection.AddCorrectionFromFile(mitdata + "/JEC/MCRUN2_74_V9_L3Absolute_AK8PFchs.txt")

ak8JetId = jetIdMod.clone('AK8JetId',
    InputName = ak8JetCorrection.GetOutputName(),
    OutputName = 'GoodAK8Jets',
    MVATrainingSet = mithep.JetIDMVA.nMVATypes
)

ak8JetExtender = fatJetExtenderMod.clone('AK8JetExtender',
    InputName = ak8JetId.GetOutputName(),
    OutputName = 'XlAK8Jets',
    ConeSize = 0.8,
    QGTaggerCHS = True,
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    VertexesName = goodPVFilterMod.GetOutputName(),
    SoftDropR0 = 0.8,
    DoShowerDeconstruction = False,
    NMaxMicrojets = 7,
    DoECF = False,
    DoQjets = False,
    SoftDropZCut = 0.1,
    BeVerbose = False
)
ak8JetExtender.SetSubJetTypeOn(mithep.XlSubJet.kSoftDrop)

ca15JetId = jetIdMod.clone('CA15JetId',
    InputName = 'CA15FatJetsCHS',
    OutputName = 'GoodCA15Jets',
    MVATrainingSet = mithep.JetIDMVA.nMVATypes
)

ca15JetExtender = fatJetExtenderMod.clone('CA15JetExtender',
    InputName = ca15JetId.GetOutputName(),
    OutputName = 'XlCA15Jets',
    ConeSize = 1.5,
    QGTaggerCHS = True,
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    VertexesName = goodPVFilterMod.GetOutputName(),
    SoftDropR0 = 1.5,
    DoShowerDeconstruction = False,
    NMaxMicrojets = 7,
    DoECF = False,
    DoQjets = False,
    SoftDropZCut = 0.2,
    BeVerbose = False
)
ca15JetExtender.SetSubJetTypeOn(mithep.XlSubJet.kSoftDrop)

photonLooseId = photonIdMod.clone('PhotonLooseId')

photonMediumId = photonIdMod.clone('PhotonMediumId',
    IsFilterMode = False,
    InputName = photonLooseId.GetOutputName(),
    OutputName = 'PhotonMediumId',
    IdType = mithep.PhotonTools.kSummer15Medium,
    IsoType = mithep.PhotonTools.kSummer15Medium
)

photonTightId = photonMediumId.clone('PhotonTightId',
    OutputName = 'PhotonTightId',
    IdType = mithep.PhotonTools.kSummer15Tight,
    IsoType = mithep.PhotonTools.kSummer15Tight
)

head = 'HEAD'
tag = 'BAMBU_042'

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
    JetIdCutWP = mithep.JetIDMVA.kLoose,
    JetIdMVATrainingSet = mithep.JetIDMVA.k53BDTCHSFullPlusRMS,
    JetIdMVAWeightsFile = mitdata + '/TMVAClassification_5x_BDT_chsFullPlusRMS.weights.xml',
    JetIdCutsFile = mitdata + '/jetIDCuts_121221.dat'
))

fillers.append(mithep.nero.TausFiller(
    TausName = pfTauId.GetOutputName()
))

fillers.append(mithep.nero.LeptonsFiller(
    MuonsName = muonLooseId.GetOutputName(),
    MuonIdsAName = muonVetoId.GetOutputName(),
    MuonIdsBName = muonFakeId.GetOutputName(),
    MuonIdsCName = muonSoftId.GetOutputName(),
    MuonIdsDName = muonTightId.GetOutputName(),
    ElectronsName = electronLooseId.GetOutputName(),
    ElectronIdsAName = electronVetoId.GetOutputName(),
    ElectronIdsBName = electronFakeId.GetOutputName(),
    ElectronIdsCName = electronMediumId.GetOutputName(),
    ElectronIdsDName = electronTightId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    NoPUPFCandsName = separatePileUpMod.GetPFNoPileUpName(),
    PUPFCandsName = separatePileUpMod.GetPFPileUpName()
))

fillers.append(mithep.nero.FatJetsFiller(mithep.nero.BaseFiller.kAK8Jets,
    FatJetsName = ak8JetExtender.GetOutputName()
))

fillers.append(mithep.nero.FatJetsFiller(mithep.nero.BaseFiller.kCA15Jets,
    FatJetsName = ca15JetExtender.GetOutputName()
))

fillers.append(mithep.nero.MetFiller(
    MetName = metCorrection.GetOutputName(),
#    JESUpMetName = metCorrectionJESUp.GetOutputName(),
#    JESDownMetName = metCorrectionJESDown.GetOutputName(),
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

#metCorrSequence = metCorrection * metCorrectionJESUp * metCorrectionJESDown
metCorrSequence = metCorrection

sequence = badEventsFilterMod * \
    goodPVFilterMod
if not analysis.isRealData:
    sequence *= generator
sequence *= separatePileUpMod * \
    jetCorrection * \
    jetId * \
    metCorrSequence * \
    pfTauId * \
    muonLooseId * \
    muonVetoId * \
    muonSoftId * \
    muonFakeId * \
    muonTightId * \
    muonTightIdMask * \
    electronLooseId * \
    electronVetoId * \
    electronFakeId * \
    electronMediumId * \
    electronTightId * \
    photonLooseId * \
    photonMediumId * \
    photonTightId * \
    ak8JetCorrection * \
    ak8JetId * \
    ak8JetExtender * \
    ca15JetId * \
    ca15JetExtender

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
