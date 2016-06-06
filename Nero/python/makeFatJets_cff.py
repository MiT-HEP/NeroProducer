
import FWCore.ParameterSet.Config as cms
#-----------------------FAT JET CLUSTERING-----------------------
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
from RecoJets.JetProducers.ak4PFJetsPuppi_cfi import ak4PFJetsPuppi
from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
from PhysicsTools.PatAlgos.tools.pfTools import *
from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness
from RecoBTag.Configuration.RecoBTag_cff import *


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
      'pfCombinedSecondaryVertexV2BJetTags'
      ,'pfCombinedInclusiveSecondaryVertexV2BJetTags'
  ]
  
  process.pfCHS = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("fromPV"))
  if isMC:
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

  if isMC:
    process.fatjetInitSequence = cms.Sequence(
        process.packedGenParticlesForJetsNoNu+
        process.ak4GenJetsNoNu
    )
  else:
    process.fatjetInitSequence = cms.Sequence()
  process.fatjetInitSequence += process.pfCHS
  process.fatjetInitSequence += process.ak4PFJets

  return process.fatjetInitSequence

def makeFatJets(process,isData,pfCandidates,algoLabel,jetRadius):
  
  isMC = not isData
  postfix='PFlow'
  if pfCandidates=='particleFlow':
    # mini aod needs a different config
    pfCandidates = 'pfCHS'
  
  if pfCandidates=='pfCHS':
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

  setattr(process,neroLabel+'Sequence',cms.Sequence())
  newSeq = getattr(process,neroLabel+'Sequence')
  
  ## Various collection names
  genParticles = 'prunedGenParticles'
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
      'pfCombinedSecondaryVertexV2BJetTags'
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

  bTagInfosSubjets=['None']
  bTagDiscriminatorsSubjets=['None']

  ### jet clustering ### 
  ########################################
  ##           REMAKE JETS              ##
  ########################################

  addingGenJets = False

  if not(hasattr(process,"genJetsNoNu"+rLabel)) and isMC:
    addingGenJets = True
    setattr(process,"genJetsNoNu"+rLabel, ak4GenJets.clone(
                                           jetAlgorithm = cms.string(jetAlgo),
                                           rParam = cms.double(jetRadius),
                                           src = cms.InputTag("packedGenParticlesForJetsNoNu")
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
  if not(hasattr(process,"genJetsNoNuSoftDrop"+rLabel)) and isMC:
    addingGenJets = True
    setattr(process,"genJetsNoNuSoftDrop"+rLabel, getattr(process,'genJetsNoNu'+rLabel).clone(
                                                      R0 = cms.double(jetRadius),
                                                      useSoftDrop = cms.bool(True),
                                                      zcut = cms.double(sdZcut),
                                                      beta = cms.double(sdBeta),
                                                      writeCompound = cms.bool(True),
                                                      jetCollInstanceName=cms.string("SubJets")
                                                  )
    )
  setattr(process,"PFJets"+"SoftDrop"+neroLabel, getattr(process,'PFJets'+neroLabel).clone(
                                                          useSoftDrop = cms.bool(True),
                                                          R0 = cms.double(jetRadius),
                                                          zcut = cms.double(sdZcut),
                                                          beta = cms.double(sdBeta),
                                                          writeCompound = cms.bool(True),
                                                          useExplicitGhosts = cms.bool(True),
                                                          jetCollInstanceName=cms.string("SubJets"),
                                                          jetPtMin = cms.double(150)
                                                      )
  )
  process.dump = cms.EDAnalyzer("EventContentAnalyzer")
  if addingGenJets and not(isData):
    print 'addingGenJets', 'genJetsNoNu'+rLabel
    newSeq += getattr(process,'genJetsNoNu'+rLabel)
    newSeq += getattr(process,'genJetsNoNuSoftDrop'+rLabel)
  newSeq += getattr(process,'PFJets'+neroLabel)
  newSeq += getattr(process,'PFJets'+"SoftDrop"+neroLabel)
  
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

  newSeq += getattr(process,neroLabel+'SDKinematics')
  newSeq += getattr(process,neroLabel+'Njettiness')

  ### subjet b-tagging ###

  setattr(process,neroLabel+'PFImpactParameterTagInfos',
      pfImpactParameterTagInfos.clone(
          jets      = cms.InputTag('PFJets'+"SoftDrop"+neroLabel,'SubJets'),
          maxDeltaR = cms.double(jetRadius),
          primaryVertex = cms.InputTag('offlineSlimmedPrimaryVertices'),
          candidates = cms.InputTag('packedPFCandidates')
      )
  )

  setattr(process,neroLabel+'PFInclusiveSecondaryVertexFinderTagInfos',
    pfInclusiveSecondaryVertexFinderTagInfos.clone(
      trackIPTagInfos = cms.InputTag(neroLabel+'PFImpactParameterTagInfos'),
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
  newSeq += getattr(process,neroLabel+'PFImpactParameterTagInfos')
  newSeq += getattr(process,neroLabel+'PFInclusiveSecondaryVertexFinderTagInfos')
  newSeq += getattr(process,neroLabel+'PFCombinedInclusiveSecondaryVertexV2BJetTags')
  
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
      pfCandidates = cms.InputTag('packedPFCandidates'),
      pvSource = cms.InputTag(pvSource),
      svSource = cms.InputTag(svSource),
      muSource = cms.InputTag(muSource),
      elSource = cms.InputTag(elSource),
      btagInfos = bTagInfos,
      btagDiscriminators = bTagDiscriminators,
      genJetCollection = cms.InputTag('genJetsNoNu'+rLabel),
      genParticles = cms.InputTag(genParticles),
      getJetMCFlavour = False, # jet flavor disabled
  )
  getattr(process,'selectedPatJetsPF'+neroLabel).cut = cms.string("abs(eta) < " + str(2.5))
  ## SOFT DROP ##
  addJetCollection(
      process,
      labelName='SoftDropPF'+neroLabel,
      jetSource=cms.InputTag('PFJets'+"SoftDrop"+neroLabel),
      pfCandidates = cms.InputTag('packedPFCandidates'),
      algo=algoLabel,
      rParam=jetRadius,
      btagInfos = ['None'],
      btagDiscriminators = ['None'],
      genJetCollection = cms.InputTag('genJetsNoNu'+rLabel),
      genParticles = cms.InputTag(genParticles),
      getJetMCFlavour = False, # jet flavor disabled
  )
  addJetCollection(
      process,
      labelName='SoftDropSubjetsPF'+neroLabel,
      jetSource=cms.InputTag('PFJets'+"SoftDrop"+neroLabel,'SubJets'),
      algo=algoLabel,  
      rParam=jetRadius, 
      pfCandidates = cms.InputTag('packedPFCandidates'),
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
      fatJets = cms.InputTag('PFJets'+neroLabel),              # needed for subjet flavor clustering
      groomedFatJets = cms.InputTag('PFJets'+"SoftDrop"+neroLabel), # needed for subjet flavor clustering
      runIVF = False,
      getJetMCFlavour = False, # jet flavor disabled
  )
  
  isMC = not(isData)
  if isMC:
    newSeq += getattr(process,'patJetPartonMatchPF'+neroLabel)
    newSeq += getattr(process,'patJetGenJetMatchPF'+neroLabel)
  newSeq += getattr(process,'patJetsPF'+neroLabel)
  newSeq += getattr(process,'selectedPatJetsPF'+neroLabel)
  
  if isMC:
    newSeq += getattr(process,'patJetPartonMatchSoftDropPF'+neroLabel)
    newSeq += getattr(process,'patJetGenJetMatchSoftDropPF'+neroLabel)
  newSeq += getattr(process,'patJetsSoftDropPF'+neroLabel)
  newSeq += getattr(process,'selectedPatJetsSoftDropPF'+neroLabel)
  
  if isMC:
    newSeq += getattr(process,'patJetPartonMatchSoftDropSubjetsPF'+neroLabel)
    newSeq += getattr(process,'patJetGenJetMatchSoftDropSubjetsPF'+neroLabel)
  newSeq += getattr(process,'patJetsSoftDropSubjetsPF'+neroLabel)
  newSeq += getattr(process,'selectedPatJetsSoftDropSubjetsPF'+neroLabel)
  
  setattr(process,"selectedPatJetsSoftDropPF"+"Packed"+neroLabel, 
          cms.EDProducer("BoostedJetMerger",    
                          jetSrc=cms.InputTag("selectedPatJetsSoftDropPF"+neroLabel),
                          subjetSrc=cms.InputTag("selectedPatJetsSoftDropSubjetsPF"+neroLabel)  
          )
  )
  ## PACK ##
  setattr(process,"packedPatJetsPF"+neroLabel, 
          cms.EDProducer("JetSubstructurePacker",
                          jetSrc = cms.InputTag('selectedPatJetsPF'+neroLabel),
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
  getattr(process,'patJetsPF'+neroLabel).userData.userFloats.src += [neroLabel+'Njettiness:tau1']
  getattr(process,'patJetsPF'+neroLabel).userData.userFloats.src += [neroLabel+'Njettiness:tau2']
  getattr(process,'patJetsPF'+neroLabel).userData.userFloats.src += [neroLabel+'Njettiness:tau3']
  getattr(process,'patJetsPF'+neroLabel).userData.userFloats.src += [neroLabel+'Njettiness:tau4']
  getattr(process,'patJetsPF'+neroLabel).userData.userFloats.src += [neroLabel+'SDKinematics:Mass']

  for m in ['patJetsPF'+neroLabel,'patJetsSoftDropSubjetsPF'+neroLabel]:
    if hasattr(process,m) and getattr( getattr(process,m),'addBTagInfo'):
      setattr( getattr(process,m), 'addTagInfos', cms.bool(True))
    #if hasattr(process,m):
    #  setattr( getattr(process,m), 'addJetFlavourInfo', cms.bool(True))

  newSeq += cms.Sequence(
      getattr(process,'selectedPatJetsSoftDropPF'+"Packed"+neroLabel)+
      getattr(process,'packedPatJetsPF'+neroLabel)
  )

  if isData:
      removeMCMatching(process, ['All'], outputModules = [])
  return newSeq
