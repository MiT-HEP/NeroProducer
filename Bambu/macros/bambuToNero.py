from MitAna.TreeMod.bambu import mithep, analysis
from MitAna.TreeMod.sequenceable import Chain, Bundle
import os

####################################
### UTIL FUNCTIONS AND VARIABLES ###
####################################

mitdata = os.environ['MIT_DATA']

def switchBX(case25, case50):
    global analysis
    return case25 if analysis.custom['bx'] == '25ns' else case50

jecVersion = switchBX('25nsV6', '50nsV5')

if analysis.isRealData:
    jecPattern = mitdata + '/JEC/Summer15_' + jecVersion + '/Summer15_' + jecVersion + '_DATA_{level}_{jettype}.txt'
    jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute', 'L2L3Residual']

else:
    jecPattern = mitdata +'/JEC/Summer15_' + jecVersion + '/Summer15_' + jecVersion + '_MC_{level}_{jettype}.txt'
    jecLevels = ['L1FastJet', 'L2Relative', 'L3Absolute']

#########################################
### MODULES RUN WITH DEFAULT SETTINGS ###
#########################################

from MitPhysics.Mods.GoodPVFilterMod import goodPVFilterMod
from MitPhysics.Mods.SeparatePileUpMod import separatePileUpMod
from MitPhysics.Mods.PuppiMod import puppiMod
from MitPhysics.Mods.PuppiPFJetMod import puppiPFJetMod

################################
### JET/MET ID & CORRECTIONS ###
################################

jetCorrection = mithep.JetCorrectionMod('JetCorrection',
    InputName = 'AKt4PFJetsCHS',
    CorrectedJetsName = 'CorrectedJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)

puppiJetCorrection = mithep.JetCorrectionMod('PuppiJetCorrection',
    InputName = puppiPFJetMod.GetOutputName(),
    CorrectedJetsName = 'CorrectedPuppiJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)

metCorrection = mithep.MetCorrectionMod('MetCorrection',
    InputName = 'PFMet',
    OutputName = 'PFType1CorrectedMet',
    JetsName = 'AKt4PFJetsCHS',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll,
    MaxEMFraction = 0.9,
    SkipMuons = True,
    MinJetPt = 15.
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
    jetCorrection.AddCorrectionFromFile(jecPattern.format(**repl))
    metCorrection.AddJetCorrectionFromFile(jecPattern.format(**repl))
    metCorrectionJESUp.AddJetCorrectionFromFile(jecPattern.format(**repl))
    metCorrectionJESDown.AddJetCorrectionFromFile(jecPattern.format(**repl))

repl = {'level': 'Uncertainty', 'jettype': 'AK4PFchs'}
metCorrectionJESUp.AddJetCorrectionFromFile(jecPattern.format(**repl))
metCorrectionJESDown.AddJetCorrectionFromFile(jecPattern.format(**repl))

puppiMet = mithep.MetMod('PuppiMet',
    InputName = puppiMod.GetOutputName(),
    OutputName = 'PuppiMet',
    OutputType = mithep.kPFMet
)

puppiMetCorrection = metCorrection.clone('PuppiMetCorrection',
    InputName = puppiMet.GetOutputName(),
    OutputName = 'PuppiType1CorrectedMet',
    JetsName = puppiPFJetMod.GetOutputName()
)

for level in jecLevels:
    repl = {'level': level, 'jettype': 'AK4PFPuppi'}
    puppiJetCorrection.AddCorrectionFromFile(jecPattern.format(**repl))
    puppiMetCorrection.AddJetCorrectionFromFile(jecPattern.format(**repl))

# Will not use PU jet ID here (MVA values to be stored in Nero tree)
looseAK4Jets = mithep.JetIdMod('AK4JetId',
    InputName = jetCorrection.GetOutputName(),
    OutputName = 'GoodAK4Jets',
    PFId = mithep.JetTools.kPFLoose,
    MVATrainingSet = mithep.JetIDMVA.nMVATypes,
    PtMin = 15.,
    EtaMax = 5.
)

###########################
### LEPTON & PHOTON IDS ###
###########################

looseTaus = mithep.PFTauIdMod('PFTauId',
    InputName = mithep.Names.gkHPSTauBrn,
    OutputName = 'LooseTaus',
    PtMin = 18.,
    EtaMax = 2.3
)
looseTaus.AddDiscriminator(mithep.PFTau.kDiscriminationByDecayModeFindingNewDMs)
looseTaus.AddCutDiscriminator(mithep.PFTau.kDiscriminationByRawCombinedIsolationDBSumPtCorr3Hits, 5., False)

# Electrons
# veryLooseElectrons = base collection
# baseline, veto, fake, loose, medium, and tight id defined on top of veryLoose

veryLooseElectrons = mithep.ElectronIdMod('FiducialElectrons',
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
    ApplyConvFilterType1 = False,
    ApplyConvFilterType2 = False,
    ApplyNExpectedHitsInnerCut = False,
    ChargeFilter = False,
    ConversionsName = 'Conversions'    
)

# for electrons, baseline and veryLoose are identical

electronBaselineId = veryLooseElectrons.clone('ElectronBaselineId',
    IsFilterMode = False,
    InputName = veryLooseElectrons.GetOutputName(),
    OutputName = 'FiducialElectronId'
)

electronVetoId = electronBaselineId.clone('ElectronVetoId',
    OutputName = 'VetoElectronId',
    ApplyD0Cut = True,
    ApplyDZCut = True,
    ApplyConvFilterType1 = True,
    ApplyConvFilterType2 = False,
    ApplyNExpectedHitsInnerCut = True,
    IdType = switchBX(mithep.ElectronTools.kSummer15Veto, mithep.ElectronTools.kSummer15Veto50ns)
)

electronVetoIso = electronBaselineId.clone('ElectronVetoIso',
    OutputName = 'VetoElectronIso',
    IsoType = switchBX(mithep.ElectronTools.kSummer15VetoIso, mithep.ElectronTools.kSummer15Veto50nsIso)
)

electronFakeId = electronVetoId.clone('ElectronFakeId',
    OutputName = 'FakeElectronId',
    IdType = switchBX(mithep.ElectronTools.kSummer15Fake, mithep.ElectronTools.kSummer15Fake50ns),
    IsoType = switchBX(mithep.ElectronTools.kSummer15FakeIso, mithep.ElectronTools.kSummer15Fake50nsIso),
)

electronLooseId = electronVetoId.clone('ElectronLooseId',
    OutputName = 'LooseElectronId',
    IdType = switchBX(mithep.ElectronTools.kSummer15Loose, mithep.ElectronTools.kSummer15Loose50ns)
)

electronLooseIso = electronBaselineId.clone('ElectronLooseIso',
    OutputName = 'LooseElectronIso',
    IsoType = switchBX(mithep.ElectronTools.kSummer15LooseIso, mithep.ElectronTools.kSummer15Loose50nsIso)
)

electronMediumId = electronVetoId.clone('ElectronMediumId',
    OutputName = 'MediumElectronId',
    IdType = switchBX(mithep.ElectronTools.kSummer15Medium, mithep.ElectronTools.kSummer15Medium50ns)
)

electronMediumIso = electronBaselineId.clone('ElectronMediumIso',
    OutputName = 'MediumElectronIso',
    IsoType = switchBX(mithep.ElectronTools.kSummer15MediumIso, mithep.ElectronTools.kSummer15Medium50nsIso)
)

electronTightId = electronVetoId.clone('ElectronTightId',
    OutputName = 'TightElectronId',
    IdType = switchBX(mithep.ElectronTools.kSummer15Tight, mithep.ElectronTools.kSummer15Tight50ns),
)

electronTightIso = electronBaselineId.clone('ElectronTightIso',
    OutputName = 'TightElectronIso',
    IsoType = switchBX(mithep.ElectronTools.kSummer15TightIso, mithep.ElectronTools.kSummer15Tight50nsIso),
)

# Muons
# veryLooseMuons = base collection
# baseline, soft (original definition), fake, veto, loose, medium, tight defined on top of veryLoose
# for medium and tight, versions with tighter IP cuts are available

veryLooseMuons = mithep.MuonIdMod('FiducialMuons',
    InputName = mithep.Names.gkMuonBrn,
    OutputName = 'FiducialMuons',
    MuonClassType = mithep.MuonTools.kAll,
    IdType = mithep.MuonTools.kNoId,
    IsoType = mithep.MuonTools.kNoIso,
    ApplyD0Cut = False,
    ApplyDZCut = False,
    PtMin = 3.,
    EtaMax = 2.4
)

# baseline has a pT cut

muonBaselineId = veryLooseMuons.clone('MuonBaselineId',
    IsFilterMode = False,
    InputName = veryLooseMuons.GetOutputName(),
    OutputName = 'BaselineMuonId',
    MuonClassType = mithep.MuonTools.kGlobalorTracker,
    ApplyD0Cut = False,
    ApplyDZCut = False,
    PtMin = 10.
)

# soft is tighter than baseline but with lower pT threshold

muonPrivSoftId = veryLooseMuons.clone('MuonPrivSoftId',
    IsFilterMode = False,
    InputName = veryLooseMuons.GetOutputName(),
    OutputName = 'SoftMuonId',
    MuonClassType = mithep.MuonTools.kSoftMuon
)

muonFakeId = muonBaselineId.clone('MuonFakeId',
    OutputName = 'FakeMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    IdType = mithep.MuonTools.kTightIP,
    ApplyD0Cut = True,
    ApplyDZCut = True,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedFake,
    PFNoPileupCandidatesName = 'pfNoPU',
    PFPileupCandidatesName = 'pfPU'
)

muonLooseId = muonBaselineId.clone('MuonLooseId',
    OutputName = 'LooseMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    IdType = mithep.MuonTools.kLoose,
    ApplyD0Cut = False,
    ApplyDZCut = False
)

muonLooseIso = muonBaselineId.clone('MuonLooseIso',
    OutputName = 'LooseMuonIso',
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedLoose,
    PFNoPileupCandidatesName = 'pfNoPU',
    PFPileupCandidatesName = 'pfPU'
)

muonMediumId = muonBaselineId.clone('MuonMediumId',
    OutputName = 'MediumMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    IdType = mithep.MuonTools.kMedium,
    ApplyD0Cut = True,
    ApplyDZCut = True
)

muonMediumIso = muonBaselineId.clone('MuonMediumIso',
    OutputName = 'MediumMuonIso',
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedTight,
    PFNoPileupCandidatesName = 'pfNoPU',
    PFPileupCandidatesName = 'pfPU'
)

muonTightId = muonBaselineId.clone('MuonTightId',
    OutputName = 'TightMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    IdType = mithep.MuonTools.kTight,
    ApplyD0Cut = True,
    ApplyDZCut = True
)

muonTightIso = muonBaselineId.clone('MuonTightIso',
    OutputName = 'TightMuonIso',
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrectedTight,
    PFNoPileupCandidatesName = 'pfNoPU',
    PFPileupCandidatesName = 'pfPU'
)

muonMediumIPId = muonBaselineId.clone('MuonMediumIPId',
    OutputName = 'MediumIPMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    IdType = mithep.MuonTools.kMediumIP,
    ApplyD0Cut = True,
    ApplyDZCut = True
)

muonTightIPId = muonBaselineId.clone('MuonTightIPId',
    OutputName = 'TightIPMuonId',
    MuonClassType = mithep.MuonTools.kPFGlobalorTracker,
    IdType = mithep.MuonTools.kTightIP,
    ApplyD0Cut = True,
    ApplyDZCut = True
)

tightMuons = mithep.MaskCollectionMod('TightMuons',
    InputName = veryLooseMuons.GetOutputName(),
    MaskName = muonTightIso.GetOutputName(),
    OutputName = 'TightMuons'
)

# Photons

baselinePhotons = mithep.PhotonIdMod('BaselinePhotons',
    InputName = mithep.Names.gkPhotonBrn,
    OutputName = 'BaselinePhotons',
    IdType = mithep.PhotonTools.kNoId,
    IsoType = mithep.PhotonTools.kNoIso,
    PtMin = 15.,
    EtaMax = 2.5
)

photonLooseId = mithep.PhotonIdMod('PhotonLooseId',
    IsFilterMode = False,
    InputName = baselinePhotons.GetOutputName(),
    OutputName = 'PhotonLooseId',
    IdType = mithep.PhotonTools.kSummer15Loose,
    IsoType = mithep.PhotonTools.kSummer15LooseIso,
    ApplyCSafeElectronVeto = False # veto applied in the filler code
)

photonMediumId = photonLooseId.clone('PhotonMediumId',
    OutputName = 'PhotonMediumId',
    IdType = mithep.PhotonTools.kSummer15Medium,
    IsoType = mithep.PhotonTools.kSummer15MediumIso
)

photonTightId = photonLooseId.clone('PhotonTightId',
    OutputName = 'PhotonTightId',
    IdType = mithep.PhotonTools.kSummer15Tight,
    IsoType = mithep.PhotonTools.kSummer15TightIso
)

photonHighPtId = photonLooseId.clone('PhotonHighPtId',
    OutputName = 'PhotonHighPtId',
    IdType = mithep.PhotonTools.kHighPtV2,
    IsoType = mithep.PhotonTools.kHighPtV2Iso,
    ApplyCSafeElectronVeto = True,
    PtMin = 100.
)

loosePhotons = photonLooseId.clone('LoosePhotons',
    IsFilterMode = True,
    OutputName = 'LoosePhotons',
    ApplyCSafeElectronVeto = True
)

#############################################
### FAT JET ID, CORRECTION + SUBSTRUCTURE ###
#############################################

ak8JetCorrection = mithep.JetCorrectionMod('AK8JetCorrection',
    InputName = 'AKt8FatJetsCHS',
    CorrectedJetsName = 'CorrectedAK8Jets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)

for level in jecLevels:
    ak8JetCorrection.AddCorrectionFromFile(jecPattern.format(level = level, jettype = 'AK8PFchs'))

goodAK8Jets = looseAK4Jets.clone('GoodAK8Jets',
    InputName = ak8JetCorrection.GetOutputName(),
    OutputName = 'GoodAK8Jets'
)

ak8JetExtender = mithep.FatJetExtenderMod('AK8JetExtender',
    InputName = goodAK8Jets.GetOutputName(),
    OutputName = 'XlAK8Jets',
    ProcessNJets = 4,
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

goodCA15Jets = looseAK4Jets.clone('GoodCA15Jets',
    InputName = 'CA15FatJetsCHS',
    OutputName = 'GoodCA15Jets'
)

ca15JetExtender = mithep.FatJetExtenderMod('CA15JetExtender',
    InputName = goodCA15Jets.GetOutputName(),
    OutputName = 'XlCA15Jets',
    ProcessNJets = 4,
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

####################
### EVENT FILTER ###
####################

# Met skim
# Filter out events with MET or lepton / photon recoil greater than 90 GeV
# The maximum recoil value in each event is used for the cut
# -> Recoil calculated "offline" with tighter objects may end up lower than 90 GeV

metSkim = mithep.MonoXSkimMod('MetSkim',
    MetName = metCorrection.GetOutputName(),
    AnalysisType = mithep.MonoXSkimMod.kMetOnly,
    GoodElectronsName = veryLooseElectrons.GetOutputName(),
    GoodMuonsName = veryLooseMuons.GetOutputName(),
    GoodPhotonsName = loosePhotons.GetOutputName(),
    MinMetPt = 90.
)
metSkim.SetCategoryActive(mithep.MonoXSkimMod.kMet, True)
metSkim.SetCategoryActive(mithep.MonoXSkimMod.kDielectron, True)
metSkim.SetCategoryActive(mithep.MonoXSkimMod.kDimuon, True)
metSkim.SetCategoryActive(mithep.MonoXSkimMod.kSingleElectron, True)
metSkim.SetCategoryActive(mithep.MonoXSkimMod.kSingleMuon, True)
metSkim.SetCategoryActive(mithep.MonoXSkimMod.kPhoton, True)

# Full event filter is an OR of
# . Met skim
# . At least one baseline electron
# . At least one baseline muon
# Using BooleanMod to express the condition

# Lepton minimum multiplicity

electronBaselineId.SetMinOutput(1)
muonBaselineId.SetMinOutput(1)

def OR(expr1, expr2):
    global mithep
    return mithep.BooleanMod.Expression(expr1, expr2, mithep.BooleanMod.Expression.kOR)

skim = mithep.BooleanMod('Skim',
    Expression = OR(metSkim, OR(electronBaselineId, muonBaselineId))
)

###############
### NEROMOD ###
###############

head = 'HEAD'
tag = 'BAMBU_042_V0004'

neroMod = mithep.NeroMod(
    Info = 'Nero',
    Head = head,
    Tag = tag,
    FileName = 'nero.root',
    PrintLevel = 0,
    Condition = skim
)

neroMod.AddFiller(mithep.nero.EventFiller(
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
))

neroMod.AddFiller(mithep.nero.VertexFiller(
    VerticesName = goodPVFilterMod.GetOutputName()
))

jetsFiller = mithep.nero.JetsFiller(mithep.nero.BaseFiller.kJets,
    JetsName = looseAK4Jets.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    JetIdCutWP = mithep.JetIDMVA.kLoose,
    JetIdMVATrainingSet = mithep.JetIDMVA.k53BDTCHSFullPlusRMS,
    JetIdMVAWeightsFile = mitdata + '/JetId/TMVAClassification_5x_BDT_chsFullPlusRMS.weights.xml',
    JetIdCutsFile = mitdata + '/JetId/jetIDCuts_121221.dat'
)

neroMod.AddFiller(jetsFiller)

neroMod.AddFiller(mithep.nero.JetsFiller(mithep.nero.BaseFiller.kPuppiJets,
    JetsName = puppiJetCorrection.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    JetIdCutWP = mithep.JetIDMVA.nCutTypes,
    JetIdMVATrainingSet = mithep.JetIDMVA.nMVATypes
))

neroMod.AddFiller(mithep.nero.TausFiller(
    TausName = looseTaus.GetOutputName()
))

# "True" -> passing lepton is saved
neroMod.AddFiller(mithep.nero.LeptonsFiller(
    MuonsName = veryLooseMuons.GetOutputName(),
    BaselineMuonIdName = (muonBaselineId.GetOutputName(), True),
    VetoMuonIdName = (muonBaselineId.GetOutputName(), True),
    FakeMuonIdName = (muonFakeId.GetOutputName(), True),
    LooseMuonIdName = (muonLooseId.GetOutputName(), True),
    MediumMuonIdName = (muonMediumId.GetOutputName(), True),
    TightMuonIdName = (muonTightId.GetOutputName(), True),
    LooseMuonIsoName = muonLooseIso.GetOutputName(),
    MediumMuonIsoName = muonMediumIso.GetOutputName(),
    TightMuonIsoName = muonTightIso.GetOutputName(),
    MediumIPMuonIdName = muonMediumIPId.GetOutputName(),
    TightIPMuonIdName = muonTightIPId.GetOutputName(),
    SoftIPMuonIdName = (muonPrivSoftId.GetOutputName(), True),
    ElectronsName = veryLooseElectrons.GetOutputName(),
    BaselineElectronIdName = (electronBaselineId.GetOutputName(), True),
    VetoElectronIdName = (electronVetoId.GetOutputName(), True),
    FakeElectronIdName = (electronFakeId.GetOutputName(), True),
    LooseElectronIdName = (electronLooseId.GetOutputName(), True),
    MediumElectronIdName = (electronMediumId.GetOutputName(), True),
    TightElectronIdName = (electronTightId.GetOutputName(), True),
    LooseElectronIsoName = electronLooseIso.GetOutputName(),
    MediumElectronIsoName = electronMediumIso.GetOutputName(),
    TightElectronIsoName = electronTightIso.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName(),
    PFCandsName = mithep.Names.gkPFCandidatesBrn,
    NoPUPFCandsName = separatePileUpMod.GetPFNoPileUpName(),
    PUPFCandsName = separatePileUpMod.GetPFPileUpName()
))

neroMod.AddFiller(mithep.nero.FatJetsFiller(mithep.nero.BaseFiller.kAK8Jets,
    FatJetsName = ak8JetExtender.GetOutputName()
))

neroMod.AddFiller(mithep.nero.FatJetsFiller(mithep.nero.BaseFiller.kCA15Jets,
    FatJetsName = ca15JetExtender.GetOutputName()
))

metFiller = mithep.nero.MetFiller(
    MetName = metCorrection.GetOutputName(),
    JESUpMetName = metCorrectionJESUp.GetOutputName(),
    JESDownMetName = metCorrectionJESDown.GetOutputName(),
    PuppiMetName = puppiMetCorrection.GetOutputName(),
    MuonsName = tightMuons.GetOutputName()
)
neroMod.AddFiller(metFiller)

neroMod.AddFiller(mithep.nero.PhotonsFiller(
    PhotonsName = baselinePhotons.GetOutputName(),
    LooseIdName = photonLooseId.GetOutputName(),
    MediumIdName = photonMediumId.GetOutputName(),
    TightIdName = photonTightId.GetOutputName(),
    HighPtIdName = photonHighPtId.GetOutputName(),
    VerticesName = goodPVFilterMod.GetOutputName()
))

neroMod.AddFiller(mithep.nero.AllFiller())

triggerFiller = mithep.nero.TriggerFiller()
neroMod.AddFiller(triggerFiller)

################
### TRIGGERS ###
################

triggers = [
    (['PFMETNoMu90_%sCleaned_PFMHTNoMu90_IDTight' % c for c in ['JetId', 'HBHE', 'Noise']] if analysis.isRealData and analysis.custom['bx'] == '25ns' else 'PFMETNoMu90_NoiseCleaned_PFMHTNoMu90_IDTight', []),
    (['PFMETNoMu120_%sCleaned_PFMHTNoMu120_IDTight' % c for c in ['JetId', 'HBHE', 'Noise']] if analysis.isRealData and analysis.custom['bx'] == '25ns' else 'PFMETNoMu120_NoiseCleaned_PFMHTNoMu120_IDTight', []),
    (['PFMET170_%sCleaned' % c for c in ['JetId', 'HBHE', 'Noise']], []),
    ('PFMET170', []),
    ('Ele23_WPLoose_Gsf' if analysis.isRealData else 'Ele22_eta2p1_WP75_Gsf', ['hltEle23WPLooseGsfTrackIsoFilter']),
    ('Ele27_WPLoose_Gsf' if analysis.isRealData else 'Ele27_WP85_Gsf', ['hltEle27WPLooseGsfTrackIsoFilter']), # filter only matches data
    ('Ele12_CaloIdL_TrackIdL_IsoVL', ['hltEle12CaloIdLTrackIdLIsoVLTrackIsoFilter']),
    ('Ele17_CaloIdL_TrackIdL_IsoVL', ['hltEle17CaloIdLTrackIdLIsoVLTrackIsoFilter']),
    ('IsoMu20', ['hltL3crIsoL1sMu16L1f0L2f10QL3f20QL3trkIsoFiltered0p09']),
    ('IsoTkMu20', ['hltL3fL1sMu16L1f0Tkf20QL3trkIsoFiltered0p09']),
    ('IsoMu27', ['hltL3crIsoL1sMu25L1f0L2f10QL3f27QL3trkIsoFiltered0p09']),
    ('Photon120', ['hltEG120HEFilter']),
    ('Photon165_HE10', ['hltEG165HE10Filter']),
    ('Photon175', ['hltEG175HEFilter']),
    ('Ele17_Ele12_CaloIdL_TrackIdL_IsoVL_DZ', []),
    ('Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ', []),
    ('Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ', []),
    ('Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL', ['hltMu8TrkIsoVVLEle17CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered8']),
    ('Mu8_TrkIsoVVL_Ele17_CaloIdL_TrackIdL_IsoVL', ['hltMu8TrkIsoVVLEle17CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter']),
    ('Mu8_TrkIsoVVL_Ele23_CaloIdL_TrackIdL_IsoVL', []),
    ('Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL', ['hltMu17TrkIsoVVLEle12CaloIdLTrackIdLIsoVLMuonlegL3IsoFiltered17']),
    ('Mu17_TrkIsoVVL_Ele12_CaloIdL_TrackIdL_IsoVL', ['hltMu17TrkIsoVVLEle12CaloIdLTrackIdLIsoVLElectronlegTrackIsoFilter']),
    ('Ele12_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Ele18_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Ele23_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Ele33_CaloIdL_TrackIdL_IsoVL_PFJet30', []),
    ('Mu8_TrkIsoVVL', ['hltL3fL1sMu5L1f0L2f5L3Filtered8TkIsoFiltered0p4']),
    ('Mu17_TrkIsoVVL', ['hltL3fL1sMu12L1f0L2f12L3Filtered17TkIsoFiltered0p4']),
    ('Mu24_TrkIsoVVL', ['hltL3fL1sMu16L1f0L2f16L3Filtered24TkIsoFiltered0p4']),
    ('Mu34_TrkIsoVVL', ['hltL3fL1sMu20L1f0L2f20L3Filtered34TkIsoFiltered0p4'])
]

for path, filters in triggers:
    if type(path) is str:
        bit = triggerFiller.AddTriggerName('HLT_' + path + '_v*')
    else:
        bit = triggerFiller.AddTriggerName('HLT_' + path[0] + '_v*')
        for p in path[1:]:
            triggerFiller.AddTriggerName('HLT_' + p + '_v*', bit)

    for filt in filters:
        triggerFiller.AddFilterName(bit, filt)


################
### SEQUENCE ###
################

initialFilterSequence = Chain([
    goodPVFilterMod
])

preskimSequence = Chain([
    veryLooseMuons,
    veryLooseElectrons,
    baselinePhotons,
    loosePhotons,
    metCorrection
])

filterMods = metSkim + muonBaselineId + electronBaselineId

postskimSequence = Chain([
    skim,
    separatePileUpMod,
    puppiMod,
    puppiPFJetMod,
    jetCorrection,
    puppiJetCorrection,
    metCorrectionJESUp,
    metCorrectionJESDown,
    puppiMet,
    puppiMetCorrection,
    looseAK4Jets,
    looseTaus,
    muonPrivSoftId,
    muonFakeId,
    muonLooseId,
    muonLooseIso,
    muonMediumId,
    muonMediumIso,
    muonTightId,
    muonTightIso,
    muonMediumIPId,
    muonTightIPId,
    tightMuons,
    electronVetoId,
    electronVetoIso,
    electronFakeId,
    electronLooseId,
    electronLooseIso,
    electronMediumId,
    electronMediumIso,
    electronTightId,
    electronTightIso,
    photonLooseId,
    photonMediumId,
    photonTightId,
    photonHighPtId,
    ak8JetCorrection,
    goodAK8Jets,
    goodCA15Jets,
    ak8JetExtender,
    ca15JetExtender
])

#############################
### DATA/MC CUSTOMIZATION ###
#############################

if analysis.isRealData:
    badEventsFilterMod = mithep.BadEventsFilterMod('BadEventsFilterMod',
        EEBadScFilter = True,
        HBHENoiseFilter = True,
        FillHist = True
    )

    hltMod = mithep.HLTMod(
        ExportTrigObjects = False
    )

    for path, filters in triggers:
        if type(path) is str:
            hltMod.AddTrigger('HLT_' + path + '_v*')
        else:
            for p in path:
                hltMod.AddTrigger(p)

    initialFilterSequence = badEventsFilterMod * hltMod * initialFilterSequence

else:
    generator = mithep.GeneratorMod(
        IsData = False,
        CopyArrays = False,
        MCMETName = "GenMet",
        FillHist = True
    )

    mcParticlesNoNu = mithep.MCParticleFilterMod('MCParticlesNoNu',
        InputName = mithep.Names.gkMCPartBrn,
        OutputName = 'MCParticlesNoNu',
        VetoParticleId = True
    )
    for pid in [12, 14, 16, 1000022]:
        mcParticlesNoNu.AddParticleId(pid)

    genJets = mithep.FastJetMod('GenJetsNoNu',
        InputName = mcParticlesNoNu.GetOutputName(),
        OutputJetsName = 'GenJetsNoNu',
        OutputType = mithep.kGenJet,
        ConeSize = 0.4,
        NoActiveArea = True,
        ParticleMinPt = 0.,
        JetMinPt = 3.
    )

    postskimSequence *= Chain([generator, mcParticlesNoNu, genJets])

    metFiller.SetGenMetName(generator.GetMCMETName())

    mcFiller = mithep.nero.MonteCarloFiller(
        GenJetsName = genJets.GetOutputJetsName()
    )
    if 'pdfrwgt' in analysis.custom and analysis.custom['pdfrwgt'] != '-':
        if analysis.custom['pdfrwgt'] == 'amc_74':
            mcFiller.AddPdfReweightGroupName('PDF_variation')
        elif analysis.custom['pdfrwgt'] == 'mg5_74':
            mcFiller.AddPdfReweightGroupName('NNPDF30_lo_as_0130.LHgrid')
        elif analysis.custom['pdfrwgt'] == 'pwhg_74':
            for rid in range(9, 111):
                mcFiller.AddPdfReweightId(rid) # 9-108: 260000 family, 109: 265000, 110: 266000
        else:
            print 'Unrecognized pdfrwgt option', analysis.custom['pdfrwgt']
            sys.exit(1)
        
    neroMod.AddFiller(mcFiller)


# neroMod must be independent of the main chain
# to ensure that the all events tree is filled properly
analysis.setSequence(initialFilterSequence * (preskimSequence + filterMods + postskimSequence) + neroMod)
