import FWCore.ParameterSet.Config as cms

process= cms.Process("SKIM")

process.load('FWCore.MessageService.MessageLogger_cfi')

process.source = cms.Source("PoolSource",
		   #fileNames = cms.untracked.vstring('/store/relval/CMSSW_7_4_0/RelValTTbarLepton_13/MINIAODSIM/MCRUN2_74_V7_gensim_740pre7-v1/00000/603E7935-4EDD-E411-B16E-0025905A612E.root')
		   #fileNames= cms.untracked.vstring('/store/mc/RunIISpring15MiniAODv2/TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/74X_mcRun2_asymptotic_v2-v1/00000/FA1FCFC0-D55C-E511-B06D-7845C4FC3AFD.root')
		   fileNames=cms.untracked.vstring('/store/data/Run2015C/JetHT/MINIAOD/PromptReco-v1/000/253/808/00000/3E6025B5-7340-E511-A8B7-02163E01440E.root')
		   )

process.maxEvents = cms.untracked.PSet(
		   input= cms.untracked.int32(-1)
		   )

process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Skim.HPlusFilterSequence_cff')

process.skimPath = cms.Path( 
		process.infoProducerSequence *
		process.HPlusFilter 
		)

process.output = cms.OutputModule(
		   "PoolOutputModule",
		      SelectEvents = cms.untracked.PSet(SelectEvents = cms.vstring('skimPath')),
		         fileName = cms.untracked.string('output.root'),
			 )
process.output_step = cms.EndPath(process.output)

# ------------------------QG-----------------------------------------------
process.load('RecoJets.JetProducers.QGTagger_cfi')
process.QGTagger.srcJets            = cms.InputTag("slimmedJets")    # Could be reco::PFJetCollection or pat::JetCollection (both AOD and miniAOD)               
process.QGTagger.srcVertexCollection = cms.InputTag("offlineSlimmedPrimaryVertices")
process.QGTagger.useQualityCuts = cms.bool(False)
##----------------GLOBAL TAG ---------------------------
# used by photon id and jets
#process.load("Configuration.StandardSequences.Geometry_cff") ### VETO BY HBB 74X SEQ
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
#data
#process.GlobalTag.globaltag = 'GR_R_52_V9::All'
#mc
process.GlobalTag.globaltag = 'PLS170_V7AN2::All'


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

################ HBB 74X ##############
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

process.HBB = cms.Sequence(
		process.pfImpactParameterTagInfosAK8 *
		process.pfInclusiveSecondaryVertexFinderTagInfosAK8 *
		process.softPFMuonsTagInfosAK8 *
		process.softPFElectronsTagInfosAK8 *
		process.pfBoostedDoubleSecondaryVertexAK8BJetTags 
		)
####################JEC
process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
jecLevels= ['L1FastJet',  'L2Relative', 'L3Absolute']

isData=False
print "FIXME isData is Set To:", isData, "Implement options. (for jec)"
#if options.isData: ## TODO options
if isData: ## TODO options
	jecLevels.append( 'L2L3Residuals')
process.patJetCorrFactorsReapplyJEC = process.patJetCorrFactorsUpdated.clone(
		  src = cms.InputTag("slimmedJets"),
		  levels = ['L1FastJet',  'L2Relative', 'L3Absolute'],
		  payload = 'AK4PFchs' ) # Make sure to choose the appropriate levels and payload here!

process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
process.patJetsReapplyJEC = process.patJetsUpdated.clone(
		  jetSource = cms.InputTag("slimmedJets"),
		  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
		  )

process.jec = cms.Sequence( process.patJetCorrFactorsReapplyJEC + process. patJetsReapplyJEC )

#################################
process.extraPath = cms.Path(
                	process.QGTagger *
                	process.egmGsfElectronIDSequence *
                	process.egmPhotonIDSequence *
                	process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
			process.jec *
			process.HBB ## HBB 74X
		)

process.schedule = cms.Schedule(
		    	process.skimPath, 
		    	process.extraPath, 
		    	process.output_step
			)

# Spit out filter efficiency at the end.
process.options = cms.untracked.PSet(
		wantSummary = cms.untracked.bool(True)
		)
