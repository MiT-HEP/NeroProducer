import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import re
import os

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
process.MessageLogger.cerr.FwkReport.reportEvery = 10

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

fileList = []

if isData:
        fileList = [
                '/store/data/Run2015D/MET/MINIAOD/05Oct2015-v1/30000/2A4C3292-B46F-E511-BAD2-0025905A60C6.root'
                ]
else:
        fileList = [
                '/store/mc/RunIISpring15MiniAODv2/TTbarDMJets_pseudoscalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/74X_mcRun2_asymptotic_v2-v1/10000/1486FE25-A16D-E511-93F2-001EC9ADE672.root'
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
    ## SILVER
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt').getVLuminosityBlockRange()


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
#process.load('NeroProducer.Nero.NeroMonotop_cfi')
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


#----------------------PUPPI, MET, & CORRECTIONS-------------------
process.puppiSequence = cms.Sequence()
### puppi ###
process.load('CommonTools.PileupAlgos.Puppi_cff')
process.puppi.candName = cms.InputTag('packedPFCandidates')
process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')
process.pfCandNoLep = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("abs(pdgId) != 13 && abs(pdgId) != 11 && abs(pdgId) != 15"))
process.pfCandLep   = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("abs(pdgId) == 13 || abs(pdgId) == 11 || abs(pdgId) == 15"))
process.puppinolep = process.puppi.clone()
process.puppinolep.candName = 'pfCandNoLep'
process.puppiSequence += process.puppi
process.puppiSequence += process.pfCandNoLep
process.puppiSequence += process.pfCandLep
process.puppiSequence += process.puppinolep

### MET ###
process.load('RecoMET.METProducers.PFMET_cfi')
process.puppiForMET = cms.EDProducer("CandViewMerger",src = cms.VInputTag( 'puppinolep','pfCandLep'))
process.puppiSequence += process.puppiForMET
process.pfMETPuppi = process.pfMet.clone();
process.pfMETPuppi.src = cms.InputTag('puppiForMET')
process.pfMETPuppi.calculateSignificance = False
process.puppiSequence += process.pfMETPuppi

### set up JEC ###
cmssw_base = os.environ['CMSSW_BASE']
if options.isData:
   connectString = cms.string('sqlite:jec/Summer15_25nsV6_DATA.db')
   tagName = 'Summer15_25nsV6_DATA_AK4PFPuppi'
else:
   connectString = cms.string('sqlite:jec/Summer15_25nsV6_MC.db')
   tagName = 'Summer15_25nsV6_MC_AK4PFPuppi'
process.jec = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(0)
      ),
      timetype = cms.string('runnumber'),
      toGet = cms.VPSet(
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_%s'%tagName),
            label  = cms.untracked.string('AK4PFPuppi')
      ),
     ), 
     connect = connectString
)
process.load('JetMETCorrections.Configuration.JetCorrectorsAllAlgos_cff')
puppilabel='PFPuppi'
process.ak4PuppiL1FastjetCorrector  = process.ak4PFCHSL1FastjetCorrector.clone (algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiL2RelativeCorrector = process.ak4PFCHSL2RelativeCorrector.clone(algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiL3AbsoluteCorrector = process.ak4PFCHSL3AbsoluteCorrector.clone(algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiResidualCorrector   = process.ak4PFCHSResidualCorrector.clone  (algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiL1FastL2L3Corrector = process.ak4PFL1FastL2L3Corrector.clone(
        correctors = cms.VInputTag("ak4PuppiL1FastjetCorrector", "ak4PuppiL2RelativeCorrector", "ak4PuppiL3AbsoluteCorrector")
)
process.ak4PuppiL1FastL2L3ResidualCorrector = process.ak4PFL1FastL2L3Corrector.clone(
        correctors = cms.VInputTag("ak4PuppiL1FastjetCorrector", "ak4PuppiL2RelativeCorrector", "ak4PuppiL3AbsoluteCorrector",'ak4PuppiResidualCorrector')
)
process.ak4PuppiL1FastL2L3Chain = cms.Sequence(
        process.ak4PuppiL1FastjetCorrector * process.ak4PuppiL2RelativeCorrector * process.ak4PuppiL3AbsoluteCorrector * process.ak4PuppiL1FastL2L3Corrector
)
process.ak4PuppiL1FastL2L3ResidualChain = cms.Sequence(
        process.ak4PuppiL1FastjetCorrector * process.ak4PuppiL2RelativeCorrector * process.ak4PuppiL3AbsoluteCorrector * process.ak4PuppiResidualCorrector * process.ak4PuppiL1FastL2L3ResidualCorrector
)
if isData:
  process.puppiSequence += process.ak4PuppiL1FastL2L3ResidualChain
else:
  process.puppiSequence += process.ak4PuppiL1FastL2L3Chain

### MET corrections ###
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
process.AK4PFJetsPuppi = ak4PFJets.clone(
        src          = cms.InputTag('puppinolep'),
        jetAlgorithm = cms.string("AntiKt"),
        rParam       = cms.double(0.4),
        jetPtMin     = cms.double(20)
)

process.puppiSequence += process.AK4PFJetsPuppi
process.puppiMETCorr = cms.EDProducer("PFJetMETcorrInputProducer",
    src = cms.InputTag('AK4PFJetsPuppi'),
    offsetCorrLabel = cms.InputTag("ak4PuppiL1FastjetCorrector"),
    jetCorrLabel = cms.InputTag("ak4PuppiL1FastL2L3Corrector"),
    jetCorrLabelRes = cms.InputTag("ak4PuppiL1FastL2L3ResidualCorrector"),
    jetCorrEtaMax = cms.double(9.9),
    type1JetPtThreshold = cms.double(15.0),
    skipEM = cms.bool(True),
    skipEMfractionThreshold = cms.double(0.90),
    skipMuons = cms.bool(True),
    skipMuonSelection = cms.string("isGlobalMuon | isStandAloneMuon")
)
if isData:
  process.puppiMETCorr.jetCorrLabel = cms.InputTag("ak4PuppiL1FastL2L3ResidualCorrector")
process.puppiSequence += process.puppiMETCorr

process.type1PuppiMET = cms.EDProducer("CorrectedPFMETProducer",
    src = cms.InputTag('pfMETPuppi'),
    applyType0Corrections = cms.bool(False),
    applyType1Corrections = cms.bool(True),
    srcCorrections = cms.VInputTag(
      cms.InputTag('puppiMETCorr','type1')
    ),
    applyType2Corrections = cms.bool(False)
)
process.puppiSequence += process.type1PuppiMET

#------------------------FATJETS--------------------------------

from NeroProducer.Nero.makeFatJets_cff import *
fatjetInitSequence = initFatJets(process,isData)
ak8PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='AK',jetRadius=0.8)
ca15CHSSequence = makeFatJets(process,isData=isData,pfCandidates='pfCHS',algoLabel='CA',jetRadius=1.5)
ca15PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='CA',jetRadius=1.5)
process.jetSequence = cms.Sequence(fatjetInitSequence*
                                     ak8PuppiSequence*
                                     ca15CHSSequence*
                                     ca15PuppiSequence
                                    )

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
process.load("CondCore.DBCommon.CondDBCommon_cfi")
from CondCore.DBCommon.CondDBSetup_cfi import *

if options.isData:
	if options.is25ns:
		connectString = cms.string('sqlite:jec/Summer15_25nsV6_DATA.db')
		tagName = 'Summer15_25nsV6_DATA_AK4PFchs'
	if options.is50ns:
		connectString = cms.string('sqlite:jec/Summer15_50nsV5_DATA.db')
		tagName = 'Summer15_50nsV5_DATA_AK5PFchs'
else:
	if options.is25ns:
		connectString = cms.string('sqlite:jec/Summer15_25nsV6_MC.db')
		tagName = 'Summer15_25nsV6_MC_AK4PFchs'
	if options.is50ns:
		connectString = cms.string('sqlite:jec/Summer15_50nsV5_MC.db')
		tagName = 'Summer15_50nsV5_MC_AK5PFchs'

process.myJec = cms.ESSource("PoolDBESSource",
      DBParameters = cms.PSet(
        messageLevel = cms.untracked.int32(0)
        ),
      timetype = cms.string('runnumber'),
      toGet = cms.VPSet(
      cms.PSet(
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_%s'%tagName),
            label  = cms.untracked.string('AK4PFchs')
            ),
      ## here you add as many jet types as you need
      ## note that the tag name is specific for the particular sqlite file 
      ), 
      connect = connectString
     # uncomment above tag lines and this comment to use MC JEC
)
## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','myJec')

################# end sqlite connection
##### BEGIN RECOMPUTE JEC ###
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetCorrFactorsUpdated
from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetsUpdated
process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")

jecLevels= ['L1FastJet',  'L2Relative', 'L3Absolute']

if options.isData:
        print 'No L2L3Residuals. Fix some time'
	jecLevels.append( 'L2L3Residual')

process.patJetCorrFactorsReapplyJEC = process.patJetCorrFactorsUpdated.clone(
		  src = cms.InputTag("slimmedJets"),
		  levels = jecLevels,
		  payload = 'AK4PFchs' ) # 

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetsReapplyJEC = process.patJetsUpdated.clone(
		  jetSource = cms.InputTag("slimmedJets"),
		  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
		  )

process.myJecSequence = cms.Sequence( 
		process.patJetCorrFactorsReapplyJEC + 
		process. patJetsReapplyJEC 
		)
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
#		process.hcalNoiseFilter * 
                process.QGTagger *
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.HBB * ## HBB 74X
                process.puppiSequence * ## does puppi, puppi met, type1 corrections
                process.jetSequence *
                process.myJecSequence *
                process.nero
                )

## DEBUG -- dump the event content with all the value maps ..
## process.schedule = cms.Schedule(
## 		process.p,
## 		process.output_step)
