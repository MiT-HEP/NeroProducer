import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing
import re
import os,sys

print sys.argv

process = cms.Process("nero")
cmssw_base = os.environ['CMSSW_BASE']

options = VarParsing ('analysis')
options.register('isData',
        False,
        VarParsing.multiplicity.singleton,
        VarParsing.varType.bool,
        "True if running on Data, False if running on MC")

options.register('isGrid', False, VarParsing.multiplicity.singleton,VarParsing.varType.bool,"Set it to true if running on Grid")
options.register('nerohead', "XXX", VarParsing.multiplicity.singleton,VarParsing.varType.string,"Set to the head of the repository. use check_output 'git rev-parse HEAD' in the crab py file. active only if isGrid.")
options.register("nerotag", "YYY", VarParsing.multiplicity.singleton,VarParsing.varType.string,"Set to the tag of the repository. use check_output 'git rev-parse HEAD' in the crab py file. active only if isGrid.")
options.register('isParticleGun', False, VarParsing.multiplicity.singleton,VarParsing.varType.bool,"Set it to true if MonteCarlo is ParticleGun")

options.register('isRun', 'Run2017B', VarParsing.multiplicity.singleton,VarParsing.varType.string," Run2016A/B/C   Run2017A/B/C/D/E/F")

options.parseArguments()

if '2017' in options.isRun: isYear=2017
elif '2016' in options.isRun: isYear=2016
else: raise ValueError("Unable to determine Year from isRun: %s"%options.isRun)

isData = options.isData
if options.isData:
    print "-> Loading DATA configuration"
else:
    print "-> Loading MC configuration"

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 250

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

if len(options.inputFiles) == 0:
    if isData:
        options.inputFiles = [
                '/store/user/amarini/Sync/0E555487-7241-E811-9209-002481CFC92C.root' ## SingleMuon Run2017B 31Mar2018
        ]
    else:
        options.inputFiles = [
                '/store/user/amarini/Sync/5AC9148F-9842-E811-892B-3417EBE535DA.root' ## GGH Hmm Apr2018
        ]

### do not remove the line below!
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(5000) )

process.source = cms.Source("PoolSource",
	skipEvents = cms.untracked.uint32(0),
    fileNames = cms.untracked.vstring(options.inputFiles)
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


## https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECDataMC
## Technically we don't have to correct the jets/met on the
## fly any more, because jec is in the release.

if (isData):
    # 2017 data
    if isYear==2017:
       process.GlobalTag.globaltag = '94X_dataRun2_v6' 
    elif isYear==2016:
       process.GlobalTag.globaltag = '94X_dataRun2_v10' 
else:
    ## new miniaod
    if isYear==2017:
        process.GlobalTag.globaltag = '94X_mc2017_realistic_v16' 
    elif isYear==2016:
        process.GlobalTag.globaltag = '94X_mcRun2_asymptotic_v3' 

### LOAD DATABASE
from CondCore.DBCommon.CondDBSetup_cfi import *
#from CondCore.CondDB.CondDB_cfi import *

######## LUMI MASK
if isData and not options.isGrid and False: ## dont load the lumiMaks, will be called by crab
    import FWCore.PythonUtilities.LumiList as LumiList
    process.source.lumisToProcess =None 
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification//Collisions17/13TeV/PromptReco/Cert_294927-304507_13TeV_PromptReco_Collisions17_JSON.txt').getVLuminosityBlockRange()

## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')

from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
setupEgammaPostRecoSeq(process,
                       runVID=True,
                       era='2017-Nov17ReReco')  

############################### JEC #####################
#### Load from a sqlite db, if not read from the global tag
process.load("CondCore.DBCommon.CondDBCommon_cfi")
from CondCore.DBCommon.CondDBSetup_cfi import *

if options.isData:
    if isYear == 2017:
        connectString = cms.string('sqlite:jec/Fall17_17Nov2017_V32_94X_DATA.db')
        tagName = 'Fall17_17Nov2017_V32_94X_DATA_AK4PFchs'
        tagNamePuppi = 'Fall17_17Nov2017_V32_94X_DATA_AK4PFPuppi'
    if isYear == 2016:
        connectString = cms.string('sqlite:jec/Summer16_07Aug2017All_V11_DATA.db')
        tagName = 'Summer16_07Aug2017_V11_94X_DATA_AK4PFchs'
        tagNamePuppi = 'Summer16_07Aug2017_V11_94X_DATA_AK4PFPuppi'
else:
    if isYear==2017:
        connectString = cms.string('sqlite:jec/Fall17_17Nov2017_V32_94X_MC.db')
        tagName = 'Fall17_17Nov2017_V32_94X_MC_AK4PFchs'
        tagNamePuppi = 'Fall17_17Nov2017_V32_94X_MC_AK4PFPuppi'
    if isYear==2016:
        connectString = cms.string('sqlite:jec/Summer16_07Aug2017_V11_MC.db')
        tagName = 'Summer16_07Aug2017_V11_94X_MC_AK4PFchs'
        tagNamePuppi = 'Summer16_07Aug2017_V11_94X_MC_AK4PFPuppi'

#data only, mc hard coded. Need to be fixed per Run
print "-> Setting Up JEC for AK8 using Year",isYear,"and Run",options.isRun
if isYear == 2017:
    process.nero.NeroFatJets.chsAK8JECMC = cms.string("jec/Fall17/Fall17_17Nov2017_V32")
    if options.isRun=='Run2017B':
        process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Fall17/Fall17_17Nov2017B_V32")
    elif options.isRun=='Run2017C':
        process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Fall17/Fall17_17Nov2017C_V32")
    elif options.isRun=='Run2017D' or options.isRun=='Run2017E':
        process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Fall17/Fall17_17Nov2017DE_V32")
    elif options.isRun=='Run2017F':
        process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Fall17/Fall17_17Nov2017F_V32")
    else: raise ValueError("Unable to configure AK8 JEC")
elif isYear==2016:
    process.nero.NeroFatJets.chsAK8JECMC = cms.string("jec/Summer16/Summer16_07Aug2017_V11")
    if options.isRun in ['Run2016A','Run2016B','Run2016C','Run2016D']:
        process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Summer16/Summer16_07Aug2017ABCD_V11")
    elif options.isRun in ['Run2016E', 'Run2016F']:
        process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Summer16/Summer16_07Aug2017EF_V11")
    elif options.isRun in ['Run2016G', 'Run2016H']:
        process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Summer16/Summer16_07Aug2017GH_V11")
    else: raise ValueError("Unable to configure AK8 JEC")

### 
### 
process.jec = cms.ESSource("PoolDBESSource",
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
      cms.PSet( ## AK8
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_%s'%re.sub('AK4','AK8',tagName)),
            label  = cms.untracked.string('AK8PFchs')
            ),
      cms.PSet(#puppi
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_%s'%tagNamePuppi),
            label  = cms.untracked.string('AK4PFPuppi')
            ),
      cms.PSet( ## AK8 puppi
            record = cms.string('JetCorrectionsRecord'),
            tag    = cms.string('JetCorrectorParametersCollection_%s'%re.sub('AK4','AK8',tagNamePuppi)),
            label  = cms.untracked.string('AK8PFPuppi')
            ),
      ## here you add as many jet types as you need
      ## note that the tag name is specific for the particular sqlite file 
      ), 
      connect = connectString
     # uncomment above tag lines and this comment to use MC JEC
)
## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

#------------------- JER -----------------
#print "TODO: Update JER"
toGet=[]
if options.isData:
    if isYear==2017:
        jerString = cms.string('sqlite:jer/Fall17_V3_94X_DATA.db')
        resTag= cms.string('JR_Fall17_V3_94X_DATA_PtResolution_AK4PFchs')
        phiTag= cms.string('JR_Fall17_V3_94X_DATA_PhiResolution_AK4PFchs')
        sfTag = cms.string('JR_Fall17_V3_94X_DATA_SF_AK4PFchs')
    if isYear==2016: # no _94X
        jerString = cms.string('sqlite:jer/Summer16_25nsV1_DATA.db')
        resTag= cms.string('JR_Summer16_25nsV1_DATA_PtResolution_AK4PFchs')
        phiTag= cms.string('JR_Summer16_25nsV1_DATA_PhiResolution_AK4PFchs')
        sfTag = cms.string('JR_Summer16_25nsV1_DATA_SF_AK4PFchs')
else:
    if isYear==2017:
        jerString = cms.string('sqlite:jer/Fall17_V3_94X_MC.db')
        resTag= cms.string('JR_Fall17_V3_94X_MC_PtResolution_AK4PFchs')
        phiTag= cms.string('JR_Fall17_V3_94X_MC_PhiResolution_AK4PFchs')
        sfTag = cms.string('JR_Fall17_V3_94X_MC_SF_AK4PFchs')
    if isYear==2016: # no _94X
        jerString = cms.string('sqlite:jer/Summer16_25nsV1_MC.db')
        resTag= cms.string('JR_Summer16_25nsV1_MC_PtResolution_AK4PFchs')
        phiTag= cms.string('JR_Summer16_25nsV1_MC_PhiResolution_AK4PFchs')
        sfTag = cms.string('JR_Summer16_25nsV1_MC_SF_AK4PFchs')

process.jer = cms.ESSource("PoolDBESSource",
        CondDBSetup,
        toGet = cms.VPSet( 
            # Resolution
            cms.PSet(
                record = cms.string('JetResolutionRcd'),
                tag    = resTag,
                label  = cms.untracked.string('AK4PFchs_pt')
                ),

             # Phi
             cms.PSet(
                 record = cms.string('JetResolutionRcd'),
                 tag    = phiTag,
                 label  = cms.untracked.string('AK4PFchs_phi')
                 ),        
             # Scale factors
             cms.PSet(
                 record = cms.string('JetResolutionScaleFactorRcd'),
                 tag    = sfTag,
                 label  = cms.untracked.string('AK4PFchs')
                 )
            ),
        connect = jerString
        )

process.es_prefer_jer = cms.ESPrefer('PoolDBESSource', 'jer')

################ end sqlite connection
#### RECOMPUTE JEC From GT ###
from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
 
jecLevels= ['L1FastJet',  'L2Relative', 'L3Absolute']
if options.isData:
        jecLevels =['L1FastJet',  'L2Relative', 'L3Absolute', 'L2L3Residual']
 
updateJetCollection(
    process,
    jetSource = process.nero.NeroJets.jets,
    labelName = 'UpdatedJEC',
    jetCorrections = ('AK4PFchs', cms.vstring(jecLevels), 'None')  # Do not forget 'L2L3Residual' on data!
)

updateJetCollection(
    process,
    jetSource = process.nero.NeroFatJets.chsAK8,
    labelName = 'UpdatedJECAK8',
    jetCorrections = ('AK8PFchs', cms.vstring(jecLevels), 'None')  # Do not forget 'L2L3Residual' on data!
)

from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD

runMetCorAndUncFromMiniAOD (
        process,
        isData = isData, # false for MC
        reclusterJets = True,
        pfCandColl=cms.InputTag("packedPFCandidates"),
        CHS = True,
        fixEE2017 = True,
        #fixEE2017Params = {'userawPt': True, 'PtThreshold':50.0, 'MinEtaThreshold':2.65, 'MaxEtaThreshold': 3.139} , ## default have modified names
        postfix = "ModifiedMET"
)

print "-> Ecal Bad "

baddetEcallist = cms.vuint32(
    [872439604,872422825,872420274,872423218,
     872423215,872416066,872435036,872439336,
     872420273,872436907,872420147,872439731,
     872436657,872420397,872439732,872439339,
     872439603,872422436,872439861,872437051,
     872437052,872420649,872422436,872421950,
     872437185,872422564,872421566,872421695,
     872421955,872421567,872437184,872421951,
     872421694,872437056,872437057,872437313])


process.ecalBadCalibReducedMINIAODFilter = cms.EDFilter(
    "EcalBadCalibFilter",
    EcalRecHitSource = cms.InputTag("reducedEgamma:reducedEERecHits"),
    ecalMinEt        = cms.double(50.),
    baddetEcal    = baddetEcallist, 
    taggingMode = cms.bool(True),
    debug = cms.bool(False)
    )

print "-> Updating the jets collection to run on to 'updatedPatJetsUpdatedJEC' with the new jec in the GT/or DB"
process.nero.NeroJets.jets=cms.InputTag('updatedPatJetsUpdatedJEC')
process.nero.NeroFatJets.chsAK8=cms.InputTag('updatedPatJetsUpdatedJECAK8')
process.nero.NeroMet.mets=cms.InputTag("slimmedMETsModifiedMET")
process.jecSequence = cms.Sequence( 
        #process.patJetCorrFactorsReapplyJEC + process.patJetsReapplyJEC
        process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC 
        * process.patJetCorrFactorsUpdatedJECAK8* process.updatedPatJetsUpdatedJECAK8
        * process.fullPatMetSequenceModifiedMET 
        * process.ecalBadCalibReducedMINIAODFilter
        )


#-----------------------ELECTRON ID-------------------------------

#from NeroProducer.Nero.egammavid_cfi import *
#initEGammaVID(process,options)

# ------------- Soft jet activity: Track jets ------------------------

from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets

process.chsForSATkJets = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string('charge()!=0 && pvAssociationQuality()>=5 && vertexRef().key()==0'))
process.softActivityJets = ak4PFJets.clone(src = 'chsForSATkJets', doAreaFastjet = False, jetPtMin=1) 

#------------------------------------------

# ------------------------QG-----------------------------------------------
# after jec, because need to be run on the corrected (latest) jet collection
#qgDatabaseVersion = 'cmssw8020_v2'
#
#process.QGPoolDBESSource = cms.ESSource("PoolDBESSource",
#      CondDBSetup,
#      toGet = cms.VPSet(
#        cms.PSet(
#            record = cms.string('QGLikelihoodRcd'),
#            tag    = cms.string('QGLikelihoodObject_'+qgDatabaseVersion+'_AK4PFchs'),
#            label  = cms.untracked.string('QGL_AK4PFchs')
#        ),
#      ),
#      connect = cms.string('sqlite:qg/QGL_'+qgDatabaseVersion+'.db')
#)
#process.es_prefer_qg = cms.ESPrefer('PoolDBESSource','QGPoolDBESSource')
#
#process.load('RecoJets.JetProducers.QGTagger_cfi')
#process.QGTagger.srcJets             = process.nero.NeroJets.jets   # Could be reco::PFJetCollection or pat::JetCollection (both AOD and miniAOD)               
#process.QGTagger.srcVertexCollection = process.nero.NeroVertex.vertices
#process.QGTagger.useQualityCuts = cms.bool(False)

#IOV:QGLikelihoodObject_v1_AK4 QGL_AK4chs_94X.db
process.QGPoolDBESSource = cms.ESSource("PoolDBESSource",
      CondDBSetup,
      toGet = cms.VPSet(
        cms.PSet(
            record = cms.string('QGLikelihoodRcd'),
            tag    = cms.string('QGLikelihoodObject_v1_AK4'),
            label  = cms.untracked.string('QGL_AK4PFchs')
        ),
      ),
      connect = cms.string('sqlite:qg/QGL_AK4chs_94X.db')
)
process.es_prefer_qg = cms.ESPrefer('PoolDBESSource','QGPoolDBESSource')

process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets             = process.nero.NeroJets.jets   # Could be reco::PFJetCollection or pat::JetCollection (both AOD and miniAOD)               
process.QGTagger.srcVertexCollection = process.nero.NeroVertex.vertices
process.QGTagger.useQualityCuts = cms.bool(False)

# ----------------------- GenHFHadronMatcher -----------------

process.load("PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff")

from PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi import selectedHadronsAndPartons
process.selectedHadronsAndPartons = selectedHadronsAndPartons.clone(
            particles = process.nero.NeroMonteCarlo.prunedgen
            )
from PhysicsTools.JetMCAlgos.AK4PFJetsMCFlavourInfos_cfi import ak4JetFlavourInfos
process.genJetFlavourInfos = ak4JetFlavourInfos.clone(
            jets = process.nero.NeroMonteCarlo.genjets
            )

from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenBHadron
process.matchGenBHadron = matchGenBHadron.clone(
            genParticles = process.nero.NeroMonteCarlo.prunedgen,
            jetFlavourInfos = "genJetFlavourInfos"
            )
from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenCHadron
process.matchGenCHadron = matchGenCHadron.clone(
            genParticles = process.nero.NeroMonteCarlo.prunedgen,
            jetFlavourInfos = "genJetFlavourInfos"
            )
process.load("TopQuarkAnalysis.TopTools.GenTtbarCategorizer_cfi")
process.categorizeGenTtbar.genJets = process.nero.NeroMonteCarlo.genjets

process.ttbarcat = cms.Sequence()
if not isData:
    process.ttbarcat = cms.Sequence( 
                    process.selectedHadronsAndPartons * process.genJetFlavourInfos * process.matchGenBHadron * process.matchGenCHadron* ## gen HF flavour matching
                    process.categorizeGenTtbar  ## return already a categorization id for tt
                    )
###############################################################

if options.isGrid:
	process.nero.head=options.nerohead ##'git rev-parse HEAD'
	process.nero.tag=options.nerotag ## git describe --tags
else:
    # runs git
    from NeroProducer.Nero.NeroTag_cfi import neroTag
    process.nero.head = neroTag.head
    process.nero.tag = neroTag.tag

if options.isParticleGun:
	process.nero.particleGun = cms.untracked.bool(True)
	## this option is for the embedding informations
	process.nero.extendEvent = cms.untracked.bool(False)

process.load('NeroProducer.Skim.QGVariablesSequence_cff')
process.QGVariables.srcJets = process.nero.NeroJets.jets
process.QGVariables.srcGenJets = cms.InputTag("slimmedGenJets")
process.QGVariables.isData = cms.bool(isData)

### Groomed variables
process.load('NeroProducer.Skim.NjettinesGroomed_cff')
process.NjettinessGroomed.srcJets = process.nero.NeroFatJets.chsAK8

### BRegression
process.load('NeroProducer.Skim.bRegressionProducer_cfi')
process.bRegressionProducer.JetTag = process.nero.NeroJets.jets

##DEBUG
##print "Process=",process, process.__dict__.keys()
#------------------------------------------------------
process.p = cms.Path(
                process.infoProducerSequence *
                process.egammaPostRecoSeq *  #-> should be everything from EGamma

                #process.electronIDValueMapProducer *  
                #process.electronMVAValueMapProducer * # needs to be produced on to of new slimmed electrons.
                process.jecSequence *
                process.QGTagger    * ## after jec, because it will produce the new jet collection
                process.ttbarcat *
                process.QGVariablesSequence*
                process.NjettinessGroomed*
                process.chsForSATkJets * process.softActivityJets * ## track jtes
                process.bRegressionProducer*
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
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
