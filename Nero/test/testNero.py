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
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

fileList = [
        #'file:/tmp/amarini/step3_0.root'
        #'/store/data/Run2015D/SinglePhoton/MINIAOD/PromptReco-v3/000/256/630/00000/BE4748B0-295F-E511-A271-02163E014402.root',
        #'/store/mc/RunIISpring15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/74X_mcRun2_asymptotic_v2-v1/50000/E4F89698-DE6E-E511-8681-0025905A60F4.root'
        #'/store/data/Run2015D/SingleMuon/MINIAOD/PromptReco-v4/000/260/373/00000/50EB0717-B081-E511-B4E2-02163E014169.root', 
        #'/store/data/Run2015D/SingleMuon/MINIAOD/PromptReco-v4/000/260/373/00000/58DC7348-C481-E511-8232-02163E011AC9.root',
        #'/store/data/Run2015D/SingleMuon/MINIAOD/PromptReco-v4/000/260/373/00000/5E276556-C481-E511-B025-02163E014414.root',
        #'/store/data/Run2015D/SingleMuon/MINIAOD/PromptReco-v4/000/260/373/00000/903B69BB-EC81-E511-B029-02163E0142F1.root',
        #'/store/data/Run2015D/SingleMuon/MINIAOD/PromptReco-v4/000/260/373/00000/AE673A5A-C481-E511-AC81-02163E0136DB.root',
        #'/store/data/Run2015D/SingleMuon/MINIAOD/PromptReco-v4/000/260/373/00000/BA5C1056-C481-E511-A636-02163E01182F.root',
        #'/store/data/Run2015D/SingleMuon/MINIAOD/PromptReco-v4/000/260/373/00000/CCCE9F28-D481-E511-970E-02163E0141B5.root',
        #'/store/mc/RunIIFall15DR76/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/25nsFlat10to25TSG_HCALDebug_76X_mcRun2_asymptotic_v11-v1/40000/FEEBF471-F987-E511-BC13-002590D0B002.root',
        #'/store/mc/RunIIFall15DR76/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/25nsFlat10to25TSG_HCALDebug_76X_mcRun2_asymptotic_v11-v1/40000/D0A3314C-1788-E511-9A86-002590D0B098.root'
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA0A72D5-C7B8-E511-8B1D-901B0E6459E0.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA1D1BF6-C7B8-E511-AD66-901B0E6459E0.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA7D82F5-D3B8-E511-AEB7-901B0E542804.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA7EFFBF-D1B8-E511-AF4A-009C02AABB60.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA8C6AD9-79BA-E511-B267-0025905B85A2.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FAB6B37C-77BA-E511-A89F-0CC47A4D76B6.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FAF5D5D7-79BA-E511-BB76-0025905B8562.root'
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
process.load("Configuration.Geometry.GeometryIdeal_cff") 
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")

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
        process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_v11'
        #process.GlobalTag.globaltag = '74X_mcRun2_asymptotic_v2'
    if options.is50ns:
        process.GlobalTag.globaltag = 'MCRUN2_74_V9A::All' ## FIXME


######## LUMI MASK
if isData and not options.isGrid : ## don't load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    ## SILVER
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt').getVLuminosityBlockRange()


## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')
#process.load('NeroProducer.Nero.NeroMonotop_cfi')
#process.load('NeroProducer.Nero.NeroMonojet_cfi')
#process.load('NeroProducer.Nero.NeroChargedHiggs_cfi')



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

## in case of conflict, use jec instead of GlobalTag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')
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
from NeroProducer.Nero.egammavid_cfi import *

initEGammaVID(process,options)

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
        process.puppiSequence * ## does puppi, puppi met, type1 corrections
        process.jetSequence *
        #process.jecSequence *
        process.nero
        )

## DEBUG -- dump the event content with all the value maps ..
## process.schedule = cms.Schedule(
## 		process.p,
## 		process.output_step)

# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
