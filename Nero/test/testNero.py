import FWCore.ParameterSet.Config as cms

process = cms.Process("nero")

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


#fileList = [ "/store/mc/Spring14miniaod/WToTauNu_M_200_Tune4C_tauola_13TeV_pythia8/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/00100400-9007-E411-964D-0025905AC822.root",
#	     "/store/mc/Spring14miniaod/WToTauNu_M_200_Tune4C_tauola_13TeV_pythia8/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/08194701-9007-E411-B181-0025905AC982.root",
#	    ]
fileList = []


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

## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')

process.load('NeroProducer.Nero.Nero_cfi')

#------------------------------------------------------
process.p = cms.Path(
		process.infoProducerSequence *
                process.QGTagger *
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.nero
                )
