import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

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

options.parseArguments()
isData = options.isData

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

fileList = [
    #'/store/data/Run2015B/MET/MINIAOD/PromptReco-v1/000/251/643/00000/CC77B94F-902C-E511-9A26-02163E01369B.root'
    #'/store/relval/CMSSW_7_4_1/RelValADDMonoJet_d3MD3_13/MINIAODSIM/MCRUN2_74_V9_gensim71X-v1/00000/80CF5456-B9EC-E411-93DA-002618FDA248.root'
    '/store/user/arapyan/mc/SUSYGluGluToTBHPTohbbW/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM2/MINIAOD_99.root'
    #'/store/relval/CMSSW_7_4_1/RelValProdTTbar_13/MINIAODSIM/MCRUN2_74_V9_gensim71X-v1/00000/0A9E2CED-C9EC-E411-A8E4-003048FFCBA8.root']
    #'/store/data/Run2015B/DoubleMuon/MINIAOD/PromptReco-v1/000/251/164/00000/402F0995-A326-E511-86BB-02163E013948.root',
    #'/store/data/Run2015B/DoubleMuon/MINIAOD/PromptReco-v1/000/251/167/00000/70C4A781-A826-E511-95B4-02163E013414.root',
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
    process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v0'
    
    import FWCore.PythonUtilities.LumiList as LumiList
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/DCSOnly/json_DCSONLY_Run2015B.txt').getVLuminosityBlockRange()
    # GoldenJsn
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-251642_13TeV_PromptReco_Collisions15_JSON.txt').getVLuminosityBlockRange()

else:
    process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    process.GlobalTag.globaltag = 'MCRUN2_74_V9::All'

#-----------------------ELECTRON ID-------------------------------
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
# turn on VID producer, indicate data format  to be
# DataFormat.AOD or DataFormat.MiniAOD, as appropriate 
useAOD=False

if useAOD == True :
    dataFormat = DataFormat.AOD
else :
    dataFormat = DataFormat.MiniAOD

switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce
my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_PHYS14_PU20bx25_V2_cff']
#                 'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV51_cff']

#add them to the VID producer
for idmod in my_id_modules:
    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

### PHOTONS
switchOnVIDPhotonIdProducer(process, dataFormat) ### PHOTON
pho_id_modules = ['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_PHYS14_PU20bx25_V2_cff']

for idmod in pho_id_modules:
        setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)
##ISO
process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")

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
#process.load('NeroProducer.Nero.NeroMonojet_cfi')
#process.load('NeroProducer.Nero.NeroChargedHiggs_cfi')

if options.isGrid:
	process.nero.head=options.nerohead ##'git rev-parse HEAD'
	process.nero.tag=options.nerotag ## git describe --tags

#------------------------------------------------------
process.p = cms.Path(
		process.infoProducerSequence *
                process.QGTagger *
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer * ## ISO MAP FOR PHOTONS
		process.HBB * ## HBB 74X
                process.nero
                )

## DEBUG -- dump the event content with all the value maps ..
## process.schedule = cms.Schedule(
## 		process.p,
## 		process.output_step)
