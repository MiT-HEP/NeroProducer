
import FWCore.ParameterSet.Config as cms
#-----------------------FAT JET CLUSTERING-----------------------
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.ak4PFJetsPuppi_cfi import ak4PFJetsPuppi
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
from PhysicsTools.PatAlgos.tools.pfTools import *
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness

def initFatJets(process,isData):
  adaptPVs(process, pvCollection=cms.InputTag('offlineSlimmedPrimaryVertices'))
  isMC = not isData

  ########################################
  ##         INITIAL SETUP              ##
  ########################################
  jetCorrectionsAK4 = ('AK4PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
  jetCorrectionsAK8 = ('AK8PFchs', ['L1FastJet', 'L2Relative', 'L3Absolute'], 'None')
  if isData:
      jetCorrectionsAK4[1].append('L2L3Residual')
      jetCorrectionsAK8[1].append('L2L3Residual')

  ## Various collection names
  genParticles = 'prunedGenParticles'
  jetSource = 'ak4PFJets'
  genJetCollection = 'ak4GenJetsNoNu'
  pfCandidates = 'packedPFCandidates'
  pvSource = 'offlineSlimmedPrimaryVertices'
  svSource = 'slimmedSecondaryVertices'
  muSource = 'slimmedMuons'
  elSource = 'slimmedElectrons'

  PFjetAlgo="AK4"

  ## Load standard PAT objects
  process.load("PhysicsTools.PatAlgos.producersLayer1.patCandidates_cff")
  process.load("PhysicsTools.PatAlgos.selectionLayer1.selectedPatCandidates_cff")

  bTagInfos = [
      'pfImpactParameterTagInfos'
     ,'pfSecondaryVertexTagInfos'
     ,'pfInclusiveSecondaryVertexFinderTagInfos'
     ,'softPFMuonsTagInfos'
     ,'softPFElectronsTagInfos'
  ]
  ## b-tag discriminators
  bTagDiscriminators = [
       'pfJetBProbabilityBJetTags'
      ,'pfJetProbabilityBJetTags'
      ,'pfTrackCountingHighPurBJetTags'
      ,'pfTrackCountingHighEffBJetTags'
      ,'pfCombinedSecondaryVertexV2BJetTags'
      ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
  ]
  
  process.pfCHS = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV"))
  process.packedGenParticlesForJetsNoNu = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedGenParticles"), cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16"))
  process.ak4GenJetsNoNu = ak4GenJets.clone(src = 'packedGenParticlesForJetsNoNu')
  process.ak4PFJets = ak4PFJets.clone(src='pfCHS',doAreaFastjet=True)

  postfix='PFlow'
  
  ## Switch the default jet collection (done in order to use the above specified b-tag infos and discriminators)
  switchJetCollection(
      process,
      jetSource = cms.InputTag(jetSource),
      pfCandidates = cms.InputTag(pfCandidates),
      pvSource = cms.InputTag(pvSource),
      svSource = cms.InputTag(svSource),
      muSource = cms.InputTag(muSource),
      elSource = cms.InputTag(elSource),
      btagInfos = bTagInfos,
      btagDiscriminators = bTagDiscriminators,
      jetCorrections = jetCorrectionsAK4,
      genJetCollection = cms.InputTag(genJetCollection),
      genParticles = cms.InputTag(genParticles),
      postfix = postfix
  )

  process.fatjetInitSequence = cms.Sequence(
      process.genParticlesForJetsNoNu*
      process.ak4GenJetsNoNu*
      process.pfCHS*
      process.ak4PFJets*

  )

  return process.fatjetInitSequence

def makeFatJets(process,isData,pfCandidates,algoLabel,jetRadius):
    
  postfix='PFlow'
  if pfCandidates=='particleFlow':
    puMethod='CHS'
  else:
    puMethod='Puppi'

  rLabel = algoLabel+str(int(jetRadius*10))
  neroLabel = rLabel+puMethod

  if algoLabel=='CA':
    jetAlgo = 'CambridgeAachen'
  else:
    jetAlgo = 'AntiKt'

  if jetRadius<1:
    sdZcut = 0.1
    sdBeta = 0.0
  else:
    sdZcut = 0.2
    sdBeta = 1.0

  setattr(process,rLabel+'Sequence',cms.Sequence())
  newSeq = getattr(process,rLabel+'Sequence')
  
  ## Various collection names
  genParticles = 'prunedGenParticles'
  jetSource = 'ak4PFJets'
  genJetCollection = 'ak4GenJetsNoNu'
  pvSource = 'offlineSlimmedPrimaryVertices'
  svSource = 'slimmedSecondaryVertices'
  muSource = 'slimmedMuons'
  elSource = 'slimmedElectrons'
  bTagInfos = [
      'pfImpactParameterTagInfos'
     ,'pfSecondaryVertexTagInfos'
     ,'pfInclusiveSecondaryVertexFinderTagInfos'
     ,'softPFMuonsTagInfos'
     ,'softPFElectronsTagInfos'
  ]
  ## b-tag discriminators
  bTagDiscriminators = [
       'pfJetBProbabilityBJetTags'
      ,'pfJetProbabilityBJetTags'
      ,'pfTrackCountingHighPurBJetTags'
      ,'pfTrackCountingHighEffBJetTags'
      ,'pfCombinedSecondaryVertexV2BJetTags'
      ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
  ]


  bTagInfosSubjets = [
      'pfImpactParameterTagInfos'
     ,'pfSecondaryVertexTagInfos'
     ,'pfInclusiveSecondaryVertexFinderTagInfos'
     ,'softPFMuonsTagInfos'
     ,'softPFElectronsTagInfos'
  ]
  ## b-tag discriminators
  bTagDiscriminatorsSubjets = [
      'pfCombinedSecondaryVertexV2BJetTags'
      ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
  ]

  ### jet clustering ### 
  ########################################
  ##           REMAKE JETS              ##
  ########################################

  addingGenJets = False

  if not(hasattr(process,"genJetsNoNu"+rLabel)):
    addingGenJets = True
    setattr(process,"genJetsNoNu"+rLabel, ak4GenJets.clone(
                                           jetAlgorithm = cms.string(jetAlgo),
                                           rParam = cms.double(jetRadius),
                                           src = cms.InputTag("genParticlesForJetsNoNu"+postfix)
                                         )
    )
  setattr(process,"PFJets"+neroLabel, ak4PFJets.clone(
                                                jetAlgorithm = cms.string(jetAlgo),
                                                rParam = cms.double(jetRadius),
                                                src = cms.InputTag(pfCandidates),
                                                srcPVs = cms.InputTag(pvSource),
                                                doAreaFastjet = cms.bool(True),
                                                jetPtMin = cms.double(150)
                                            )
  )
  if not(hasattr(process,"genJetsNoNuSoftDrop"+rLabel)):
    addingGenJets = True
    setattr(process,"genJetsNoNuSoftDrop"+rLabel, ak4GenJets.clone(
                                                      jetAlgorithm = cms.string(jetAlgo),
                                                      rParam = cms.double(jetRadius),
                                                      R0 = cms.double(jetRadius),
                                                      src = cms.InputTag("genParticlesForJetsNoNu"+postfix),
                                                      useSoftDrop = cms.bool(True),
                                                      zcut = cms.double(sdZcut),
                                                      beta = cms.double(sdBeta),
                                                      writeCompound = cms.bool(True),
                                                      jetCollInstanceName=cms.string("SubJets")
                                                  )
    )
  setattr(process,"PFJets"+"SoftDrop"+neroLabel, ak4PFJetsSoftDrop.clone(
                                                          jetAlgorithm = cms.string(jetAlgo),
                                                          rParam = cms.double(jetRadius),
                                                          R0 = cms.double(jetRadius),
                                                          zcut = cms.double(sdZcut),
                                                          beta = cms.double(sdBeta),
                                                          src = cms.InputTag(pfCandidates),
                                                          srcPVs = cms.InputTag(pvSource),
                                                          doAreaFastjet = cms.bool(True),
                                                          writeCompound = cms.bool(True),
                                                          jetCollInstanceName=cms.string("SubJets"),
                                                          jetPtMin = cms.double(150)
                                                      )
  )
  if addingGenJets:
    newSeq *= getattr(process,'genJetsNoNu'+rLabel)
    newSeq *= getattr(process,'genJetsNoNuSoftDrop'+rLabel)
  newSeq *= getattr(process,'PFJets'+neroLabel)
  newSeq *= getattr(process,'PFJets'+"SoftDrop"+neroLabel)
  
  ########################################
  ##           SUBSTRUCTURE             ##
  #######################################

  setattr(process,neroLabel+'Njettiness',                          
          Njettiness.clone(                                      
            src = cms.InputTag('PFJets'+neroLabel),       
            R0 = cms.double(jetRadius),
            Njets = cms.vuint32(1,2,3,4)
          )
  )

  setattr(process,neroLabel+'SDKinematics',
      cms.EDProducer('RecoJetDeltaRValueMapProducer',
            src = cms.InputTag('PFJets'+neroLabel),
            matched = cms.InputTag('PFJets'+"SoftDrop"+neroLabel),
            distMax = cms.double(1.5),
            values = cms.vstring('mass'),
            valueLabels = cms.vstring('Mass'),
      )
  )

  newSeq *= getattr(process,neroLabel+'Njettiness')
  newSeq *= getattr(process,neroLabel+'SDKinematics')

  ### subjet b-tagging ###
  from RecoBTag.Configuration.RecoBTag_cff import *

  setattr(process,neroLabel+'PFImpactParameterTagInfos',
      pfImpactParameterTagInfos.clone(
          jets      = cms.InputTag('PFJets'+"SoftDrop"+neroLabel,'SubJets'),
          maxDeltaR = cms.double(jetRadius),
          primaryVertex = cms.InputTag('offlineSlimmedPrimaryVertices'),
          candidates = cms.InputTag(pfCandidates)
      )
  )

  setattr(process,neroLabel+'PFInclusiveSecondaryVertexFinderTagInfos',
    pfInclusiveSecondaryVertexFinderTagInfos.clone(
      trackIPTagInfos = cms.InputTag("CA15PFImpactParameterTagInfosSJPuppi"),
      extSVCollection = cms.InputTag('slimmedSecondaryVertices')
    )
  )
  setattr(process,neroLabel+'PFCombinedInclusiveSecondaryVertexV2BJetTags',
    pfCombinedInclusiveSecondaryVertexV2BJetTags.clone(
      tagInfos = cms.VInputTag( 
        cms.InputTag(neroLabel+"PFImpactParameterTagInfos"), 
        cms.InputTag(neroLabel+"PFInclusiveSecondaryVertexFinderTagInfos") 
      )
    )
  )
  newSeq *= getattr(process,neroLabel+'PFImpactParameterTagInfos')
  newSeq *= getattr(process,neroLabel+'PFInclusiveSecondaryVertexFinderTagInfos')
  newSeq *= getattr(process,neroLabel+'PFCombinedInclusiveSecondaryVertexV2BJetTags')
  
  bTagInfos = ['None']
  bTagDiscriminators = ['None']

  ########################################
  ##          MAKE PAT JETS             ##
  ########################################

  addJetCollection(
      process,
      labelName='PF'+neroLabel,
      jetSource=cms.InputTag('PFJets'+neroLabel),
      algo=algoLabel,           # needed for jet flavor clustering
      rParam=jetRadius, # needed for jet flavor clustering
      pfCandidates = cms.InputTag(pfCandidates),
      pvSource = cms.InputTag(pvSource),
      svSource = cms.InputTag(svSource),
      muSource = cms.InputTag(muSource),
      elSource = cms.InputTag(elSource),
      btagInfos = bTagInfos,
      btagDiscriminators = bTagDiscriminators,
      genJetCollection = cms.InputTag('genJetsNoNu'+rLabel),
      genParticles = cms.InputTag(genParticles),
      postfix = postfix
  )
  getattr(process,'selectedPatJetsPF'+neroLabel+postfix).cut = cms.string("abs(eta) < " + str(2.5))
  ## SOFT DROP ##
  addJetCollection(
      process,
      labelName='SoftDropPF'+neroLabel,
      jetSource=cms.InputTag('PFJets'+"SoftDrop"+neroLabel),
      algo=algoLabel,
      btagInfos = ['None'],
      btagDiscriminators = ['None'],
      genJetCollection = cms.InputTag('genJetsNoNu'+rLabel),
      genParticles = cms.InputTag(genParticles),
      getJetMCFlavour = False, # jet flavor disabled
      postfix = postfix
  )
  addJetCollection(
      process,
      labelName='SoftDropSubjetsPF'+neroLabel,
      jetSource=cms.InputTag('PFJets'+"SoftDrop"+neroLabel,'SubJets'),
      algo=algoLabel,  
      rParam=jetRadius, 
      pfCandidates = cms.InputTag(pfCandidates),
      pvSource = cms.InputTag(pvSource),
      svSource = cms.InputTag(svSource),
      muSource = cms.InputTag(muSource),
      elSource = cms.InputTag(elSource),
      btagInfos = bTagInfosSubjets,
      btagDiscriminators = bTagDiscriminatorsSubjets,
      genJetCollection = cms.InputTag('genJetsNoNuSoftDrop'+rLabel,'SubJets'),
      genParticles = cms.InputTag(genParticles),
      explicitJTA = True,  # needed for subjet b tagging
      svClustering = True, # needed for subjet b tagging
      getJetMCFlavour = False, # jet flavor disabled
      fatJets = cms.InputTag('PFJets'+neroLabel),              # needed for subjet flavor clustering
      groomedFatJets = cms.InputTag('PFJets'+"SoftDrop"+neroLabel), # needed for subjet flavor clustering
      runIVF = False,
      postfix = postfix
  )
  setattr(process,"selectedPatJetsSoftDropPF"+"Packed"+neroLabel, 
          cms.EDProducer("BoostedJetMerger",    
                          jetSrc=cms.InputTag("selectedPatJetsSoftDropPF"+neroLabel+postfix),
                          subjetSrc=cms.InputTag("selectedPatJetsSoftDropSubjetsPF"+neroLabel+postfix)  
          )
  )
  ## PACK ##
  setattr(process,"packedPatJetsPF"+neroLabel, 
          cms.EDProducer("JetSubstructurePacker",
                          jetSrc = cms.InputTag('selectedPatJetsPF'+neroLabel+postfix),
                          distMax = cms.double(jetRadius),
                          algoTags = cms.VInputTag(),
                          algoLabels = cms.vstring(),
                          fixDaughters = cms.bool(False)
                        )
  )
  getattr(process,"packedPatJetsPF"+neroLabel).algoTags.append(
      cms.InputTag('selectedPatJetsSoftDropPF'+"Packed"+neroLabel)
  )
  getattr(process,"packedPatJetsPF"+neroLabel).algoLabels.append(
      'SoftDrop'
  )
  getattr(process,'patJetsPF'+neroLabel+postfix).userData.userFloats.src += [neroLabel+'Njettiness:tau1']
  getattr(process,'patJetsPF'+neroLabel+postfix).userData.userFloats.src += [neroLabel+'Njettiness:tau2']
  getattr(process,'patJetsPF'+neroLabel+postfix).userData.userFloats.src += [neroLabel+'Njettiness:tau3']
  getattr(process,'patJetsPF'+neroLabel+postfix).userData.userFloats.src += [neroLabel+'Njettiness:tau4']
  getattr(process,'patJetsPF'+neroLabel+postfix).userData.userFloats.src += [neroLabel+'SDKinematics:Mass']

  for m in ['patJetsPF'+neroLabel+postfix,'patJetsSoftDropSubjetsPF'+neroLabel+postfix]:
    if hasattr(process,m) and getattr( getattr(process,m),'addBTagInfo'):
      setattr( getattr(process,m), 'addTagInfos', cms.bool(True))
    if hasattr(process,m):
      setattr( getattr(process,m), 'addJetFlavourInfo', cms.bool(False))

  newSeq *= cms.Sequence(
      getattr(process,'patJetsSoftDropPF'+neroLabel+'PFlow')*
      getattr(process,'selectedPatJetsSoftDropPF'+neroLabel+'PFlow')*
      getattr(process,'patJetsSoftDropSubjetsPF'+neroLabel+'PFlow')*
      getattr(process,'selectedPatJetsSoftDropSubjetsPF'+neroLabel+'PFlow')*
      getattr(process,'selectedPatJetsSoftDropPF'+"Packed"+neroLabel)*
      getattr(process,'patJetsPF'+neroLabel+'PFlow')*
      getattr(process,'selectedPatJetsPF'+neroLabel+'PFlow')*
      getattr(process,'packedPatJetsPF'+neroLabel)
  )

  if not(isData):
    if addingGenJets:
      newSeq.insert(0,cms.Sequence(getattr(process,'genJetsNoNu'+rLabel)*
                                   getattr(process,'genJetsNoNuSoftDrop'+rLabel)
                      )
      )
  return newSeq
