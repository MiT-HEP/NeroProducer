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

options.register('isRun', "Run2018A", VarParsing.multiplicity.singleton,VarParsing.varType.string,"Run2018A / B / C / D (for data)")

options.parseArguments()

isYear=2018
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
                '/store/data/Run2018A/SingleMuon/MINIAOD/17Sep2018-v2/00000/033CDF0C-6890-0E49-9C83-C769AD220FD8.root'
        ]
    else:
        options.inputFiles = [
                '/store/mc/RunIIAutumn18MiniAOD/DYJetsToLL_M-105To160_TuneCP5_PSweights_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/100000/48A13F85-97BC-8348-A555-9AC79DF6628B.root'
        ]

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

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
#process.load("CondCore.DBCommon.CondDBCommon_cfi")


## https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECDataMC
## Technically we don't have to correct the jets/met on the
## fly any more, because jec is in the release.

if (isData):
    # 2017 data
    if options.isRun in ['Run2018A', 'Run2018B','Run2018C']:
        process.GlobalTag.globaltag = '102X_dataRun2_Sep2018Rereco_v1' 
    elif options.isRun == 'Run2018D':
        process.GlobalTag.globaltag = '102X_dataRun2_Prompt_v11' 
else:
    ## new miniaod
    process.GlobalTag.globaltag = '102X_upgrade2018_realistic_v12' 

### LOAD DATABASE
from CondCore.DBCommon.CondDBSetup_cfi import *
#from CondCore.CondDB.CondDB_cfi import *

######## LUMI MASK
if isData and not options.isGrid and False: ## dont load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    print "-> UPDATE THE LUMI LIST"
    process.source.lumisToProcess =None 
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification//Collisions17/13TeV/PromptReco/Cert_294927-304507_13TeV_PromptReco_Collisions17_JSON.txt').getVLuminosityBlockRange()
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt').getVLuminosityBlockRange()

## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')

### ##ISO
#process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
#process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")

from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
setupEgammaPostRecoSeq(process,
                       runVID=True,
                       era='2018-Prompt')  

############################### JEC #####################
#### Load from a sqlite db, if not read from the global tag
process.load("CondCore.DBCommon.CondDBCommon_cfi")
from CondCore.DBCommon.CondDBSetup_cfi import *

if options.isData:
    connectString = cms.string('sqlite:jec/Autumn18_RunABCD_V16_DATA.db')
    tagName = 'Autumn18_RunABCD_V16_DATA_AK4PFchs'
    tagNamePuppi = 'Autumn18_RunABCD_V16_DATA_AK4PFPuppi'
else:
    connectString = cms.string('sqlite:jec/Autumn18_V16_MC.db')
    tagName = 'Autumn18_V16_MC_AK4PFchs'
    tagNamePuppi = 'Autumn18_V16_MC_AK4PFPuppi'

#print "-> FIX JEC FOR AK8"
print "-> Setting Up JEC for AK8 using Run",options.isRun
process.nero.NeroFatJets.chsAK8JECMC = cms.string("jec/Autumn18/Autumn18_V8")

if options.isRun=="Run2018A":
    process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Autumn18/Autumn18_RunA_V8")
elif options.isRun=="Run2018B":
    process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Autumn18/Autumn18_RunB_V8")
elif options.isRun=="Run2018C":
    process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Autumn18/Autumn18_RunC_V8")
elif options.isRun=="Run2018D":
    process.nero.NeroFatJets.chsAK8JEC = cms.string("jec/Autumn18/Autumn18_RunD_V8")
else: raise ValueError("Unable to configure AK8 JEC")

#### 
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
print "TODO: Update JER"
toGet=[]
if options.isData:
    jerString = cms.string('sqlite:jer/Fall17_V3_102X_DATA.db')
    resTag= cms.string('JR_Fall17_V3_102X_DATA_PtResolution_AK4PFchs')
    phiTag= cms.string('JR_Fall17_V3_102X_DATA_PhiResolution_AK4PFchs')
    sfTag = cms.string('JR_Fall17_V3_102X_DATA_SF_AK4PFchs')
else:
    jerString = cms.string('sqlite:jer/Fall17_V3_102X_MC.db')
    resTag= cms.string('JR_Fall17_V3_102X_MC_PtResolution_AK4PFchs')
    phiTag= cms.string('JR_Fall17_V3_102X_MC_PhiResolution_AK4PFchs')
    sfTag = cms.string('JR_Fall17_V3_102X_MC_SF_AK4PFchs')

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

print "-> Updating the jets collection to run on to 'updatedPatJetsUpdatedJEC' with the new jec in the GT/or DB"
process.nero.NeroJets.jets=cms.InputTag('updatedPatJetsUpdatedJEC')
process.nero.NeroFatJets.chsAK8=cms.InputTag('updatedPatJetsUpdatedJECAK8')
process.jecSequence = cms.Sequence( 
        #process.patJetCorrFactorsReapplyJEC + process.patJetsReapplyJEC
        process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC 
        * process.patJetCorrFactorsUpdatedJECAK8* process.updatedPatJetsUpdatedJECAK8
        )

########################################## MET ##########################
## Update met collection ?
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
runMetCorAndUncFromMiniAOD(process,
        isData=options.isData,
        )

#-----------------------ELECTRON ID-------------------------------

# ------------- Soft jet activity: Track jets ------------------------

from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets

process.chsForSATkJets = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string('charge()!=0 && pvAssociationQuality()>=5 && vertexRef().key()==0'))
process.softActivityJets = ak4PFJets.clone(src = 'chsForSATkJets', doAreaFastjet = False, jetPtMin=1) 

#------------------------------------------

# ------------------------QG-----------------------------------------------
# after jec, because need to be run on the corrected (latest) jet collection
qgDatabaseVersion = 'cmssw8020_v2'

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

################## FSR PHOTONS ###############

from FSRPhotonRecovery.FSRPhotons.FSRphotonSequence_cff import addFSRphotonSequence 

print "-> Adding FSR photons to:",process.nero.NeroLeptons.muons.value()
PhotonMVA="FSRPhotonRecovery/FSRPhotons/data/PhotonMVAv9_BDTG800TreesDY.weights.xml"
addFSRphotonSequence(process, process.nero.NeroLeptons.muons.value(), PhotonMVA)


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

## Two muons preselection
#process.nero.info= cms.string("two_muons")
#process.load('NeroProducer.Skim.MuonFilterSequence_cff')

process.nero.info = cms.string( str(process.nero.info) +"  |options:" + ','.join(sys.argv))

##DEBUG
##print "Process=",process, process.__dict__.keys()
#------------------------------------------------------
process.p = cms.Path(
                process.infoProducerSequence *
                process.egammaPostRecoSeq *  #-> should be everything from EGamma
                process.jecSequence *
                process.fullPatMetSequence *
                process.QGTagger    * ## after jec, because it will produce the new jet collection
                process.ttbarcat *
                process.QGVariablesSequence*
                process.NjettinessGroomed*
                process.chsForSATkJets * process.softActivityJets * ## track jtes
                process.bRegressionProducer*
                process.FSRphotonSequence*
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
