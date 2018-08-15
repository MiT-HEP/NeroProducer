from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call, check_output

import sys, os

### CHECK THAT CMS env and it is correct
pwd = os.environ['PWD']
if 'CMSSW_VERSION' not in os.environ:
    print "Do cmsenv!"
    exit(0)
version = os.environ['CMSSW_VERSION']
ok = False
for dir in reversed(pwd.split('/')):
    if version == dir : 
        ok = True
        break
if not ok:
    print "Do (redo) cmsenv (2) !"
    exit(0)


config = config()

config.General.requestName = 'Nero_request_XXX'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

## JobType
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'testNero.py'
config.JobType.pyCfgParams=['isGrid=True','isData=False','nerohead='+check_output("git rev-parse HEAD",shell=True), 'nerotag='+check_output('git describe --tags',shell=True)]

# request shipping of the JEC V4 -- local
#config.JobType.inputFiles=['jec/Summer15_50nsV4_DATA.db','jec/Summer15_50nsV4_MC.db']
config.JobType.inputFiles=['jec','qg','jer']

### DATA configuration
config.Data.inputDataset = '/HplusToTauNu-M500/amarini-amarini_PrivateMC_HPlusToTauNu_June2015-16aa19d591b8b49c55c4508e7a7c9233/USER'
#config.Data.inputDBS = 'phys03'
config.Data.inputDBS = 'global'
config.Data.ignoreLocality = False

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 10
config.Data.totalUnits = -1

tag = check_output("git describe --tags | cut -d'-' -f 1 | tr -d '\n' ",shell=True)
print "-> current tag is '"+tag + "'"
#config.Data.outLFNDirBase = '/store/user/%s/Nero/%s/' % (getUsernameFromSiteDB(), tag)
config.Data.outLFNDirBase = '/store/group/phys_higgs/cmshmm/%s/Nero/%s/' % (getUsernameFromSiteDB(), tag)
config.Data.publication = False
config.Data.outputDatasetTag ='NeroNtuples'
config.Data.allowNonValidInputDataset = True 

config.Site.storageSite = 'T2_CH_CERN'
#config.Site.blacklist = [ 'T2_US_Florida','T2_US_Vanderbilt']


if __name__ == '__main__':

    from CRABAPI.RawCommand import crabCommand
    from CRABClient.ClientExceptions import ClientException
    from httplib import HTTPException

    # We want to put all the CRAB project directories from the tasks we submit here into one common directory.
    # That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).
    config.General.workArea = 'NeroSubmission2'

    def submit(config):
        ### for some reason only the first dataset is submitted correctly, work around
        if len(sys.argv) ==1:
            ## book the command and run python
            cmd = "python " + sys.argv[0] + " '" + config.General.requestName + "'"
            print "calling: "+cmd
            call(cmd,shell=True)
            return
        if len(sys.argv) > 1:
            ## if it is not in the request try the next
            if sys.argv[1] !=  config.General.requestName: return
            ###
            print "--- Submitting " + "\033[01;32m" + config.Data.inputDataset.split('/')[1] + "\033[00m"  + " ---"
            config.Data.outputDatasetTag = config.General.requestName
            print "lumi-mask is",config.Data.lumiMask
            try:
                crabCommand('submit', config = config)
            except HTTPException as hte:
                print "Failed submitting task: %s" % (hte.headers)
            except ClientException as cle:
                print "Failed submitting task: %s" % (cle)

    def setdata(value="True"):
        if value=='True':
            config.Data.splitting = 'LumiBased'
            #config.Data.lumiMask=None
            #url = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/Final"
            #config.Data.lumiMask = url + "/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt"
            url = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/ReReco"
            #config.Data.lumiMask = url + "/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt"
            config.Data.lumiMask = url + "/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.txt"
        else:
            config.Data.lumiMask = None
            config.Data.splitting = 'FileBased'

        for idx,par in enumerate(config.JobType.pyCfgParams):
            if "isData" in par:
                config.JobType.pyCfgParams[idx] = "isData=" + value
        return 
            
    def submitDatasets(datasetNames):
        usedNames = {}
        for datasetName in datasetNames:
            basename = datasetName.split('/')[1]
            datasetNum = usedNames.get(basename, 0)
            config.General.requestName = '{0}_{1}'.format(basename,datasetNum)
            config.Data.inputDataset = datasetName
            submit(config)
            usedNames[basename] = datasetNum + 1

    #############################################################################################
    ## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
    #############################################################################################
    
    ###################################################
    ########            25ns  2017             ########
    ###################################################
    setdata("True")
    ###################################################
    config.Data.unitsPerJob = 100

    # /SingleMuon/Run2017B-31Mar2018-v1/MINIAOD
    #config.General.requestName = 'SingleMuon-Run2017B'
    #config.Data.inputDataset = '/SingleMuon/Run2017B-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'SingleMuon-Run2017C'
    #config.Data.inputDataset = '/SingleMuon/Run2017C-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'SingleMuon-Run2017D'
    #config.Data.inputDataset = '/SingleMuon/Run2017D-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'SingleMuon-Run2017E'
    #config.Data.inputDataset = '/SingleMuon/Run2017E-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'SingleMuon-Run2017F'
    #config.Data.inputDataset = '/SingleMuon/Run2017F-31Mar2018-v1/MINIAOD'
    #submit(config)

    ### LOW PU DATA
    #config.Data.unitsPerJob = 50
    #url = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/PromptReco"
    #config.Data.lumiMask = url + "/Cert_306896-307082_13TeV_PromptReco_Collisions17_JSON_LowPU.txt"
    #config.Data.outLFNDirBase = '/store/group/phys_higgs/cmshmm/%s/Nero/LowPU/%s/' % (getUsernameFromSiteDB(), tag)
    #config.General.requestName = 'SingleMuon-Run2017H'
    #config.Data.inputDataset = '/SingleMuon/Run2017H-17Nov2017-v2/MINIAOD'
    #submit(config)

    #config.General.requestName = 'ZeroBias-Run2017H'
    #config.Data.inputDataset = '/ZeroBias/Run2017H-17Nov2017-v1/MINIAOD'
    #submit(config)

    ####### SINGLE ELECTRON
    # /SingleElectron/Run2017B-31Mar2018-v1/MINIAOD
    config.General.requestName = 'SingleElectron-Run2017B'
    config.Data.inputDataset = '/SingleElectron/Run2017B-31Mar2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleElectron-Run2017C'
    config.Data.inputDataset = '/SingleElectron/Run2017C-31Mar2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleElectron-Run2017D'
    config.Data.inputDataset = '/SingleElectron/Run2017D-31Mar2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleElectron-Run2017E'
    config.Data.inputDataset = '/SingleElectron/Run2017E-31Mar2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleElectron-Run2017F'
    config.Data.inputDataset = '/SingleElectron/Run2017F-31Mar2018-v1/MINIAOD'
    submit(config)

    ####################################### DOUBLE MUON
    # /DoubleMuon/Run2017B-31Mar2018-v1/MINIAOD
    #config.General.requestName = 'DoubleMuon-Run2017B'
    #config.Data.inputDataset = '/DoubleMuon/Run2017B-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'DoubleMuon-Run2017C'
    #config.Data.inputDataset = '/DoubleMuon/Run2017C-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'DoubleMuon-Run2017D'
    #config.Data.inputDataset = '/DoubleMuon/Run2017D-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'DoubleMuon-Run2017E'
    #config.Data.inputDataset = '/DoubleMuon/Run2017E-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'DoubleMuon-Run2017F'
    #config.Data.inputDataset = '/DoubleMuon/Run2017F-31Mar2018-v1/MINIAOD'
    #submit(config)

    ###################################################
    setdata("True")

    #config.Data.unitsPerJob = 50
    #config.General.requestName = 'JetHT-Run2017B'
    #config.Data.inputDataset = '/JetHT/Run2017B-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'JetHT-Run2017C'
    #config.Data.inputDataset = '/JetHT/Run2017C-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'JetHT-Run2017D'
    #config.Data.inputDataset = '/JetHT/Run2017D-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'JetHT-Run2017E'
    #config.Data.inputDataset = '/JetHT/Run2017E-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'JetHT-Run2017F'
    #config.Data.inputDataset = '/JetHT/Run2017F-31Mar2018-v1/MINIAOD'
    #submit(config)


    #config.General.requestName = 'ZeroBias-Run2017B'
    #config.Data.inputDataset = '/ZeroBias/Run2017B-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'ZeroBias-Run2017C'
    #config.Data.inputDataset = '/ZeroBias/Run2017C-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'ZeroBias-Run2017D'
    #config.Data.inputDataset = '/ZeroBias/Run2017D-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'ZeroBias-Run2017E'
    #config.Data.inputDataset = '/ZeroBias/Run2017E-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'ZeroBias-Run2017F'
    #config.Data.inputDataset = '/ZeroBias/Run2017F-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'Charmonium-Run2017B'
    #config.Data.inputDataset = '/Charmonium/Run2017B-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'Charmonium-Run2017C'
    #config.Data.inputDataset = '/Charmonium/Run2017C-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'Charmonium-Run2017D'
    #config.Data.inputDataset = '/Charmonium/Run2017D-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'Charmonium-Run2017E'
    #config.Data.inputDataset = '/Charmonium/Run2017E-31Mar2018-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'Charmonium-Run2017F'
    #config.Data.inputDataset = '/Charmonium/Run2017F-31Mar2018-v1/MINIAOD'
    #submit(config)

    #####################################################
    ##setdata("False")
    #####################################################

    ##config.General.requestName = 'WZ-pythia8'
    ##config.Data.inputDataset = '/WZ_TuneCUETP8M1_13TeV-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    ##submit(config)

    ####################################
    ##config.Data.unitsPerJob = 10
    ####################################

    ##config.General.requestName = 'QCD-15-30'
    ##config.Data.inputDataset = '/QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    ##submit(config)

    ###################################################
    setdata("False")
    ###################################################

    ##################################
    config.Data.unitsPerJob = 1
    ##################################
    #config.JobType.pyCfgParams.append('isParticleGun=True')
    #config.General.requestName = 'JPsiToMuMu'
    #config.Data.inputDataset = '/JPsiToMuMu_Pt20to100-pythia8-gun/RunIIFall17MiniAODv2-PU2017RECOSIMstep_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TTTo2L2Nu'
    #config.Data.inputDataset = '/TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'ST_s-channel'
    #config.Data.inputDataset = '/ST_s-channel_4f_leptonDecays_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TTZToQQ'
    #config.Data.inputDataset = '/TTZToQQ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WWTo1L1Nu2Q'
    #config.Data.inputDataset = '/WWTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WWTo2L2Nu'
    #config.Data.inputDataset = '/WWTo2L2Nu_NNPDF31_TuneCP5_PSweights_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14_ext1-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WWW'
    #config.Data.inputDataset = '/WWW_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WWZ'
    #config.Data.inputDataset = '/WWZ_4F_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WZTo2L2Q'
    #config.Data.inputDataset = '/WZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WZTo3LNu'
    #config.Data.inputDataset = '/WZTo3LNu_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WZZ'
    #config.Data.inputDataset = '/WZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'ZZTo2L2Q'
    #config.Data.inputDataset = '/ZZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'ZZT24L'
    #config.Data.inputDataset = '/ZZTo4L_13TeV_powheg_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14_ext1-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'ZZZ'
    #config.Data.inputDataset = '/ZZZ_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TTZ'
    #config.Data.inputDataset = '/TTZToLLNuNu_M-10_TuneCP5_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TTW'
    #config.Data.inputDataset = '/TTWJetsToLNu_TuneCP5_PSweights_13TeV-amcatnloFXFX-madspin-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TTTT'
    #config.Data.inputDataset = '/TTTT_TuneCP5_PSweights_13TeV-amcatnlo-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)


    #config.General.requestName = 'DYJetsToLL'
    #config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14_ext1-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'DYJetsToLL2'
    #config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCP5_13TeV-amcatnloFXFX-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'QCD-15-700-hpp'
    #config.Data.inputDataset = '/QCD_Pt-15to7000_TuneCUETHS1_Flat_13TeV_herwigpp/RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'QCD-HT-100-200'
    #config.Data.inputDataset = '/QCD_HT100to200_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'QCD-HT-200-300'
    #config.Data.inputDataset = '/QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'QCD-HT-300-500'
    #config.Data.inputDataset = '/QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'QCD-HT-500-700'
    #config.Data.inputDataset = '/QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAOD-94X_mc2017_realistic_v10-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'QCD-HT-700-1000'
    #config.Data.inputDataset = '/QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)

    #config.Data.inputDBS = 'phys03'
    #config.General.requestName = 'VBFHHTo2B2Mu_CV_1_C2V_1_C3_1_13TeV-madgraph'
    #config.Data.inputDataset = '/VBFHHTo2B2Mu_CV_1_C2V_1_C3_1_13TeV-madgraph/amarini-Fall17_94X_Apr2018_MINIAOD-18783c0a07109245951450a1a4f55409/USER'
    #submit(config)

    #config.Data.inputDBS = 'phys03'
    #config.General.requestName = 'GluGluToHHTo2B2M_node_4_13TeV-madgraph'
    #config.Data.inputDataset = '/GluGluToHHTo2B2G_node_4_13TeV-madgraph/amarini-Fall17_94X_Apr2018_MINIAOD-18783c0a07109245951450a1a4f55409/USER'
    #submit(config)

    #config.Data.inputDBS = 'phys03'
    #config.Data.unitsPerJob = 10
    #config.General.requestName = 'VBFH_HToMuMu_M125_13TeV_powheg_pythia8'
    #config.Data.inputDataset = '/VBFH_HToMuMu_M125_13TeV_powheg_pythia8/amarini-Fall17_94X-MINIAODSIM-18783c0a07109245951450a1a4f55409/USER'
    #submit(config)

    #### HMM
    ##for mh in [120,130]:
    ##    config.General.requestName = 'ttH_%d'%mh
    ##    config.Data.inputDataset = '/ttHToMuMu_M%d_TuneCP5_13TeV-powheg-pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3/MINIAODSIM'%mh
    ##    submit(config)

    ##    config.General.requestName = 'ZH_%d'%mh
    ##    config.Data.inputDataset = '/ZH_HToMuMu_ZToAll_M%d_13TeV_powheg_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'%mh
    ##    submit(config)

    ##    config.General.requestName = 'WPlusH_%d'%mh
    ##    config.Data.inputDataset = '/WplusH_HToMuMu_WToAll_M%d_13TeV_powheg_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v3/MINIAODSIM'%mh
    ##    submit(config)
    ##    
    ##    config.General.requestName = 'WMinusH_%d'%mh
    ##    config.Data.inputDataset = '/WminusH_HToMuMu_WToAll_M%d_13TeV_powheg_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'%mh
    ##    submit(config)
    ##    
    ##    config.General.requestName = 'GluGluH_%d'%mh
    ##    config.Data.inputDataset = '/GluGluHToMuMu_M%d_13TeV_amcatnloFXFX_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'%mh
    ##    submit(config)
    ##    
    ##    config.General.requestName = 'VBFH_%d'%mh
    ##    config.Data.inputDataset = '/VBFHToMuMu_M%d_13TeV_amcatnlo_pythia8/RunIIFall17MiniAODv2-PU2017_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'%mh
    ##    submit(config)
        


# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
