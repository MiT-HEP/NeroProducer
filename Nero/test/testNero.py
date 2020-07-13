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

if '2015' in options.isRun: isYear=2015
elif '2017' in options.isRun: isYear=2017
elif '2016' in options.isRun: isYear=2016
elif '2018' in options.isRun: isYear=2018
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


if len(options.inputFiles) == 0:
    if isData:
        options.inputFiles = [
                '/store/data/Run2017G/SingleMuon/MINIAOD/09Aug2019_UL2017-v1/20000/5B32D24F-BDAD-6F45-95B9-6424DE9D8075.root', ## Single Muon
        ]
    else:
        options.inputFiles = [
                '/store/mc/RunIISummer19UL17MiniAOD/TTTo2L2Nu_TuneCP5CR2_13TeV-powheg-pythia8/MINIAODSIM/106X_mc2017_realistic_v6-v1/50000/E876FEB5-706D-B348-ACE3-EA5FD04F44D7.root' ## TT UL 2017
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


## https://twiki.cern.ch/twiki/bin/viewauth/CMS/JECDataMC
## Technically we don't have to correct the jets/met on the
## fly any more, because jec is in the release.

if (isData):
    # 2017 data
    process.GlobalTag.globaltag = '106X_dataRun2_v28' 
else:
    ## new miniaod
    if isYear==2016:
        process.GlobalTag.globaltag = '106X_mcRun2_asymptotic_preVFP_v8' 
    elif isYear==2016: ## toFIX
        process.GlobalTag.globaltag = '106X_mcRun2_asymptotic_v13' 
    elif isYear==2017:
        process.GlobalTag.globaltag = '106X_mc2017_realistic_v7' 
    elif isYear==2018:
        process.GlobalTag.globaltag = '106X_upgrade2018_realistic_v11_L1v1' 

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

## TODO
#from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
#setupEgammaPostRecoSeq(process,
#                       runVID=True,
#                       era='2017-Nov17ReReco')  

############################### JEC #####################
#### Load from a sqlite db, if not read from the global tag
process.load("CondCore.DBCommon.CondDBCommon_cfi")
from CondCore.DBCommon.CondDBSetup_cfi import *

if options.isData:
    if isYear == 2017:
        connectString = cms.string('sqlite:jec/Summer19UL17_RunBCDEF_V5_DATA.db')
        tagName = 'Summer19UL17_RunBCDEF_V5_DATA_AK4PFchs'
        tagNamePuppi = 'Summer19UL17_RunBCDEF_V5_DATA_AK4PFPuppi'
    else: raise ValueError("Not implemented")
    #if isYear == 2016:
    #    connectString = cms.string('sqlite:jec/Summer16_07Aug2017All_V11_DATA.db')
    #    tagName = 'Summer16_07Aug2017_V11_94X_DATA_AK4PFchs'
    #    tagNamePuppi = 'Summer16_07Aug2017_V11_94X_DATA_AK4PFPuppi'
else:
    if isYear==2017:
        connectString = cms.string('sqlite:jec/Summer19UL17_V5_MC.db')
        tagName = 'Summer19UL17_V5_MC_AK4PFchs'
        tagNamePuppi = 'Summer19UL17_V5_MC_AK4PFPuppi'
    else: raise ValueError("Not implemented")

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
toGet=[]
if options.isData:
    if isYear==2017:
        jerString = cms.string('sqlite:jer/Summer19UL17_JRV2_DATA.db')
        resTag= cms.string('JR_Summer19UL17_JRV2_DATA_PtResolution_AK4PFchs')
        phiTag= cms.string('JR_Summer19UL17_JRV2_DATA_PhiResolution_AK4PFchs')
        sfTag = cms.string('JR_Summer19UL17_JRV2_DATA_SF_AK4PFchs')
    else: raise ValueError("Not implemented")
    #if isYear==2016: # no _94X
    #    jerString = cms.string('sqlite:jer/Summer16_25nsV1_DATA.db')
    #    resTag= cms.string('JR_Summer16_25nsV1_DATA_PtResolution_AK4PFchs')
    #    phiTag= cms.string('JR_Summer16_25nsV1_DATA_PhiResolution_AK4PFchs')
    #    sfTag = cms.string('JR_Summer16_25nsV1_DATA_SF_AK4PFchs')
else:
    if isYear==2017:
        jerString = cms.string('sqlite:jer/Summer19UL17_JRV2_MC.db')
        resTag= cms.string('JR_Summer19UL17_JRV2_MC_PtResolution_AK4PFchs')
        #phiTag= cms.string('JR_Summer19UL17_JRV2_MC_PhiResolution_AK4PFchs')
        sfTag = cms.string('JR_Summer19UL17_JRV2_MC_SF_AK4PFchs')
    else: raise ValueError("Not implemented")
    #if isYear==2016: # no _94X
    #    jerString = cms.string('sqlite:jer/Summer16_25nsV1_MC.db')
    #    resTag= cms.string('JR_Summer16_25nsV1_MC_PtResolution_AK4PFchs')
    #    phiTag= cms.string('JR_Summer16_25nsV1_MC_PhiResolution_AK4PFchs')
    #    sfTag = cms.string('JR_Summer16_25nsV1_MC_SF_AK4PFchs')

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
             #cms.PSet(
             #    record = cms.string('JetResolutionRcd'),
             #    tag    = phiTag,
             #    label  = cms.untracked.string('AK4PFchs_phi')
             #    ),        
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

#from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
#
#runMetCorAndUncFromMiniAOD (
#        process,
#        isData = isData, # false for MC
        #reclusterJets = True, ## FIXME
#        pfCandColl=cms.InputTag("packedPFCandidates"),
#        CHS = True,
#        postfix = "ModifiedMET"
#)

print "-> Updating the jets collection to run on to 'updatedPatJetsUpdatedJEC' with the new jec in the GT/or DB"
process.nero.NeroJets.jets=cms.InputTag('updatedPatJetsUpdatedJEC')
process.nero.NeroFatJets.chsAK8=cms.InputTag('updatedPatJetsUpdatedJECAK8')
#process.nero.NeroMet.mets=cms.InputTag("slimmedMETsModifiedMET")

process.jecSequence = cms.Sequence( 
        #process.patJetCorrFactorsReapplyJEC + process.patJetsReapplyJEC
        process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC 
        * process.patJetCorrFactorsUpdatedJECAK8* process.updatedPatJetsUpdatedJECAK8
        #* process.fullPatMetSequenceModifiedMET 
        #* process.ecalBadCalibReducedMINIAODFilter
        )


#-----------------------ELECTRON ID-------------------------------

from RecoEgamma.EgammaTools.EgammaPostRecoTools import setupEgammaPostRecoSeq
setupEgammaPostRecoSeq(process,
                    runVID=False, #saves CPU time by not needlessly re-running VID, if you want the Fall17V2 IDs, set this to True or remove (default is True)
                    era='%d-UL'%isYear)    
#a sequence egammaPostRecoSeq has now been created and should be added to your path, eg process.p=cms.Path(process.egammaPostRecoSeq)

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

#process.QGPoolDBESSource = cms.ESSource("PoolDBESSource",
#      CondDBSetup,
#      toGet = cms.VPSet(
#        cms.PSet(
#            record = cms.string('QGLikelihoodRcd'),
#            tag    = cms.string('QGLikelihoodObject_v1_AK4'),
#            label  = cms.untracked.string('QGL_AK4PFchs')
#        ),
#      ),
#      connect = cms.string('sqlite:qg/QGL_AK4chs_94X.db')
#)
#process.es_prefer_qg = cms.ESPrefer('PoolDBESSource','QGPoolDBESSource')
#
process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets             = process.nero.NeroJets.jets   # Could be reco::PFJetCollection or pat::JetCollection (both AOD and miniAOD)               
process.QGTagger.srcVertexCollection = process.nero.NeroVertex.vertices
process.QGTagger.useQualityCuts = cms.bool(False)
#
## ----------------------- GenHFHadronMatcher -----------------

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
################## FSR PHOTONS ###############

#process.load("NeroProducer.Nero.fsrPhotons_cfi")

from FSRPhotonRecovery.FSRPhotons.FSRphotonSequence_cff import addFSRphotonSequence 

print "-> Adding FSR photons to:",process.nero.NeroLeptons.muons.value()
PhotonMVA="FSRPhotonRecovery/FSRPhotons/data/PhotonMVAv9_BDTG800TreesDY.weights.xml"
addFSRphotonSequence(process, process.nero.NeroLeptons.muons.value(), PhotonMVA)
#electrons = cms.InputTag("slimmedElectrons","","nero"),
process.FSRRecovery.patphotons= cms.InputTag("slimmedPhotons","","nero")


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
