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

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

fileList = [
        #'/store/data/Run2015D/MET/MINIAOD/16Dec2015-v1/50000/00EA1DB2-90AA-E511-AEEE-0025905C2CE6.root'
        #'/store/mc/RunIIFall15MiniAODv2/TTbarDMJets_pseudoscalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/20000/0A4E9031-7CB9-E511-8ABE-02163E00EA21.root',
        #'/store/mc/RunIIFall15MiniAODv2/TTbarDMJets_pseudoscalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/20000/2A4D5764-7CB9-E511-A4E5-02163E017828.root'
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA0A72D5-C7B8-E511-8B1D-901B0E6459E0.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA1D1BF6-C7B8-E511-AD66-901B0E6459E0.root',
        '/store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA7D82F5-D3B8-E511-AEB7-901B0E542804.root',

        ]

### do not remove the line below!
###FILELIST###

process.source = cms.Source("PoolSource",
	skipEvents = cms.untracked.uint32(0),
    	fileNames = cms.untracked.vstring(fileList)
        )

# ---- define the output file -------------------------------------------
process.TFileService = cms.Service("TFileService",
        closeFileFast = cms.untracked.bool(True),
        fileName = cms.string("NeroNtuples.root"),
        )

##----------------GLOBAL TAG ---------------------------
# used by photon id and jets
process.load("Configuration.Geometry.GeometryIdeal_cff") 
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")

#mc https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions#Global_Tags_for_Run2_MC_Producti
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
if (isData):
    if options.is25ns:
        process.GlobalTag.globaltag = '76X_dataRun2_16Dec2015_v0'
    if options.is50ns:
        process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v1'
        print "FIX GLOBAL TAG"
else:
    #process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    if options.is25ns:
        process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_RunIIFall15DR76_v1'
    if options.is50ns:
        process.GlobalTag.globaltag = 'MCRUN2_74_V9A::All' ## FIXME
        print "FIX GLOBAL TAG"

### LOAD DATABASE
from CondCore.DBCommon.CondDBSetup_cfi import *

######## LUMI MASK
if isData and not options.isGrid : ## dont load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    ## SILVER
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt').getVLuminosityBlockRange()
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver_v2.txt').getVLuminosityBlockRange()


## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')
#process.load('NeroProducer.Nero.NeroMonotop_cfi')
#process.load('NeroProducer.Nero.NeroMonojet_cfi')
#process.load('NeroProducer.Nero.NeroChargedHiggs_cfi')



# ------------------- JER -----------------
if options.isData:
    jerString = cms.string('sqlite:jer/Summer15_25nsV6_DATA.db')
else:
    jerString = cms.string('sqlite:jer/Summer15_25nsV6_MC.db')

process.jer = cms.ESSource("PoolDBESSource",
        CondDBSetup,
        toGet = cms.VPSet(
            # Resolution
            cms.PSet(
                record = cms.string('JetResolutionRcd'),
                tag    = cms.string('JR_Summer15_25nsV6_MC_PtResolution_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs_pt')
                ),

            # Scale factors
            cms.PSet(
                record = cms.string('JetResolutionScaleFactorRcd'),
                tag    = cms.string('JR_Summer15_25nsV6_MC_SF_AK4PFchs'),
                label  = cms.untracked.string('AK4PFchs')
                ),
            ),
        connect = jerString
        )

process.es_prefer_jer = cms.ESPrefer('PoolDBESSource', 'jer')

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

### ### set up JEC ###
### if options.isData:
###     connectString = cms.string('sqlite:jec/Summer15_25nsV6_DATA.db')
###     tagName = 'Summer15_25nsV6_DATA_AK4PFPuppi'
### else:
###     connectString = cms.string('sqlite:jec/Summer15_25nsV6_MC.db')
###     tagName = 'Summer15_25nsV6_MC_AK4PFPuppi'
### 
### process.jec = cms.ESSource("PoolDBESSource",
###         DBParameters = cms.PSet(
###             messageLevel = cms.untracked.int32(0)
###             ),
###         timetype = cms.string('runnumber'),
###         toGet = cms.VPSet(
###             cms.PSet(
###                 record = cms.string('JetCorrectionsRecord'),
###                 tag    = cms.string('JetCorrectorParametersCollection_%s'%tagName),
###                 label  = cms.untracked.string('AK4PFPuppi')
###                 ),
###             ), 
###         connect = connectString
###         )
### process.load('JetMETCorrections.Configuration.JetCorrectorsAllAlgos_cff')
### puppilabel='PFPuppi'
### process.ak4PuppiL1FastjetCorrector  = process.ak4PFCHSL1FastjetCorrector.clone (algorithm   = cms.string('AK4'+puppilabel))
### process.ak4PuppiL2RelativeCorrector = process.ak4PFCHSL2RelativeCorrector.clone(algorithm   = cms.string('AK4'+puppilabel))
### process.ak4PuppiL3AbsoluteCorrector = process.ak4PFCHSL3AbsoluteCorrector.clone(algorithm   = cms.string('AK4'+puppilabel))
### process.ak4PuppiResidualCorrector   = process.ak4PFCHSResidualCorrector.clone  (algorithm   = cms.string('AK4'+puppilabel))
### process.ak4PuppiL1FastL2L3Corrector = process.ak4PFL1FastL2L3Corrector.clone(
###         correctors = cms.VInputTag("ak4PuppiL1FastjetCorrector", "ak4PuppiL2RelativeCorrector", "ak4PuppiL3AbsoluteCorrector")
###         )
### process.ak4PuppiL1FastL2L3ResidualCorrector = process.ak4PFL1FastL2L3Corrector.clone(
###         correctors = cms.VInputTag("ak4PuppiL1FastjetCorrector", "ak4PuppiL2RelativeCorrector", "ak4PuppiL3AbsoluteCorrector",'ak4PuppiResidualCorrector')
###         )
### process.ak4PuppiL1FastL2L3Chain = cms.Sequence(
###         process.ak4PuppiL1FastjetCorrector * process.ak4PuppiL2RelativeCorrector * process.ak4PuppiL3AbsoluteCorrector * process.ak4PuppiL1FastL2L3Corrector
###         )
### process.ak4PuppiL1FastL2L3ResidualChain = cms.Sequence(
###         process.ak4PuppiL1FastjetCorrector * process.ak4PuppiL2RelativeCorrector * process.ak4PuppiL3AbsoluteCorrector * process.ak4PuppiResidualCorrector * process.ak4PuppiL1FastL2L3ResidualCorrector
###         )
### if isData:
###     process.puppiSequence += process.ak4PuppiL1FastL2L3ResidualChain
### else:
###     process.puppiSequence += process.ak4PuppiL1FastL2L3Chain
### 
### ## in case of conflict, use jec instead of GlobalTag
### process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')
### MET corrections ###
### from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
### process.AK4PFJetsPuppi = ak4PFJets.clone(
###         src          = cms.InputTag('puppinolep'),
###         jetAlgorithm = cms.string("AntiKt"),
###         rParam       = cms.double(0.4),
###         jetPtMin     = cms.double(20)
###         )
### 
### process.puppiSequence += process.AK4PFJetsPuppi
### process.puppiMETCorr = cms.EDProducer("PFJetMETcorrInputProducer",
###         src = cms.InputTag('AK4PFJetsPuppi'),
###         offsetCorrLabel = cms.InputTag("ak4PuppiL1FastjetCorrector"),
###         jetCorrLabel = cms.InputTag("ak4PuppiL1FastL2L3Corrector"),
###         jetCorrLabelRes = cms.InputTag("ak4PuppiL1FastL2L3ResidualCorrector"),
###         jetCorrEtaMax = cms.double(9.9),
###         type1JetPtThreshold = cms.double(15.0),
###         skipEM = cms.bool(True),
###         skipEMfractionThreshold = cms.double(0.90),
###         skipMuons = cms.bool(True),
###         skipMuonSelection = cms.string("isGlobalMuon | isStandAloneMuon")
###         )
### if isData:
###     process.puppiMETCorr.jetCorrLabel = cms.InputTag("ak4PuppiL1FastL2L3ResidualCorrector")
### process.puppiSequence += process.puppiMETCorr
### 
### process.type1PuppiMET = cms.EDProducer("CorrectedPFMETProducer",
###         src = cms.InputTag('pfMETPuppi'),
###         applyType0Corrections = cms.bool(False),
###         applyType1Corrections = cms.bool(True),
###         srcCorrections = cms.VInputTag(
###             cms.InputTag('puppiMETCorr','type1')
###             ),
###         applyType2Corrections = cms.bool(False)
###         )
### process.puppiSequence += process.type1PuppiMET

#------------------------FATJETS--------------------------------

### from NeroProducer.Nero.makeFatJets_cff import *
### fatjetInitSequence = initFatJets(process,isData)
### ak8PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='AK',jetRadius=0.8)
### ca15CHSSequence = makeFatJets(process,isData=isData,pfCandidates='pfCHS',algoLabel='CA',jetRadius=1.5)
### ca15PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='CA',jetRadius=1.5)
### process.jetSequence = cms.Sequence(fatjetInitSequence*
###                     ak8PuppiSequence*
###                     ca15CHSSequence*
###                     ca15PuppiSequence
###                     )
### 
### process.reclusterSequence = cms.Sequence()
### if process.nero.doReclustering: ## use python to figure it out
###     process.reclusterSequence += process.puppiSequence 
###     process.reclusterSequence += process.jetSequence 
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
#### RECOMPUTE JEC From GT ###
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection

jecLevels= ['L1FastJet',  'L2Relative', 'L3Absolute']
if options.isData:
	jecLevels.append( 'L2L3Residuals')

updateJetCollection(
   process,
   #jetSource = cms.InputTag('slimmedJets'),
   jetSource = process.nero.jets,
   labelName = 'UpdatedJEC',
   jetCorrections = ('AK4PFchs', cms.vstring(jecLevels), 'None')  # Do not forget 'L2L3Residual' on data!
)
print "-> Updating the jets collection to run on to 'updatedPatJetsUpdatedJEC' with the new jec in the GT"
process.nero.jets=cms.InputTag('updatedPatJetsUpdatedJEC')
process.jecSequence = cms.Sequence( process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC )
#process.jecSequence = cms.Sequence( )

# ------------------------QG-----------------------------------------------
# after jec, because need to be run on the corrected (latest) jet collection
qgDatabaseVersion = '76X'

# to use the database, see https://twiki.cern.ch/twiki/bin/view/CMS/QuarkGluonLikelihood
#connect = cms.string('frontier://FrontierProd/CMS_COND_PAT_000'),
#for type in ['AK4PFchs','AK4PFchs_antib']:
#  QGPoolDBESSource.toGet.extend(cms.VPSet(cms.PSet(
#    record = cms.string('QGLikelihoodRcd'),
#    tag    = cms.string('QGLikelihoodObject_'+qgDatabaseVersion+'_'+type),
#    label  = cms.untracked.string('QGL_'+type)
#  )))

process.QGPoolDBESSource = cms.ESSource("PoolDBESSource",
      CondDBSetup,
      toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('QGLikelihoodRcd'),
            tag    = cms.string('QGLikelihoodObject_'+qgDatabaseVersion+'_AK4PFchs'),
            label  = cms.untracked.string('QGL_AK4PFchs')
        ),
      ),
      connect = cms.string('sqlite:qg/QGL_'+qgDatabaseVersion+'.db')
)
process.es_prefer_qg = cms.ESPrefer('PoolDBESSource','QGPoolDBESSource')

process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets             = process.nero.jets   # Could be reco::PFJetCollection or pat::JetCollection (both AOD and miniAOD)               
process.QGTagger.srcVertexCollection = process.nero.vertices
process.QGTagger.useQualityCuts = cms.bool(False)

###############################

if options.isGrid:
	process.nero.head=options.nerohead ##'git rev-parse HEAD'
	process.nero.tag=options.nerotag ## git describe --tags

if options.isParticleGun:
	process.nero.particleGun = cms.untracked.bool(True)
	## this option is for the embedding informations
	process.nero.extendEvent = cms.untracked.bool(False)

process.load('NeroProducer.Skim.MonoJetFilterSequence_cff')

##DEBUG
##print "Process=",process, process.__dict__.keys()
#------------------------------------------------------
process.p = cms.Path(
                process.infoProducerSequence *
                #process.MonoJetFilter *
                #process.hcalNoiseFilter * 
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer *  ## ISO MAP FOR PHOTONS
                #process.puppiSequence * ## does puppi, puppi met, type1 corrections
                #process.jetSequence *
                #process.reclusterSequence * ##  includes puppi and jets if recluster options is on
                process.jecSequence  *
                process.QGTagger * ## after jec, because it will produce the new jet collection
                process.nero
                )

## DEBUG -- dump the event content with all the value maps ..
## process.output = cms.OutputModule(
##                 "PoolOutputModule",
##                       fileName = cms.untracked.string('output.root'),
##                       )
## process.output_step = cms.EndPath(process.output)
## 
## process.schedule = cms.Schedule(
## 		process.p,
## 		process.output_step)

# Local Variables:
# mode:c++
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
