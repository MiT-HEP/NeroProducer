import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import re

process = cms.Process("nero")

options = VarParsing.VarParsing ('analysis')
options.register('isData',
                 False,
                 VarParsing.VarParsing.multiplicity.singleton,
                 VarParsing.VarParsing.varType.bool,
                 "True if running on Data, False if running on MC")

options.register('isGrid', False, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"Set it to true if running on Grid")
options.register('nerohead', "XXX", VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"Set to the head of the repository. use check_output 'git rev-parse HEAD' in the crab py file. active only if isGrid.")
options.register("nerotag", "YYY", VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.string,"Set to the tag of the repository. use check_output 'git rev-parse HEAD' in the crab py file. active only if isGrid.")
options.register('isParticleGun', False, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"Set it to true if MonteCarlo is ParticleGun")
options.register('is25ns', True, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"Set it to true to run on 25ns data/MC")
options.register('is50ns', False, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"Set it to true to run on 50ns data/MC")

options.parseArguments()
isData = options.isData

if options.is25ns and options.is50ns : 
	raise('cannot run both on 25 and 50ns. Pick up one')
if not options.is25ns and not options.is50ns:
	raise('cannot run nor 25ns nor 50ns configuration. Pick up one.')

if options.is25ns:
	print "-> Loading 25ns configuration"
if options.is50ns:
	print "-> Loading 50ns configuration"
if options.isData:
	print "-> Loading DATA configuration"
else:
	print "-> Loading MC configuration"

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

fileList = [
    #'file:/tmp/amarini/step3_0.root'
    #'/store/data/Run2015D/SinglePhoton/MINIAOD/PromptReco-v3/000/256/630/00000/BE4748B0-295F-E511-A271-02163E014402.root',
    '/store/mc/RunIISpring15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/74X_mcRun2_asymptotic_v2-v1/50000/E4F89698-DE6E-E511-8681-0025905A60F4.root'
]


### do not remove the line below!
###FILELIST###

process.source = cms.Source("PoolSource",
    	fileNames = cms.untracked.vstring(fileList)
    )

# ---- define the output file -------------------------------------------
process.TFileService = cms.Service("TFileService",
			closeFileFast = cms.untracked.bool(True),
      fileName = cms.string("NeroNtuples.root"),
                )
# ------------------------QG-----------------------------------------------
process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets            = cms.InputTag("slimmedJets")    # Could be reco::PFJetCollection or pat::JetCollection (both AOD and miniAOD)               
process.QGTagger.srcVertexCollection = cms.InputTag("offlineSlimmedPrimaryVertices")
process.QGTagger.useQualityCuts = cms.bool(False)
##----------------GLOBAL TAG ---------------------------
# used by photon id and jets
#process.load("Configuration.StandardSequences.Geometry_cff") ### VETO BY HBB 74X SEQ
process.load('Configuration.StandardSequences.Services_cff')

#mc https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions#Global_Tags_for_Run2_MC_Producti
if (isData):
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
    if options.is25ns:
    	process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v2'
    if options.is50ns:
    	process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v1'
else:
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    if options.is25ns:
	    #process.GlobalTag.globaltag = '74X_mcRun2_asymptotic_realisticBS_v1'
	    #process.GlobalTag.globaltag = '74X_mcRun2_asymptotic_realisticBS_v1'
	    process.GlobalTag.globaltag = 'MCRUN2_74_V9::All'
	    #process.GlobalTag.globaltag = '74X_mcRun2_asymptotic_v2'
    if options.is50ns:
	    #process.GlobalTag.globaltag = '741_p1_mcRun2_Realistic_50ns_v0::All'
	    process.GlobalTag.globaltag = 'MCRUN2_74_V9A::All'
	    #process.GlobalTag.globaltag = '74X_mcRun2_startup_v2::All'


######## LUMI MASK
if isData and not options.isGrid : ## don't load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    ## GoldenJsn
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-251883_13TeV_PromptReco_Collisions15_JSON_v2.txt').getVLuminosityBlockRange()
    # DCS only
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/DCSOnly/json_DCSONLY_Run2015B.txt').getVLuminosityBlockRange()
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-258159_13TeV_PromptReco_Collisions15_25ns_JSON_v3.txt').getVLuminosityBlockRange()


### HBB 74X ####
process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load("Configuration.Geometry.GeometryRecoDB_cff")
process.load("RecoBTag.Configuration.RecoBTag_cff") # this loads all available b-taggers
process.load("RecoBTag.SecondaryVertex.pfBoostedDoubleSecondaryVertexAK8BJetTags_cfi")

process.pfImpactParameterTagInfosAK8.primaryVertex = cms.InputTag("offlineSlimmedPrimaryVertices")
process.pfImpactParameterTagInfosAK8.candidates = cms.InputTag("packedPFCandidates")
process.pfImpactParameterTagInfosAK8.jets = cms.InputTag("slimmedJetsAK8") 

process.load("RecoBTag.SoftLepton.softPFMuonTagInfosAK8_cfi")
process.softPFMuonsTagInfosAK8.jets=cms.InputTag("slimmedJetsAK8")
process.softPFMuonsTagInfosAK8.muons=cms.InputTag("slimmedMuons")
process.softPFMuonsTagInfosAK8.primaryVertex=cms.InputTag("offlineSlimmedPrimaryVertices")

process.load("RecoBTag.SoftLepton.softPFElectronTagInfosAK8_cfi")
process.softPFElectronsTagInfosAK8.jets=cms.InputTag("slimmedJetsAK8")
process.softPFElectronsTagInfosAK8.electrons=cms.InputTag("slimmedElectrons")
process.softPFElectronsTagInfosAK8.primaryVertex=cms.InputTag("offlineSlimmedPrimaryVertices")

process.load("RecoBTag.SecondaryVertex.pfInclusiveSecondaryVertexFinderTagInfosAK8_cfi")
process.pfInclusiveSecondaryVertexFinderTagInfosAK8.extSVCollection = cms.InputTag("slimmedSecondaryVertices")

## DEBUG
## process.output = cms.OutputModule(
## 		   "PoolOutputModule",
## 		         fileName = cms.untracked.string('output.root'),
## 			 )
## process.output_step = cms.EndPath(process.output)
process.HBB = cms.Sequence(
		process.pfImpactParameterTagInfosAK8 *
		process.pfInclusiveSecondaryVertexFinderTagInfosAK8 *
		process.softPFMuonsTagInfosAK8 *
		process.softPFElectronsTagInfosAK8 *
		process.pfBoostedDoubleSecondaryVertexAK8BJetTags 
		)
############ END HBB ####


## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')
process.load('NeroProducer.Nero.NeroMonotop_cfi')
#process.load('NeroProducer.Nero.NeroMonojet_cfi')
#process.load('NeroProducer.Nero.NeroChargedHiggs_cfi')

if options.is25ns:
	replace = {'bx' : '25ns'}
if options.is50ns:
	replace = {'bx' : '50ns'}

toProduce={}
for obj in ['ele','pho']:
  toProduce[obj]={}
  if obj=='ele': directory = 'RecoEgamma.ElectronIdentification'
  if obj=='pho': directory = 'RecoEgamma.PhotonIdentification'
  for ID in ['veto','medium','loose','tight']:
	if obj == 'pho' and ID == 'veto' : continue
	if obj == 'pho' : replace['bx'] = '50ns' ##FIXME, we have only this

	replace['id'] = ID
	cmd = 'string = process.nero.' + obj + ID.title() + 'IdMap.value()'
	exec(cmd)
	cmd = 'process.nero.'+obj + ID.title() + 'IdMap = cms.InputTag("' + string % replace+ '")'
	print 'executing replacement:',cmd
	exec(cmd)

	myid = (string%replace ).replace('-','_').split(':')[1]
	myid = re.sub('_standalone.*','',myid)
	toProduce[obj][ directory + '.Identification.' + myid + "_cff"] = 1 #remove duplicates

#-----------------------FAT JET CLUSTERING-----------------------

useCA15 = process.nero.useFatJetCA15

process.jetSequence = cms.Sequence()
if useCA15:
  from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
  from RecoJets.JetProducers.ak4PFJetsPuppi_cfi import ak4PFJetsPuppi
  from RecoJets.JetProducers.ak4GenJets_cfi import ak4GenJets
  from PhysicsTools.PatAlgos.tools.pfTools import *
  from RecoJets.JetProducers.nJettinessAdder_cfi import Njettiness

  adaptPVs(process, pvCollection=cms.InputTag('offlineSlimmedPrimaryVertices'))

  algoLabel = 'CA'
  jetAlgo = 'CambridgeAachen'

  ### puppi ###
  process.load('CommonTools.PileupAlgos.Puppi_cff')
  process.puppi.candName = cms.InputTag('packedPFCandidates')
  process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')
  process.jetSequence += process.puppi

  ### jet clustering ### 
  # set up puppi jets
  process.packedGenParticlesForJetsNoNu = cms.EDFilter(
    "CandPtrSelector", 
    src = cms.InputTag("packedGenParticles"), 
    cut = cms.string("abs(pdgId) != 12 && abs(pdgId) != 14 && abs(pdgId) != 16")
  )
  process.ca15GenJetsNoNu = ak4GenJets.clone(
    jetAlgorithm = cms.string('CambridgeAachen'),
    rParam = cms.double(1.5),
    src = cms.InputTag("packedGenParticlesForJetsNoNu")
  )
  process.ca15PFJetsPuppi = ak4PFJetsPuppi.clone(
    jetAlgorithm = cms.string('CambridgeAachen'),
    rParam = cms.double(1.5),
    src = cms.InputTag('puppi'),
    jetPtMin = cms.double(250),
    doAreaFastjet = cms.bool(True)
  )
  # set up soft drop (for subjets and mass)
  process.ca15GenJetsNoNuSD = process.ca15GenJetsNoNu.clone(
    useSoftDrop = cms.bool(True),
    R0 = cms.double(1.5),
    zcut = cms.double(0.2),
    beta = cms.double(1.0),
    writeCompound = cms.bool(True),
    jetCollInstanceName=cms.string("SubJets")
  )
  process.ca15PFJetsPuppiSD = process.ca15PFJetsPuppi.clone(
    useSoftDrop = cms.bool(True),
    R0 = cms.double(1.5),
    zcut = cms.double(0.2),
    beta = cms.double(1.0),
    writeCompound = cms.bool(True),
    jetCollInstanceName=cms.string("SubJets")
  )
  process.jetSequence += process.packedGenParticlesForJetsNoNu
  process.jetSequence += process.ca15GenJetsNoNu
  process.jetSequence += process.ca15GenJetsNoNuSD
  process.jetSequence += process.ca15PFJetsPuppi
  process.jetSequence += process.ca15PFJetsPuppiSD

  ### get out soft drop kinematics ###
  process.SoftDrop15 = cms.EDProducer('RecoJetDeltaRValueMapProducer',
      src = cms.InputTag('ca15PFJetsPuppi'),
      matched = cms.InputTag('ca15PFJetsPuppiSD'),
      distMax = cms.double(1.5),
      values = cms.vstring('mass'),
      valueLabels = cms.vstring('Mass'),
      lazyParser = cms.bool(True)
  )
  process.jetSequence += process.SoftDrop15

  ### and finally, N-subjettiness ###
  process.Njettiness15 = Njettiness.clone(
      src = cms.InputTag('ca15PFJetsPuppi'),
      R0 = cms.double(1.5),
      Njets = cms.vuint32(1,2,3,4)
  )
  process.jetSequence += process.Njettiness15

  bTagInfos = ['pfImpactParameterTagInfos',
               'pfSecondaryVertexTagInfos',
               'pfInclusiveSecondaryVertexFinderTagInfos'
              ]
  bTagDiscriminators = ['pfCombinedInclusiveSecondaryVertexV2BJetTags']

  ### add jet collections ###
  addJetCollection(
      process,
      labelName='PuppiJetsCA15',
      jetSource=cms.InputTag('ca15PFJetsPuppi'),
      algo=algoLabel,           # needed for jet flavor clustering
      rParam=1.5, # needed for jet flavor clustering
      pfCandidates = cms.InputTag('puppi'),
      pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
      svSource = cms.InputTag('slimmedSecondaryVertices'),
      muSource = cms.InputTag('slimmedMuons'),
      elSource = cms.InputTag('slimmedElectrons'),
      btagInfos = bTagInfos,
      btagDiscriminators = ['None'],
      genJetCollection = cms.InputTag('ca15GenJetsNoNu'),
      genParticles = cms.InputTag('prunedGenParticles'),
      getJetMCFlavour=False,
  )
  addJetCollection(
      process,
      labelName='SoftDropPuppiJetsCA15',
      jetSource=cms.InputTag('ca15PFJetsPuppiSD'),
      algo=algoLabel,
      rParam=1.5,
      btagInfos = ['None'],
      btagDiscriminators = ['None'],
      genJetCollection=cms.InputTag('ca15GenJetsNoNu'),
      genParticles=cms.InputTag('prunedGenParticles'),
      getJetMCFlavour=False,
  )
  addJetCollection(
      process,
      labelName='SoftDropPuppiSubJetsCA15',
      jetSource=cms.InputTag('ca15PFJetsPuppiSD','SubJets'),
      algo=algoLabel,
      rParam=1.5,
      pfCandidates = cms.InputTag('puppi'),
      pvSource = cms.InputTag('offlineSlimmedPrimaryVertices'),
      svSource = cms.InputTag('slimmedSecondaryVertices'),
      muSource = cms.InputTag('slimmedMuons'),
      elSource = cms.InputTag('slimmedElectrons'),
      btagInfos = bTagInfos,
      btagDiscriminators = bTagDiscriminators,
      genJetCollection=cms.InputTag('ca15GenJetsNoNuSD','SubJets'),
      genParticles=cms.InputTag('prunedGenParticles'),
      explicitJTA=True,
      svClustering=True,
      fatJets=cms.InputTag('ca15PFJetsPuppi'),
      groomedFatJets=cms.InputTag('ca15PFJetsPuppiSD'),
      runIVF=False,
      getJetMCFlavour=False,
  )

  process.jetSequence += process.patJetPartonMatchPuppiJetsCA15
  process.jetSequence += process.patJetGenJetMatchPuppiJetsCA15
  process.jetSequence += process.patJetsPuppiJetsCA15
  process.jetSequence += process.selectedPatJetsPuppiJetsCA15

  process.jetSequence += process.patJetPartonMatchSoftDropPuppiJetsCA15
  process.jetSequence += process.patJetGenJetMatchSoftDropPuppiJetsCA15
  process.jetSequence += process.patJetsSoftDropPuppiJetsCA15
  process.jetSequence += process.selectedPatJetsSoftDropPuppiJetsCA15

  process.jetSequence += process.patJetPartonMatchSoftDropPuppiSubJetsCA15
  process.jetSequence += process.patJetGenJetMatchSoftDropPuppiSubJetsCA15
  process.jetSequence += process.pfImpactParameterTagInfosSoftDropPuppiSubJetsCA15
  process.jetSequence += process.pfInclusiveSecondaryVertexFinderTagInfosSoftDropPuppiSubJetsCA15
  process.jetSequence += process.pfCombinedInclusiveSecondaryVertexV2BJetTagsSoftDropPuppiSubJetsCA15
  process.jetSequence += process.patJetsSoftDropPuppiSubJetsCA15
  process.jetSequence += process.selectedPatJetsSoftDropPuppiSubJetsCA15

  ### link groomed fatjets with subjets ###
  process.CA15SoftDropPuppiJetsMerged = cms.EDProducer('BoostedJetMerger',
      jetSrc=cms.InputTag('selectedPatJetsSoftDropPuppiJetsCA15'),
      subjetSrc=cms.InputTag('selectedPatJetsSoftDropPuppiSubJetsCA15'),
  )
  process.jetSequence += process.CA15SoftDropPuppiJetsMerged

  ### pack fat jets with subjets ###
  process.CA15PuppiJetsPacked = cms.EDProducer('JetSubstructurePacker',
      jetSrc = cms.InputTag('selectedPatJetsPuppiJetsCA15'),
      distMax = cms.double(1.5),
      algoTags = cms.VInputTag(),
      algoLabels = cms.vstring(),
      fixDaughters = cms.bool(False)
  )
  process.CA15PuppiJetsPacked.algoTags.append(cms.InputTag('CA15SoftDropPuppiJetsMerged'))
  process.CA15PuppiJetsPacked.algoLabels.append('SoftDrop')
  process.jetSequence += process.CA15PuppiJetsPacked

  process.patJetsPuppiJetsCA15.userData.userFloats.src += ['SoftDrop15:Mass']
  process.patJetsPuppiJetsCA15.userData.userFloats.src += ['Njettiness15:tau1','Njettiness15:tau2','Njettiness15:tau3','Njettiness15:tau4']


#-----------------------ELECTRON ID-------------------------------
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate 

dataFormat = DataFormat.MiniAOD

switchOnVIDElectronIdProducer(process, dataFormat)

### # define which IDs we want to produce. it is silly to redifine them here hard coded
### my_id_modules = [ #'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Spring15_25ns_V1_cff' ]
### 
### #add them to the VID producer
### for idmod in my_id_modules:
for idmod in toProduce['ele']:
   print "will produce", idmod
   setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
### 
### ### PHOTONS
switchOnVIDPhotonIdProducer(process, dataFormat) ### PHOTON
### pho_id_modules = [ #'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring15_50ns_V1_cff']
###         'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_PHYS14_PU20bx25_V2_cff']
### for idmod in pho_id_modules:
for idmod in toProduce['pho']:
      setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)

### ##ISO
process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")

############################### JEC #####################
#### Load from a sqlite db, if not read from the global tag
## process.load("CondCore.DBCommon.CondDBCommon_cfi")
## from CondCore.DBCommon.CondDBSetup_cfi import *
## 
## if options.isData:
## 	if options.is25ns:
## 		connectString = cms.string('sqlite:jec/Summer15_25nsV2_DATA.db')
## 		tagName = 'Summer15_25nsV2_DATA_AK4PFchs'
## 	if options.is50ns:
## 		connectString = cms.string('sqlite:jec/Summer15_50nsV5_DATA.db')
## 		tagName = 'Summer15_50nsV5_DATA_AK5PFchs'
## else:
## 	if options.is25ns:
## 		connectString = cms.string('sqlite:jec/Summer15_25nsV2_MC.db')
## 		tagName = 'Summer15_25nsV2_MC_AK5PFchs'
## 	if options.is50ns:
## 		connectString = cms.string('sqlite:jec/Summer15_50nsV5_MC.db')
## 		tagName = 'Summer15_50nsV5_MC_AK5PFchs'
## 
## process.jec = cms.ESSource("PoolDBESSource",
##       DBParameters = cms.PSet(
##         messageLevel = cms.untracked.int32(0)
##         ),
##       timetype = cms.string('runnumber'),
##       toGet = cms.VPSet(
##       cms.PSet(
##             record = cms.string('JetCorrectionsRecord'),
##             tag    = cms.string('JetCorrectorParametersCollection_%s'%tagName),
##             label  = cms.untracked.string('AK4PFchs')
##             ),
##       ## here you add as many jet types as you need
##       ## note that the tag name is specific for the particular sqlite file 
##       ), 
##       connect = connectString
##      # uncomment above tag lines and this comment to use MC JEC
## )
## ## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
## process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

################ end sqlite connection
#### BEGIN RECOMPUTE JEC ###
#from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetCorrFactorsUpdated
#from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetsUpdated
## process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
## 
## jecLevels= ['L1FastJet',  'L2Relative', 'L3Absolute']
## 
## if options.isData:
## 	print "NO L2L3 Residual Applied so far. FIXME"
## 	#jecLevels.append( 'L2L3Residuals')
## 
## process.patJetCorrFactorsReapplyJEC = process.patJetCorrFactorsUpdated.clone(
## 		  src = cms.InputTag("slimmedJets"),
## 		  levels = jecLevels,
## 		  payload = 'AK4PFchs' ) # 
## 
## process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
## process.patJetsReapplyJEC = process.patJetsUpdated.clone(
## 		  jetSource = cms.InputTag("slimmedJets"),
## 		  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
## 		  )
## 
## process.jecSequence = cms.Sequence( 
## 		process.patJetCorrFactorsReapplyJEC + 
## 		process. patJetsReapplyJEC 
## 		)
##___________________________HCAL_Noise_Filter________________________________||
process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
process.HBHENoiseFilterResultProducer.IgnoreTS4TS5ifJetInLowBVRegion=cms.bool(False) 
process.HBHENoiseFilterResultProducer.defaultDecision = cms.string("HBHENoiseFilterResultRun2Loose")

process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
		   inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResult'),
		      reverseDecision = cms.bool(False)
		      )

process.ApplyBaselineHBHEIsoNoiseFilter = cms.EDFilter('BooleanFlagFilter',
		   inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHEIsoNoiseFilterResult'),
		      reverseDecision = cms.bool(False)
		      )

process.hcalNoiseFilter = cms.Sequence(
		    process.HBHENoiseFilterResultProducer* #produces HBHE baseline bools
		    process.ApplyBaselineHBHENoiseFilter  #reject events based 
		    #process.ApplyBaselineHBHEIsoNoiseFilter*   #reject events based  < 10e-3 mistake rate 
		    )
###############################

if options.isGrid:
	process.nero.head=options.nerohead ##'git rev-parse HEAD'
	process.nero.tag=options.nerotag ## git describe --tags

if options.isParticleGun:
	process.nero.particleGun = cms.untracked.bool(True)
	## this option is for the embedding informations
	process.nero.extendEvent = cms.untracked.bool(False)

#------------------------------------------------------
process.p = cms.Path(
		process.infoProducerSequence *
		process.hcalNoiseFilter * 
                process.QGTagger *
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer * ## ISO MAP FOR PHOTONS
		process.HBB * ## HBB 74X
                process.jetSequence*
		#process.jecSequence *
                process.nero
                )

## DEBUG -- dump the event content with all the value maps ..
## process.schedule = cms.Schedule(
## 		process.p,
## 		process.output_step)
