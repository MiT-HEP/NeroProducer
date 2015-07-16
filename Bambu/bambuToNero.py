from MitAna.TreeMod.bambu import mithep, analysis

import ROOT
ROOT.gSystem.Load('libMitNero.so')
setattr(mithep, 'nero', ROOT.mithep.nero)

import os
from subprocess import check_output

from MitPhysics.Mods.GoodPVFilterMod import goodPVFilterMod
from MitPhysics.Mods.JetCorrectionMod import jetCorrectionMod
from MitPhysics.Mods.JetIdMod import jetIdMod
from MitPhysics.Mods.PFTauIdMod import pfTauIdMod
from MitPhysics.Mods.ElectronIdMod import electronIdMod
from MitPhysics.Mods.MuonIdMod import muonIdMod
from MitPhysics.Mods.PhotonIdMod import photonIdMod
from MitPhysics.Mods.SeparatePileUpMod import separatePileUpMod

# TODO replace when clone() is available
electronTightId = mithep.ElectronIdMod(
    IsFilterMode = False,
    InputName = electronIdMod.GetOutputName(),
    OutputName = 'TightElectronId',
    IdType = mithep.ElectronTools.kPhys14Tight,
    IsoType = mithep.ElectronTools.kPhys14TightIso,
    ApplyEcalFiducial = True,
    WhichVertex = 0,
    PtMin = 10.,
    EtaMax = 2.5,
    ConversionsName = 'Conversions'
)

muonTightId = mithep.MuonIdMod(
    IsFilterMode = False,
    InputName = muonIdMod.GetOutputName(),
    OutputName = 'TightMuonId',
    IdType = mithep.MuonTools.kMuonPOG2012CutBasedIdTight,
    IsoType = mithep.MuonTools.kPFIsoBetaPUCorrected,
    PFNoPileupCandidatesName = separatePileUpMod.GetPFNoPileUpName(),
    PFPileupCandidatesName = separatePileUpMod.GetPFPileUpName(),
    PtMin = 10.,
    EtaMax = 2.4
)

fatJetCorrectionMod = mithep.JetCorrectionMod(
    InputName = 'AKt8PFJetsCHS',
    CorrectedJetsName = 'CorrectedFatJets',
    RhoAlgo = mithep.PileupEnergyDensity.kFixedGridFastjetAll
)
fatJetCorrectionMod.AddCorrectionFromFile("/local/yiiyama/jec/MCRUN2_74_V9_L1FastJet_AK8PFchs.txt")
fatJetCorrectionMod.AddCorrectionFromFile("/local/yiiyama/jec/MCRUN2_74_V9_L2Relative_AK8PFchs.txt")
fatJetCorrectionMod.AddCorrectionFromFile("/local/yiiyama/jec/MCRUN2_74_V9_L3Absolute_AK8PFchs.txt")

fatJetIdMod = mithep.JetIdMod(
    InputName = fatJetCorrectionMod.GetOutputName(),
    OutputName = 'GoodFatJets',
    ApplyPFLooseId = True,
    PtMin = 30.,
    EtaMax = 2.5
)

photonMediumId = mithep.PhotonIdMod('PhotonMediumId',
    IsFilterMode = False,
    InputName = photonIdMod.GetOutputName(),
    OutputName = 'PhotonMediumId',
    IdType = mithep.PhotonTools.kPhys14Medium,
    IsoType = mithep.PhotonTools.kPhys14Medium,
    PtMin = 15.,
    EtaMax = 2.5
)

photonTightId = mithep.PhotonIdMod('PhotonTightId',
    IsFilterMode = False,
    InputName = photonIdMod.GetOutputName(),
    OutputName = 'PhotonTightId',
    IdType = mithep.PhotonTools.kPhys14Tight,
    IsoType = mithep.PhotonTools.kPhys14Tight,
    PtMin = 15.,
    EtaMax = 2.5
)

head = check_output("cd " + os.environ['CMSSW_BASE'] + "/src/NeroProducer/ && git rev-parse HEAD && cd - 2>&1 >/dev/null", shell = True)
tag = check_output("cd " + os.environ["CMSSW_BASE"] + "/src/NeroProducer && { git describe --tags || true ; } && cd - 2>&1 >/dev/null", shell = True)

fillers = []

fillers.append(mithep.nero.EventFiller())
fillers[-1].SetRhoAlgo(mithep.PileupEnergyDensity.kFixedGridFastjetAll)

fillers.append(mithep.nero.VertexFiller())
fillers[-1].SetVerticesName(goodPVFilterMod.GetOutputName())

fillers.append(mithep.nero.JetsFiller())
fillers[-1].SetJetsName(jetIdMod.GetOutputName())

fillers.append(mithep.nero.TausFiller())
fillers[-1].SetTausName(pfTauIdMod.GetOutputName())

fillers.append(mithep.nero.LeptonsFiller())
fillers[-1].SetElectronsName(electronIdMod.GetOutputName())
fillers[-1].SetMuonsName(muonIdMod.GetOutputName())
fillers[-1].SetElectronIdsName(electronTightId.GetOutputName())
fillers[-1].SetMuonIdsName(muonTightId.GetOutputName())
fillers[-1].SetVerticesName(goodPVFilterMod.GetOutputName())
fillers[-1].SetPFCandsName(mithep.Names.gkPFCandidatesBrn)
fillers[-1].SetNoPUPFCandsName(separatePileUpMod.GetPFNoPileUpName())
fillers[-1].SetPUPFCandsName(separatePileUpMod.GetPFPileUpName())

fillers.append(mithep.nero.FatJetsFiller())
fillers[-1].SetFatJetsName(fatJetIdMod.GetOutputName())

fillers.append(mithep.nero.PhotonsFiller())
fillers[-1].SetPhotonsName(photonIdMod.GetOutputName())
fillers[-1].SetMediumIdName(photonMediumId.GetOutputName())
fillers[-1].SetTightIdName(photonTightId.GetOutputName())
fillers[-1].SetVerticesName(goodPVFilterMod.GetOutputName())

fillers.append(mithep.nero.MonteCarloFiller())

fillers.append(mithep.nero.TriggerFiller())

fillers.append(mithep.nero.AllFiller())

neroMod = mithep.NeroMod(
    Info = 'Nero',
    Head = head,
    Tag = tag,
    FileName = 'nero.root'
)
for filler in fillers:
    neroMod.AddFiller(filler)

analysis.setSequence(
    goodPVFilterMod *
    separatePileUpMod *
    jetCorrectionMod *
    jetIdMod *
    pfTauIdMod *
    electronIdMod *
    muonIdMod *
    photonIdMod *
    electronTightId *
    muonTightId *
    fatJetCorrectionMod *
    fatJetIdMod *
    photonMediumId *
    photonTightId *
    neroMod
)
