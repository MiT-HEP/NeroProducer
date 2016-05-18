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
options.register('is2016', False, VarParsing.VarParsing.multiplicity.singleton,VarParsing.VarParsing.varType.bool,"Set to true to run on 2016 data")

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
if options.is2016:
    print "-> Loading 2016 data configuration"

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

if isData:
   fileList = [
       #'/store/data/Run2015D/MET/MINIAOD/16Dec2015-v1/50000/00EA1DB2-90AA-E511-AEEE-0025905C2CE6.root'
       #'/store/data/Run2015D/DoubleMuon/MINIAOD/16Dec2015-v1/10000/000913F7-E9A7-E511-A286-003048FFD79C.root'
       ]
else:
   fileList = [
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/626CD584-6AF3-E511-986F-001E67DDBEDA.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/6C339CAD-54F3-E511-8BD4-90B11C12E856.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/704816A7-54F3-E511-802A-001E67A3ED40.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7612ABC7-60F3-E511-9AFA-001E67A3F49D.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7A9FA1C7-5BF3-E511-93D7-001E67A3EC05.root",
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
        if options.is2016:
            process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'
        else:
            process.GlobalTag.globaltag = '76X_dataRun2_16Dec2015_v0'
    if options.is50ns:
        process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v1'
        print "FIX GLOBAL TAG"
else:
    if options.is25ns:
        #process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_RunIIFall15DR76_v1'
        process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_v3'
    if options.is50ns:
        process.GlobalTag.globaltag = 'MCRUN2_74_V9A::All' ## FIXME
        print "FIX GLOBAL TAG"

### LOAD DATABASE
from CondCore.DBCommon.CondDBSetup_cfi import *
#from CondCore.CondDB.CondDB_cfi import *

######## LUMI MASK
if isData and not options.isGrid and not options.is2016: ## dont load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    ## SILVER
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt').getVLuminosityBlockRange()
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver_v2.txt').getVLuminosityBlockRange()
    print "FIX JSON"

## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')
#process.load('NeroProducer.Nero.NeroMonotop_cfi')
#process.load('NeroProducer.Nero.NeroMonojet_cfi')
#process.load('NeroProducer.Nero.NeroChargedHiggs_cfi')

#-----------------------ELECTRON ID-------------------------------
from NeroProducer.Nero.egammavid_cfi import *

initEGammaVID(process,options)

### ##ISO
process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")

#------------------- JER -----------------
toGet=[]
if options.isData:
    jerString = cms.string('sqlite:jer/Summer15_25nsV6_DATA.db')
    resTag= cms.string('JR_Summer15_25nsV6_DATA_PtResolution_AK4PFchs')
    phiTag= cms.string('JR_Summer15_25nsV6_DATA_PhiResolution_AK4PFchs')
    sfTag = cms.string('JR_Summer15_25nsV6_DATA_SF_AK4PFchs')
else:
    jerString = cms.string('sqlite:jer/Summer15_25nsV6_MC.db')
    resTag=cms.string('JR_Summer15_25nsV6_MC_PtResolution_AK4PFchs')
    phiTag= cms.string('JR_Summer15_25nsV6_MC_PhiResolution_AK4PFchs')
    sfTag = cms.string('JR_Summer15_25nsV6_MC_SF_AK4PFchs')

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
### from PhysicsTools.PatAlgos.tools.jetTools import updateJetCollection
### 
### jecLevels= ['L1FastJet',  'L2Relative', 'L3Absolute']
### if options.isData:
###         jecLevels =['L1FastJet',  'L2Relative', 'L3Absolute', 'L2L3Residual']
### 
### updateJetCollection(
###    process,
###    jetSource = process.nero.jets,
###    labelName = 'UpdatedJEC',
###    jetCorrections = ('AK4PFchs', cms.vstring(jecLevels), 'None')  # Do not forget 'L2L3Residual' on data!
### )
### print "-> Updating the jets collection to run on to 'updatedPatJetsUpdatedJEC' with the new jec in the GT"
### process.nero.jets=cms.InputTag('updatedPatJetsUpdatedJEC')
### process.jecSequence = cms.Sequence( process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC )

############ RECOMPUTE MET #######################
## from PhysicsTools.PatUtils.tools.runMETCorrectionsAndUncertainties import runMetCorAndUncFromMiniAOD
## runMetCorAndUncFromMiniAOD(process,
##            isData=isData,
##            )
##
## print "-> Updating the met collection to run on to 'slimmedMETs with nero' with the new jec in the GT for Type1"



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
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer *  ## ISO MAP FOR PHOTONS
                #process.jecSequence *
                process.QGTagger    * ## after jec, because it will produce the new jet collection
                #process.fullPatMetSequence *## no puppi
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
# mode:c++
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
