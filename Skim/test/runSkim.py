import FWCore.ParameterSet.Config as cms

process= cms.Process("SKIM")

process.load('FWCore.MessageService.MessageLogger_cfi')

process.source = cms.Source("PoolSource",
		   fileNames = cms.untracked.vstring('/store/relval/CMSSW_7_4_0/RelValTTbarLepton_13/MINIAODSIM/MCRUN2_74_V7_gensim_740pre7-v1/00000/603E7935-4EDD-E411-B16E-0025905A612E.root')
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
process.load("Configuration.StandardSequences.Geometry_cff")
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

process.extraPath = cms.Path(
                	process.QGTagger *
                	process.egmGsfElectronIDSequence *
                	process.egmPhotonIDSequence *
                	process.photonIDValueMapProducer ## ISO MAP FOR PHOTONS
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
