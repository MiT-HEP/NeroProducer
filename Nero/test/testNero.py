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

options.parseArguments()
isData = options.isData

if options.isData:
    print "-> Loading DATA configuration"
else:
    print "-> Loading MC configuration"

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

if len(options.inputFiles) == 0:
    if isData:
        options.inputFiles = [
            '/store/data/Run2016B/SingleMuon/MINIAOD/23Sep2016-v3/60000/3A6A80A6-D797-E611-B571-0CC47A04CFF6.root'
        ]
    else:
        options.inputFiles = [
            '/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_backup_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/10000/FAF828EF-1EBB-E611-9151-848F69FD483E.root'
        ]

### do not remove the line below!
###FILELIST###

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

if (isData):
    # sept reprocessing
    process.GlobalTag.globaltag = '80X_dataRun2_2016SeptRepro_v3'
else:
    ## tranch IV v6
    process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_TrancheIV_v6'

### LOAD DATABASE
from CondCore.DBCommon.CondDBSetup_cfi import *
#from CondCore.CondDB.CondDB_cfi import *

######## LUMI MASK
if isData and not options.isGrid and False: ## dont load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    ## SILVER
    print "-> UPDATE THE LUMI LIST"
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt').getVLuminosityBlockRange()

## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')

### ##ISO
process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")


############################### JEC #####################
#### Load from a sqlite db, if not read from the global tag
#process.load("CondCore.DBCommon.CondDBCommon_cfi")
from CondCore.DBCommon.CondDBSetup_cfi import *

if options.isData:
    connectString = cms.string('sqlite:jec/Summer16_23Sep2016AllV3_DATA.db')
    tagName = 'Summer16_23Sep2016AllV3_DATA_AK4PFchs'
    tagNamePuppi = 'Summer16_23Sep2016AllV3_DATA_AK4PFPuppi'
else:
    connectString = cms.string('sqlite:jec/Summer16_23Sep2016V3_MC.db')
    tagName = 'Summer16_23Sep2016V3_MC_AK4PFchs'
    tagNamePuppi = 'Summer16_23Sep2016V3_MC_AK4PFPuppi'
#data only, mc hard coded
process.nero.chsAK8JEC = cms.string("jec/Summer16_23Sep2016BCDV3")


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
    jerString = cms.string('sqlite:jer/Spring16_25nsV6_DATA.db')
    resTag= cms.string('JR_Spring16_25nsV6_DATA_PtResolution_AK4PFchs')
    phiTag= cms.string('JR_Spring16_25nsV6_DATA_PhiResolution_AK4PFchs')
    sfTag = cms.string('JR_Spring16_25nsV6_DATA_SF_AK4PFchs')
else:
    jerString = cms.string('sqlite:jer/Spring16_25nsV6_MC.db')
    resTag=cms.string('JR_Spring16_25nsV6_MC_PtResolution_AK4PFchs')
    phiTag= cms.string('JR_Spring16_25nsV6_MC_PhiResolution_AK4PFchs')
    sfTag = cms.string('JR_Spring16_25nsV6_MC_SF_AK4PFchs')

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
    jetSource = process.nero.jets,
    labelName = 'UpdatedJEC',
    jetCorrections = ('AK4PFchs', cms.vstring(jecLevels), 'None')  # Do not forget 'L2L3Residual' on data!
)

updateJetCollection(
    process,
    jetSource = process.nero.chsAK8,
    labelName = 'UpdatedJECAK8',
    jetCorrections = ('AK8PFchs', cms.vstring(jecLevels), 'None')  # Do not forget 'L2L3Residual' on data!
)

print "-> Updating the jets collection to run on to 'updatedPatJetsUpdatedJEC' with the new jec in the GT"
process.nero.jets=cms.InputTag('updatedPatJetsUpdatedJEC')
process.nero.chsAK8=cms.InputTag('updatedPatJetsUpdatedJECAK8')
process.jecSequence = cms.Sequence( process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC 
        * process.patJetCorrFactorsUpdatedJECAK8* process.updatedPatJetsUpdatedJECAK8
        )

########### MET Filter ################
process.load('RecoMET.METFilters.BadPFMuonFilter_cfi')
process.BadPFMuonFilter.muons = cms.InputTag("slimmedMuons")
process.BadPFMuonFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadPFMuonFilter.taggingMode = cms.bool(True)

process.load('RecoMET.METFilters.BadChargedCandidateFilter_cfi')
process.BadChargedCandidateFilter.muons = cms.InputTag("slimmedMuons")
process.BadChargedCandidateFilter.PFCandidates = cms.InputTag("packedPFCandidates")
process.BadChargedCandidateFilter.taggingMode = cms.bool(True)

############ RECOMPUTE MET #######################
from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
runMetCorAndUncFromMiniAOD(process,
           isData=isData,
           )

print "-> Updating the met collection to run on to 'slimmedMETs with nero' with the new jec in the GT for Type1"
process.nero.mets=cms.InputTag('slimmedMETs','','nero')

## TO DO this is production specific, when testing on relval it works with RECO, maybe we should leave it empty
##if not options.isData:
##            process.nero.metFilterToken=cms.InputTag("TriggerResults","","PAT")

############ RUN Puppi MET CLUSTERING ##########################

from PhysicsTools.PatAlgos.slimming.puppiForMET_cff import makePuppiesFromMiniAOD
makePuppiesFromMiniAOD( process, True );

runMetCorAndUncFromMiniAOD(process,
                              isData=options.isData,
                              metType="Puppi",
                              pfCandColl=cms.InputTag("puppiForMET"),
                              recoMetFromPFCs=True,
                              jetFlavor="AK4PFPuppi",
                              postfix="Puppi"
                              )
#-----------------------ELECTRON ID-------------------------------
from NeroProducer.Nero.egammavid_cfi import *

initEGammaVID(process,options)

############### end of met conf avoid overwriting

## turn off the existing weight usage for the spring16 monte carlo
## this slows the computation by x2, but is necessary

process.puppiNoLep.useExistingWeights = False
process.puppi.useExistingWeights = False
process.puppiForMET.photonId = process.nero.phoLooseIdMap

print "-> Updating the puppi met collection to run on to 'slimmedMETsPuppi with nero' with the new jec in the GT for Type1"
process.nero.metsPuppi=cms.InputTag('slimmedMETsPuppi','','nero')

############### REGRESSION EGM #############
process.load('EgammaAnalysis.ElectronTools.regressionApplication_cff')
from EgammaAnalysis.ElectronTools.regressionWeights_cfi import regressionWeights
process = regressionWeights(process)

########## EGM Smear and Scale ###
process.load('Configuration.StandardSequences.Services_cff')
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService",
        calibratedPatElectrons  = cms.PSet( initialSeed = cms.untracked.uint32(81),
            engineName = cms.untracked.string('TRandom3'),
            ),
        calibratedPatPhotons  = cms.PSet( initialSeed = cms.untracked.uint32(81),
            engineName = cms.untracked.string('TRandom3'),
            ),
        )
process.load('EgammaAnalysis.ElectronTools.calibratedElectronsRun2_cfi')
process.load('EgammaAnalysis.ElectronTools.calibratedPhotonsRun2_cfi')
process.calibratedPatElectrons.electrons=process.nero.electrons
process.calibratedPatPhotons.photons= process.nero.photons
process.nero.electrons =  cms.InputTag("calibratedPatElectrons")
process.nero.photons = cms.InputTag("calibratedPatPhotons")
print "-> Updating slimmedElectrons and slimmedPhotons to calibratedPatElectrons and calibratedPatPhotons"
print "   ",process.nero.photons, process.nero.electrons
#######################################

# modify electrons Input Tags
process.egmGsfElectronIDs.physicsObjectSrc = process.nero.electrons
process.electronIDValueMapProducer.srcMiniAOD= process.nero.electrons
process.electronMVAValueMapProducer.srcMiniAOD= process.nero.electrons

# modify photons Input Tags
process.egmPhotonIsolation.srcToIsolate = process.nero.photons
process.egmPhotonIDs.physicsObjectSrc = process.nero.photons
process.photonIDValueMapProducer.srcMiniAOD= process.nero.photons
process.photonMVAValueMapProducer.srcMiniAOD= process.nero.photons 
process.puppiForMET.photonName  = process.nero.photons
process.puppiPhoton.photonName = process.nero.photons 
process.modifiedPhotons.src  = process.nero.photons

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

# ----------------------- GenHFHadronMatcher -----------------

process.load("PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff")

from PhysicsTools.JetMCAlgos.HadronAndPartonSelector_cfi import selectedHadronsAndPartons
process.selectedHadronsAndPartons = selectedHadronsAndPartons.clone(
            particles = process.nero.prunedgen
            )
from PhysicsTools.JetMCAlgos.AK4PFJetsMCFlavourInfos_cfi import ak4JetFlavourInfos
process.genJetFlavourInfos = ak4JetFlavourInfos.clone(
            jets = process.nero.genjets
            )

from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenBHadron
process.matchGenBHadron = matchGenBHadron.clone(
            genParticles = process.nero.prunedgen,
            jetFlavourInfos = "genJetFlavourInfos"
            )
from PhysicsTools.JetMCAlgos.GenHFHadronMatcher_cff import matchGenCHadron
process.matchGenCHadron = matchGenCHadron.clone(
            genParticles = process.nero.prunedgen,
            jetFlavourInfos = "genJetFlavourInfos"
            )
process.load("TopQuarkAnalysis.TopTools.GenTtbarCategorizer_cfi")
process.categorizeGenTtbar.genJets = process.nero.genjets

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

##DEBUG
##print "Process=",process, process.__dict__.keys()
#------------------------------------------------------
process.p = cms.Path(
                process.infoProducerSequence *
                process.regressionApplication *
                process.calibratedPatElectrons  *
                process.calibratedPatPhotons *
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence * ## this is overwritten by puppi/met configuration
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer *  ## ISO MAP FOR PHOTONS
                process.jecSequence *
                process.QGTagger    * ## after jec, because it will produce the new jet collection
                process.fullPatMetSequence *## no puppi
                process.puppiMETSequence * #puppi candidate producer
                process.fullPatMetSequencePuppi * ## full puppi sequence
                process.BadPFMuonFilter *
                process.BadChargedCandidateFilter * 
                process.ttbarcat *
                process.nero
                )

## DEBUG -- dump the event content with all the value maps ..
##process.output = cms.OutputModule(
##                "PoolOutputModule",
##                      fileName = cms.untracked.string('output.root'),
##                      )
##process.output_step = cms.EndPath(process.output)
##
##process.schedule = cms.Schedule(
##		process.p,
##		process.output_step)

# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
