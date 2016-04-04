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

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

fileList = [
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/626CD584-6AF3-E511-986F-001E67DDBEDA.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/6C339CAD-54F3-E511-8BD4-90B11C12E856.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/704816A7-54F3-E511-802A-001E67A3ED40.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7612ABC7-60F3-E511-9AFA-001E67A3F49D.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7A9FA1C7-5BF3-E511-93D7-001E67A3EC05.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7C15B37C-9FF3-E511-820C-0025905A60AA.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/7C3FC040-86F3-E511-AEFD-0025905A48F2.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/80A2B9C1-68F3-E511-86F5-0CC47A4D76B2.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/88AFBC35-55F3-E511-9AC8-001E67DFF61D.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/8A3A0176-6BF3-E511-AA0F-0025905B856E.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/8E0358CB-5BF3-E511-9DF4-90B11C08CA45.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/8EB68FCE-54F3-E511-8A6E-001E675A4759.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/8EE1F3B5-79F3-E511-B5E8-90B11C12E856.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/962C9EC9-54F3-E511-916D-001E67A3E8CC.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/A2F67BC0-68F3-E511-BFE2-0CC47A4D7694.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/A63063C6-5BF3-E511-A910-001E67DDC4CF.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/BC8BF52B-7AF3-E511-9691-90B11C050429.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/C84089C8-60F3-E511-82BA-001E67A3E8F9.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/C88B363C-86F3-E511-B855-0CC47A4C8EE2.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/CEEA1766-5CF3-E511-A048-90B11C08CDB7.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/D2555C49-66F3-E511-ABB8-001E675A6928.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/D402B7CE-68F3-E511-8AEB-001E67A3EB1A.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/D43B274B-66F3-E511-8793-001E675A6725.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/DEA5F70D-A0F3-E511-BA38-001E67DDC88A.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/DEAD90A4-54F3-E511-B9A0-001E67A3EA89.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/ECBAD108-7BF3-E511-B4AF-003048FFD7A4.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/FA0718CF-54F3-E511-8EE5-001E67DDC051.root",
        "/store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/FE1376C8-60F3-E511-8333-001E675A68C4.root",
        
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
# ------------------------QG-----------------------------------------------
qgDatabaseVersion = '76X'

# to use the database, see https://twiki.cern.ch/twiki/bin/view/CMS/QuarkGluonLikelihood
#connect = cms.string('frontier://FrontierProd/CMS_COND_PAT_000'),
#for type in ['AK4PFchs','AK4PFchs_antib']:
#  QGPoolDBESSource.toGet.extend(cms.VPSet(cms.PSet(
#    record = cms.string('QGLikelihoodRcd'),
#    tag    = cms.string('QGLikelihoodObject_'+qgDatabaseVersion+'_'+type),
#    label  = cms.untracked.string('QGL_'+type)
#  )))

from CondCore.DBCommon.CondDBSetup_cfi import *
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
process.QGTagger.srcJets             = cms.InputTag("slimmedJets")    # Could be reco::PFJetCollection or pat::JetCollection (both AOD and miniAOD)               
process.QGTagger.srcVertexCollection = cms.InputTag("offlineSlimmedPrimaryVertices")
process.QGTagger.useQualityCuts = cms.bool(False)
##----------------GLOBAL TAG ---------------------------
# used by photon id and jets
process.load("Configuration.Geometry.GeometryIdeal_cff") 
process.load('Configuration.StandardSequences.Services_cff')
process.load("Configuration.StandardSequences.MagneticField_cff")

#mc https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideFrontierConditions#Global_Tags_for_Run2_MC_Producti
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_condDBv2_cff')
if (isData):
    if options.is25ns:
        process.GlobalTag.globaltag = '76X_dataRun2_v15'
    if options.is50ns:
        process.GlobalTag.globaltag = '74X_dataRun2_Prompt_v1'
        print "FIX GLOBAL TAG"
else:
    #process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
    if options.is25ns:
        process.GlobalTag.globaltag = '80X_mcRun2_asymptotic_2016_v3'
        #process.GlobalTag.globaltag = '76X_mcRun2_asymptotic_v12'
        #process.GlobalTag.globaltag = '74X_mcRun2_asymptotic_v2'
    if options.is50ns:
        process.GlobalTag.globaltag = 'MCRUN2_74_V9A::All' ## FIXME
        print "FIX GLOBAL TAG"

######## LUMI MASK
if isData and not options.isGrid : ## don't load the lumiMaks, will be called by crab
    #pass
    import FWCore.PythonUtilities.LumiList as LumiList
    ## SILVER
    #process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt').getVLuminosityBlockRange()
    process.source.lumisToProcess = LumiList.LumiList(filename='/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver_v2.txt').getVLuminosityBlockRange()


## SKIM INFO
process.load('NeroProducer.Skim.infoProducerSequence_cff')
process.load('NeroProducer.Nero.Nero_cfi')
#process.load('NeroProducer.Nero.NeroMonotop_cfi')
#process.load('NeroProducer.Nero.NeroMonojet_cfi')
#process.load('NeroProducer.Nero.NeroChargedHiggs_cfi')



#----------------------PUPPI, MET, & CORRECTIONS-------------------
process.puppiSequence = cms.Sequence()
### puppi ###
process.load('CommonTools.PileupAlgos.Puppi_cff')
process.puppi.candName = cms.InputTag('packedPFCandidates')
process.puppi.vertexName = cms.InputTag('offlineSlimmedPrimaryVertices')
process.pfCandNoLep = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("abs(pdgId) != 13 && abs(pdgId) != 11 && abs(pdgId) != 15"))
process.pfCandLep   = cms.EDFilter("CandPtrSelector", src = cms.InputTag("packedPFCandidates"), cut = cms.string("abs(pdgId) == 13 || abs(pdgId) == 11 || abs(pdgId) == 15"))
process.puppinolep = process.puppi.clone()
process.puppinolep.candName = 'pfCandNoLep'
process.puppiSequence += process.puppi
process.puppiSequence += process.pfCandNoLep
process.puppiSequence += process.pfCandLep
process.puppiSequence += process.puppinolep

### MET ###
process.load('RecoMET.METProducers.PFMET_cfi')
process.puppiForMET = cms.EDProducer("CandViewMerger",src = cms.VInputTag( 'puppinolep','pfCandLep'))
process.puppiSequence += process.puppiForMET
process.pfMETPuppi = process.pfMet.clone();
process.pfMETPuppi.src = cms.InputTag('puppiForMET')
process.pfMETPuppi.calculateSignificance = False
process.puppiSequence += process.pfMETPuppi

### set up JEC ###
cmssw_base = os.environ['CMSSW_BASE']
if options.isData:
    connectString = cms.string('sqlite:jec/Summer15_25nsV6_DATA.db')
    tagName = 'Summer15_25nsV6_DATA_AK4PFPuppi'
else:
    connectString = cms.string('sqlite:jec/Summer15_25nsV6_MC.db')
    tagName = 'Summer15_25nsV6_MC_AK4PFPuppi'

process.jec = cms.ESSource("PoolDBESSource",
        DBParameters = cms.PSet(
            messageLevel = cms.untracked.int32(0)
            ),
        timetype = cms.string('runnumber'),
        toGet = cms.VPSet(
            cms.PSet(
                record = cms.string('JetCorrectionsRecord'),
                tag    = cms.string('JetCorrectorParametersCollection_%s'%tagName),
                label  = cms.untracked.string('AK4PFPuppi')
                ),
            ), 
        connect = connectString
        )
process.load('JetMETCorrections.Configuration.JetCorrectorsAllAlgos_cff')
puppilabel='PFPuppi'
process.ak4PuppiL1FastjetCorrector  = process.ak4PFCHSL1FastjetCorrector.clone (algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiL2RelativeCorrector = process.ak4PFCHSL2RelativeCorrector.clone(algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiL3AbsoluteCorrector = process.ak4PFCHSL3AbsoluteCorrector.clone(algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiResidualCorrector   = process.ak4PFCHSResidualCorrector.clone  (algorithm   = cms.string('AK4'+puppilabel))
process.ak4PuppiL1FastL2L3Corrector = process.ak4PFL1FastL2L3Corrector.clone(
        correctors = cms.VInputTag("ak4PuppiL1FastjetCorrector", "ak4PuppiL2RelativeCorrector", "ak4PuppiL3AbsoluteCorrector")
        )
process.ak4PuppiL1FastL2L3ResidualCorrector = process.ak4PFL1FastL2L3Corrector.clone(
        correctors = cms.VInputTag("ak4PuppiL1FastjetCorrector", "ak4PuppiL2RelativeCorrector", "ak4PuppiL3AbsoluteCorrector",'ak4PuppiResidualCorrector')
        )
process.ak4PuppiL1FastL2L3Chain = cms.Sequence(
        process.ak4PuppiL1FastjetCorrector * process.ak4PuppiL2RelativeCorrector * process.ak4PuppiL3AbsoluteCorrector * process.ak4PuppiL1FastL2L3Corrector
        )
process.ak4PuppiL1FastL2L3ResidualChain = cms.Sequence(
        process.ak4PuppiL1FastjetCorrector * process.ak4PuppiL2RelativeCorrector * process.ak4PuppiL3AbsoluteCorrector * process.ak4PuppiResidualCorrector * process.ak4PuppiL1FastL2L3ResidualCorrector
        )
if isData:
    process.puppiSequence += process.ak4PuppiL1FastL2L3ResidualChain
else:
    process.puppiSequence += process.ak4PuppiL1FastL2L3Chain

## in case of conflict, use jec instead of GlobalTag
process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')
### MET corrections ###
from RecoJets.JetProducers.ak4PFJets_cfi import ak4PFJets
process.AK4PFJetsPuppi = ak4PFJets.clone(
        src          = cms.InputTag('puppinolep'),
        jetAlgorithm = cms.string("AntiKt"),
        rParam       = cms.double(0.4),
        jetPtMin     = cms.double(20)
        )

process.puppiSequence += process.AK4PFJetsPuppi
process.puppiMETCorr = cms.EDProducer("PFJetMETcorrInputProducer",
        src = cms.InputTag('AK4PFJetsPuppi'),
        offsetCorrLabel = cms.InputTag("ak4PuppiL1FastjetCorrector"),
        jetCorrLabel = cms.InputTag("ak4PuppiL1FastL2L3Corrector"),
        jetCorrLabelRes = cms.InputTag("ak4PuppiL1FastL2L3ResidualCorrector"),
        jetCorrEtaMax = cms.double(9.9),
        type1JetPtThreshold = cms.double(15.0),
        skipEM = cms.bool(True),
        skipEMfractionThreshold = cms.double(0.90),
        skipMuons = cms.bool(True),
        skipMuonSelection = cms.string("isGlobalMuon | isStandAloneMuon")
        )
if isData:
    process.puppiMETCorr.jetCorrLabel = cms.InputTag("ak4PuppiL1FastL2L3ResidualCorrector")
process.puppiSequence += process.puppiMETCorr

process.type1PuppiMET = cms.EDProducer("CorrectedPFMETProducer",
        src = cms.InputTag('pfMETPuppi'),
        applyType0Corrections = cms.bool(False),
        applyType1Corrections = cms.bool(True),
        srcCorrections = cms.VInputTag(
            cms.InputTag('puppiMETCorr','type1')
            ),
        applyType2Corrections = cms.bool(False)
        )
process.puppiSequence += process.type1PuppiMET

#------------------------FATJETS--------------------------------

from NeroProducer.Nero.makeFatJets_cff import *
fatjetInitSequence = initFatJets(process,isData)
ak8PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='AK',jetRadius=0.8)
ca15CHSSequence = makeFatJets(process,isData=isData,pfCandidates='pfCHS',algoLabel='CA',jetRadius=1.5)
ca15PuppiSequence = makeFatJets(process,isData=isData,pfCandidates='puppiForMET',algoLabel='CA',jetRadius=1.5)
process.jetSequence = cms.Sequence(fatjetInitSequence*
                    ak8PuppiSequence*
                    ca15CHSSequence*
                    ca15PuppiSequence
                    )

process.reclusterSequence = cms.Sequence()
if process.nero.doReclustering: ## use python to figure it out
    process.reclusterSequence += process.puppiSequence 
    process.reclusterSequence += process.jetSequence 
#-----------------------ELECTRON ID-------------------------------
from NeroProducer.Nero.egammavid_cfi import *

initEGammaVID(process,options)

### ##ISO
process.load("RecoEgamma/PhotonIdentification/PhotonIDValueMapProducer_cfi")
process.load("RecoEgamma/ElectronIdentification/ElectronIDValueMapProducer_cfi")

############################### JEC #####################
#### Load from a sqlite db, if not read from the global tag
## process.load("CondCore.DBCommon.CondDBCommon_cfi")
## from CondCore.DBCommon.CondDBSetup_cfi import *
## 
## if options.isData:
## 	if options.is25ns:
## 		connectString = cms.string('sqlite:jec/Summer15_25nsV2_DATA.db')
## 		tagName = 'Summer15_25nsV2_DATA_AK4PFchs'
## 	if options.is50ns:
## 		connectString = cms.string('sqlite:jec/Summer15_50nsV5_DATA.db')
## 		tagName = 'Summer15_50nsV5_DATA_AK5PFchs'
## else:
## 	if options.is25ns:
## 		connectString = cms.string('sqlite:jec/Summer15_25nsV2_MC.db')
## 		tagName = 'Summer15_25nsV2_MC_AK5PFchs'
## 	if options.is50ns:
## 		connectString = cms.string('sqlite:jec/Summer15_50nsV5_MC.db')
## 		tagName = 'Summer15_50nsV5_MC_AK5PFchs'
## 
## process.jec = cms.ESSource("PoolDBESSource",
##       DBParameters = cms.PSet(
##         messageLevel = cms.untracked.int32(0)
##         ),
##       timetype = cms.string('runnumber'),
##       toGet = cms.VPSet(
##       cms.PSet(
##             record = cms.string('JetCorrectionsRecord'),
##             tag    = cms.string('JetCorrectorParametersCollection_%s'%tagName),
##             label  = cms.untracked.string('AK4PFchs')
##             ),
##       ## here you add as many jet types as you need
##       ## note that the tag name is specific for the particular sqlite file 
##       ), 
##       connect = connectString
##      # uncomment above tag lines and this comment to use MC JEC
## )
## ## add an es_prefer statement to resolve a possible conflict from simultaneous connection to a global tag
## process.es_prefer_jec = cms.ESPrefer('PoolDBESSource','jec')

################ end sqlite connection
#### BEGIN RECOMPUTE JEC ###
#from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetCorrFactorsUpdated
#from PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff import patJetsUpdated
## process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
## 
## jecLevels= ['L1FastJet',  'L2Relative', 'L3Absolute']
## 
## if options.isData:
## 	print "NO L2L3 Residual Applied so far. FIXME"
## 	#jecLevels.append( 'L2L3Residuals')
## 
## process.patJetCorrFactorsReapplyJEC = process.patJetCorrFactorsUpdated.clone(
## 		  src = cms.InputTag("slimmedJets"),
## 		  levels = jecLevels,
## 		  payload = 'AK4PFchs' ) # 
## 
## process.load("PhysicsTools.PatAlgos.producersLayer1.jetUpdater_cff")
## process.patJetsReapplyJEC = process.patJetsUpdated.clone(
## 		  jetSource = cms.InputTag("slimmedJets"),
## 		  jetCorrFactorsSource = cms.VInputTag(cms.InputTag("patJetCorrFactorsReapplyJEC"))
## 		  )
## 
## process.jecSequence = cms.Sequence( 
## 		process.patJetCorrFactorsReapplyJEC + 
## 		process. patJetsReapplyJEC 
## 		)
##___________________________HCAL_Noise_Filter________________________________||
process.load('CommonTools.RecoAlgos.HBHENoiseFilterResultProducer_cfi')
process.HBHENoiseFilterResultProducer.minZeros = cms.int32(99999)
process.HBHENoiseFilterResultProducer.IgnoreTS4TS5ifJetInLowBVRegion=cms.bool(False) 
process.HBHENoiseFilterResultProducer.defaultDecision = cms.string("HBHENoiseFilterResultRun2Loose")

process.ApplyBaselineHBHENoiseFilter = cms.EDFilter('BooleanFlagFilter',
        inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHENoiseFilterResult'),
        reverseDecision = cms.bool(False)
        )

process.ApplyBaselineHBHEIsoNoiseFilter = cms.EDFilter('BooleanFlagFilter',
        inputLabel = cms.InputTag('HBHENoiseFilterResultProducer','HBHEIsoNoiseFilterResult'),
        reverseDecision = cms.bool(False)
        )

process.hcalNoiseFilter = cms.Sequence(
        process.HBHENoiseFilterResultProducer* #produces HBHE baseline bools
        process.ApplyBaselineHBHENoiseFilter  #reject events based 
        #process.ApplyBaselineHBHEIsoNoiseFilter*   #reject events based  < 10e-3 mistake rate 
        )
###############################

if options.isGrid:
	process.nero.head=options.nerohead ##'git rev-parse HEAD'
	process.nero.tag=options.nerotag ## git describe --tags

if options.isParticleGun:
	process.nero.particleGun = cms.untracked.bool(True)
	## this option is for the embedding informations
	process.nero.extendEvent = cms.untracked.bool(False)

process.load('NeroProducer.Skim.MonoJetFilterSequence_cff')

#------------------------------------------------------
process.p = cms.Path(
                process.infoProducerSequence *
                #process.MonoJetFilter *
                #process.hcalNoiseFilter * 
                process.QGTagger *
                process.egmGsfElectronIDSequence *
                process.egmPhotonIDSequence *
                process.photonIDValueMapProducer * ## ISO MAP FOR PHOTONS
                process.electronIDValueMapProducer * ## ISO MAP FOR PHOTONS
                #process.puppiSequence * ## does puppi, puppi met, type1 corrections
                #process.jetSequence *
                process.reclusterSequence * ##  includes puppi and jets if recluster options is on
                #process.jecSequence *
                process.nero
                )

## DEBUG -- dump the event content with all the value maps ..
## process.schedule = cms.Schedule(
## 		process.p,
## 		process.output_step)

# Local Variables:
# mode:c++
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
