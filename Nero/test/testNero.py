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
       '/store/data/Run2016B/MET/MINIAOD/PromptReco-v2/000/273/150/00000/2CF02CDC-D819-E611-AA68-02163E011A52.root'
       #'/store/data/Run2016B/SingleMuon/MINIAOD/PromptReco-v2/000/273/409/00000/16132799-721B-E611-BDDA-02163E014231.root'
       #'/store/data/Run2015D/MET/MINIAOD/16Dec2015-v1/50000/00EA1DB2-90AA-E511-AEEE-0025905C2CE6.root'
       #'/store/data/Run2015D/DoubleMuon/MINIAOD/16Dec2015-v1/10000/000913F7-E9A7-E511-A286-003048FFD79C.root'
       ]
else:
   fileList = [
       "/store/mc/RunIISpring16MiniAODv2/TTbarDMJets_pseudoscalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUSpring16RAWAODSIM_80X_mcRun2_asymptotic_2016_miniAODv2_v0_ext1-v1/40000/C06A61EE-EF25-E611-870A-02163E011A12.root"
       #"/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/626CD584-6AF3-E511-986F-001E67DDBEDA.root",
       #"/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/6C339CAD-54F3-E511-8BD4-90B11C12E856.root",
       #"/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/704816A7-54F3-E511-802A-001E67A3ED40.root",
       #"/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7612ABC7-60F3-E511-9AFA-001E67A3F49D.root",
       #"/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7A9FA1C7-5BF3-E511-93D7-001E67A3EC05.root",
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
        process.GlobalTag.globaltag = '80X_dataRun2_Prompt_v8'
else:
        process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_miniAODv2'

### LOAD DATABASE
from CondCore.DBCommon.CondDBSetup_cfi import *
#from CondCore.CondDB.CondDB_cfi import *

######## LUMI MASK
if isData and not options.isGrid and False: ## dont load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    ## SILVER
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt').getVLuminosityBlockRange()
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/DCSOnly/json_DCSONLY.txt').getVLuminosityBlockRange()

## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')
#process.load('NeroProducer.Nero.NeroMonojet_cfi')
#process.load('NeroProducer.Nero.NeroMonotop_cfi')
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
process.jecSequence = cms.Sequence( process.patJetCorrFactorsUpdatedJEC* process.updatedPatJetsUpdatedJEC* process.patJetCorrFactorsUpdatedJECAK8* process.updatedPatJetsUpdatedJECAK8)

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
qgDatabaseVersion = '80X'

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
                process.jecSequence *
                process.QGTagger    * ## after jec, because it will produce the new jet collection
                process.fullPatMetSequence *## no puppi
                process.puppiSequence *
                process.puppiMetSequence *
                process.jetSequence *
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
