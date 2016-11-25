import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
import re
import os

process = cms.Process("nero")
cmssw_base = os.environ['CMSSW_BASE']

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

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

if isData:
   fileList = [
        '/store/data/Run2016B/SingleMuon/MINIAOD/23Sep2016-v3/60000/3A6A80A6-D797-E611-B571-0CC47A04CFF6.root'
       ]
else:
   fileList = [
       "file:1A41BFCB-6DA5-E611-A620-00259057495C.root" ## monophoton trancheIV
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

#-----------------------ELECTRON ID-------------------------------
from NeroProducer.Nero.egammavid_cfi import *

initEGammaVID(process,options)

### ##ISO
process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")


################# JEC 
############################### JEC #####################
#### Load from a sqlite db, if not read from the global tag
#process.load("CondCore.DBCommon.CondDBCommon_cfi")
from CondCore.DBCommon.CondDBSetup_cfi import *

if options.isData:
    connectString = cms.string('sqlite:jec/Spring16_25nsV8BCD_DATA.db')
    tagName = 'Spring16_25nsV8BCD_DATA_AK4PFchs'
    ## use the first to check run, TODO, check that this work with crab
    if 'Run2016E' in process.source.fileNames[0]:
        connectString = cms.string('sqlite:jec/Spring16_25nsV8E_DATA.db')
        tagName = 'Spring16_25nsV8E_DATA_AK4PFchs'
    if 'Run2016F' in process.source.fileNames[0]:
        connectString = cms.string('sqlite:jec/Spring16_25nsV8F_DATA.db')
        tagName = 'Spring16_25nsV8F_DATA_AK4PFchs'
        print "TODO: FIX JEC for A PART OF F->G run>=278802"
    if 'Run2016G' in process.source.fileNames[0] or 'Run2016H' in process.source.fileNames[0]:
        connectString = cms.string('sqlite:jec/Spring16_25nsV8p2_DATA.db')
        tagName = 'Spring16_25nsV8G_DATA_AK4PFchs'
    print "JEC SUMMARY FOR DATA: using file",connectString
else:
    #connectString = cms.string('sqlite:jec/Spring16_25nsV8_MC.db')
    #tagName = 'Spring16_25nsV8_MC_AK4PFchs'
    connectString = cms.string('sqlite:jec/Spring16_25nsV8BCD_MC.db')
    tagName = 'Spring16_25nsV8BCD_MC_AK4PFchs'


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

# Electron Smear/Scale                                                                                                                                                          
process.selectedElectrons = cms.EDFilter("PATElectronSelector", 
                                         src = cms.InputTag("slimmedElectrons"), 
                                         cut = cms.string("pt > 5 && abs(eta)<2.5") 
                                         ) 
 
process.RandomNumberGeneratorService = cms.Service("RandomNumberGeneratorService", 
                                                   calibratedPatElectrons = cms.PSet(initialSeed = cms.untracked.uint32(123456), 
                                                                                     engineName = cms.untracked.string('TRandom3') 
                                                                                     ) ,
                                                   calibratedPatPhotons = cms.PSet(initialSeed = cms.untracked.uint32(654321), 
                                                                                   engineName = cms.untracked.string('TRandom3') 
                                                                                   ) 
                                                   ) 
 
process.load('EgammaAnalysis.ElectronTools.calibratedElectronsRun2_cfi') 
process.calibratedPatElectrons = cms.EDProducer("CalibratedPatElectronProducerRun2", 
                                                electrons = cms.InputTag('selectedElectrons'), 
                                                gbrForestName = cms.string("gedelectron_p4combination_25ns"), 
                                                isMC = cms.bool(not isData), 
                                                isSynchronization = cms.bool(False), 
                                                correctionFile = cms.string("EgammaAnalysis/ElectronTools/data/ScalesSmearings/80X_Golden22June_approval") 
                                                )

#Photons
process.load('EgammaAnalysis.ElectronTools.calibratedPhotonsRun2_cfi') 
process.calibratedPatPhotons = cms.EDProducer("CalibratedPatPhotonProducerRun2", 
                                              photons = cms.InputTag('slimmedPhotons'),
                                              isMC = cms.bool(not isData), 
                                              isSynchronization = cms.bool(False),
                                              correctionFile = cms.string("EgammaAnalysis/ElectronTools/data/ScalesSmearings/80X_Golden22June_approval") 
                                              )

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
if not options.isData:
            process.nero.metFilterToken=cms.InputTag("TriggerResults","","PAT")

############ RUN CLUSTERING ##########################
process.puppiSequence = cms.Sequence()
process.puppiMetSequence = cms.Sequence()
process.jetSequence = cms.Sequence()

if process.nero.doReclustering:
    if process.nero.doPuppi:
        # run puppi algo
        process.load('NeroProducer.Nero.Puppi_cff')

        process.puppi.candName   = cms.InputTag('packedPFCandidates')
        process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')

        process.pfCandNoLep = cms.EDFilter("CandPtrSelector", 
                                            src = cms.InputTag("packedPFCandidates"), 
                                            cut =  cms.string("abs(pdgId) != 13 && abs(pdgId) != 11 && abs(pdgId) != 15"))
        process.pfCandLep   = cms.EDFilter("CandPtrSelector", 
                                            src = cms.InputTag("packedPFCandidates"), 
                                            cut = cms.string("abs(pdgId) == 13 || abs(pdgId) == 11 || abs(pdgId) == 15"))

        process.puppiNoLep = process.puppi.clone()
        process.puppiNoLep.candName = cms.InputTag('pfCandNoLep') 

        process.puppiMerged = cms.EDProducer("CandViewMerger",src = cms.VInputTag( 'puppiNoLep','pfCandLep'))

        process.puppiForMET = cms.EDProducer("PuppiPhoton",
                                             candName       = cms.InputTag('packedPFCandidates'),
                                             puppiCandName  = cms.InputTag('puppiMerged'),
                                             photonName     = cms.InputTag('slimmedPhotons'),
                                             photonId       = cms.InputTag("egmPhotonIDs:cutBasedPhotonID_PHYS14_PU20bx25_V2p1-standalone-loose"),
                                             pt             = cms.double(10),
                                             useRefs        = cms.bool(True),
                                             dRMatch        = cms.vdouble(10,10,10,10),
                                             pdgids         = cms.vint32 (22,11,211,130),
                                             weight         = cms.double(1.),
                                             useValueMap    = cms.bool(False),
                                             weightsName    = cms.InputTag('puppi'),
                                             )
        process.puppiForMET.puppiCandName    = 'puppiMerged'
        process.puppiSequence += process.puppi
        process.puppiSequence += process.pfCandNoLep
        process.puppiSequence += process.pfCandLep
        process.puppiSequence += process.puppiNoLep
        process.puppiSequence += process.puppiMerged
        process.puppiSequence += process.puppiForMET

        # recompute ak4 jets for corrections
        from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
        process.ak4PFJetsPuppi = ak4PFJets.clone(src=cms.InputTag('puppiNoLep'))
        process.puppiMetSequence += process.ak4PFJetsPuppi

        # compute puppi MET
        from RecoMET.METProducers.PFMET_cfi import pfMet
        process.pfMETPuppi = pfMet.clone()
        process.pfMETPuppi.src = cms.InputTag('puppiForMET')
        process.pfMETPuppi.calculateSignificance = False
        process.puppiMetSequence += process.pfMETPuppi

        # correct puppi jets
        jeclabel = 'DATA' if isData else 'MC'
        process.jec =  cms.ESSource("PoolDBESSource",
                            CondDBSetup,
                            toGet = cms.VPSet(
                      cms.PSet(record  = cms.string('JetCorrectionsRecord'),
                               tag     = cms.string('JetCorrectorParametersCollection_Fall15_25nsV2_'+jeclabel+'_AK4PFPuppi'),
                               label   = cms.untracked.string('AK4Puppi')
                               ),
                       cms.PSet(record  = cms.string('JetCorrectionsRecord'),
                                tag     = cms.string('JetCorrectorParametersCollection_Fall15_25nsV2_'+jeclabel+'_AK8PFPuppi'),
                                label   = cms.untracked.string('AK8Puppi')
                                ),
                      cms.PSet(record  = cms.string('JetCorrectionsRecord'),
                               tag     = cms.string('JetCorrectorParametersCollection_Fall15_25nsV2_'+jeclabel+'_AK4PFchs'),
                               label   = cms.untracked.string('AK4chs')
                               ),
                      cms.PSet(record  = cms.string('JetCorrectionsRecord'),
                               tag     = cms.string('JetCorrectorParametersCollection_Fall15_25nsV2_'+jeclabel+'_AK8PFchs'),
                               label   = cms.untracked.string('AK8chs')
                               ),
                      cms.PSet(record  = cms.string('JetCorrectionsRecord'),
                               tag     = cms.string('JetCorrectorParametersCollection_Fall15_25nsV2_'+jeclabel+'_AK4PF'),
                               label   = cms.untracked.string('AK4')
                               ),
                       cms.PSet(record  = cms.string('JetCorrectionsRecord'),
                                tag     = cms.string('JetCorrectorParametersCollection_Fall15_25nsV2_'+jeclabel+'_AK8PF'),
                                label   = cms.untracked.string('AK8')
                                )
                       ),

                )  
        if isData:
          #process.jec.connect = cms.string('sqlite:////'+cmssw_base+'/src/NeroProducer/Nero/test/jec/Fall15_25nsV2_DATA.db')
          process.jec.connect = cms.string('sqlite:jec/Fall15_25nsV2_DATA.db')
        else:
          process.jec.connect = cms.string('sqlite:jec/Fall15_25nsV2_MC.db')
        process.es_prefer_jec = cms.ESPrefer('PoolDBESSource', 'jec')

        from JetMETCorrections.Configuration.JetCorrectorsAllAlgos_cff  import *
        jetlabel='AK4PFPuppi'
        process.ak4PuppiL1  = ak4PFCHSL1FastjetCorrector.clone (algorithm = cms.string(jetlabel))
        process.ak4PuppiL2  = ak4PFCHSL2RelativeCorrector.clone(algorithm = cms.string(jetlabel))
        process.ak4PuppiL3  = ak4PFCHSL3AbsoluteCorrector.clone(algorithm = cms.string(jetlabel))
        process.ak4PuppiRes = ak4PFCHSResidualCorrector.clone  (algorithm = cms.string(jetlabel))
        process.puppiMetSequence += process.ak4PuppiL1
        process.puppiMetSequence += process.ak4PuppiL2
        process.puppiMetSequence += process.ak4PuppiL3

        process.ak4PuppiCorrector = ak4PFL1FastL2L3Corrector.clone(
                correctors = cms.VInputTag("ak4PuppiL1", 
                                            "ak4PuppiL2",
                                            "ak4PuppiL3")
            )
        process.ak4PuppiCorrectorRes = ak4PFL1FastL2L3Corrector.clone(
                correctors = cms.VInputTag("ak4PuppiL1", 
                                            "ak4PuppiL2",
                                            "ak4PuppiL3",
                                            'ak4PuppiRes')
            )
        if isData:
            process.puppiMetSequence += process.ak4PuppiRes
            process.puppiMetSequence += process.ak4PuppiCorrectorRes
            correctorLabel = 'ak4PuppiCorrectorRes'
        else:
            process.puppiMetSequence += process.ak4PuppiCorrector
            correctorLabel = 'ak4PuppiCorrector'

        # correct puppi MET
        process.puppiJetMETcorr = cms.EDProducer("PFJetMETcorrInputProducer",
            src = cms.InputTag('ak4PFJetsPuppi'),
            offsetCorrLabel = cms.InputTag('ak4PuppiL1'),
            jetCorrLabel = cms.InputTag(correctorLabel),
            jetCorrLabelRes = cms.InputTag('ak4PuppiCorrectorRes'),
            jetCorrEtaMax = cms.double(9.9),
            type1JetPtThreshold = cms.double(15.0),
            skipEM = cms.bool(True),
            skipEMfractionThreshold = cms.double(0.90),
            skipMuons = cms.bool(True),
            skipMuonSelection = cms.string("isGlobalMuon | isStandAloneMuon")
        )
        process.type1PuppiMET = cms.EDProducer("CorrectedPFMETProducer",
            src = cms.InputTag('pfMETPuppi'),
            applyType0Corrections = cms.bool(False),
            applyType1Corrections = cms.bool(True),
            srcCorrections = cms.VInputTag(cms.InputTag('puppiJetMETcorr', 'type1')),
            applyType2Corrections = cms.bool(False)
        )   
        process.puppiMetSequence += process.puppiJetMETcorr
        process.puppiMetSequence += process.type1PuppiMET

    from NeroProducer.Nero.makeFatJets_cff import *
    if process.nero.doAK8 or process.nero.doCA15:
        fatjetInitSequence = initFatJets(process,isData)
        process.jetSequence += fatjetInitSequence
    if process.nero.doAK8 and process.nero.doPuppi:
        ak8PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='AK',jetRadius=0.8)
        process.jetSequence += ak8PuppiSequence
    if process.nero.doCA15:
        if process.nero.doPuppi:
            ca15PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='CA',jetRadius=1.5)
            process.jetSequence += ca15PuppiSequence
        else:
            ca15CHSSequence = makeFatJets(process,isData=isData,pfCandidates='pfCHS',algoLabel='CA',jetRadius=1.5)
            process.jetSequence += ca15CHSSequence

# ------------------------QG-----------------------------------------------
# after jec, because need to be run on the corrected (latest) jet collection
qgDatabaseVersion = 'cmssw8020_v1'

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

##DEBUG
##print "Process=",process, process.__dict__.keys()
#------------------------------------------------------
process.p = cms.Path(
                process.infoProducerSequence *
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer *  ## ISO MAP FOR PHOTONS
                process.selectedElectrons *
                process.calibratedPatElectrons *
                process.calibratedPatPhotons *
                process.jecSequence *
                process.QGTagger    * ## after jec, because it will produce the new jet collection
                process.fullPatMetSequence *## no puppi
                process.puppiSequence *
                process.puppiMetSequence *
                process.jetSequence *
                process.BadPFMuonFilter *
                process.BadChargedCandidateFilter * 
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
