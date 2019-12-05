from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call, check_output

import sys, os, re

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
#config.JobType.psetName = 'testNero_twomu.py'
config.JobType.pyCfgParams=['isGrid=True','isData=False','nerohead='+check_output("git rev-parse HEAD",shell=True), 'nerotag='+check_output('git describe --tags',shell=True),'isRun=2016B']

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
config.Data.outLFNDirBase = '/store/group/phys_higgs/cmshmm/%s/Nero/%s/year2016/' % (getUsernameFromSiteDB(), tag)
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
    config.General.workArea = 'NeroSubmission2016_v2'

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
            if '*' in config.Data.inputDataset:
                print "-> Figuring out dataset from *"
                cmd="dasgoclient -query='%s'"%config.Data.inputDataset
                print "Running cmd '"+cmd+"'"
                dataset=check_output(cmd,shell=True).split('\n')[0].split()[0]
                config.Data.inputDataset=dataset
            #config.Data.inputDataset = '/SingleMuon/Run2017B-31Mar2018-v1/MINIAOD'
            if 'Run2017' in config.Data.inputDataset or 'Run2016' in config.Data.inputDataset or 'Run2018' in config.Data.inputDataset:
                run=config.Data.inputDataset.split('/')[2].split('-')[0]
                setrun(run)
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
            url = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/"
            #config.Data.lumiMask = url + "/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt"
            config.Data.lumiMask = url + "Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
        else:
            config.Data.lumiMask = None
            config.Data.splitting = 'FileBased'

        for idx,par in enumerate(config.JobType.pyCfgParams):
            if "isData" in par:
                config.JobType.pyCfgParams[idx] = "isData=" + value
        return 

    def setrun(value='Run2017B'):
        print "-> Setting run Option to:", value
        for idx,par in enumerate(config.JobType.pyCfgParams):
            if "isRun" in par:
                config.JobType.pyCfgParams[idx] = "isRun=" + value
            
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

    config.General.requestName = 'SingleMuon-Run2016B_ver1'
    config.Data.inputDataset = '/SingleMuon/Run2016B-17Jul2018_ver1-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2016B_ver2'
    config.Data.inputDataset = '/SingleMuon/Run2016B-17Jul2018_ver2-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2016C'
    config.Data.inputDataset = '/SingleMuon/Run2016C-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2016D'
    config.Data.inputDataset = '/SingleMuon/Run2016D-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2016E'
    config.Data.inputDataset = '/SingleMuon/Run2016E-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2016F'
    config.Data.inputDataset = '/SingleMuon/Run2016F-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2016G'
    config.Data.inputDataset = '/SingleMuon/Run2016G-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2016H'
    config.Data.inputDataset = '/SingleMuon/Run2016H-17Jul2018-v1/MINIAOD'
    submit(config)

    ######################################## DOUBLE MUON

    config.General.requestName = 'DoubleMuon-Run2016B_ver1'
    config.Data.inputDataset = '/DoubleMuon/Run2016B-17Jul2018_ver1-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'DoubleMuon-Run2016B_ver2'
    config.Data.inputDataset = '/DoubleMuon/Run2016B-17Jul2018_ver2-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'DoubleMuon-Run2016C'
    config.Data.inputDataset = '/DoubleMuon/Run2016C-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'DoubleMuon-Run2016D'
    config.Data.inputDataset = '/DoubleMuon/Run2016D-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'DoubleMuon-Run2016E'
    config.Data.inputDataset = '/DoubleMuon/Run2016E-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'DoubleMuon-Run2016F'
    config.Data.inputDataset = '/DoubleMuon/Run2016F-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'DoubleMuon-Run2016G'
    config.Data.inputDataset = '/DoubleMuon/Run2016G-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'DoubleMuon-Run2016H'
    config.Data.inputDataset = '/DoubleMuon/Run2016H-17Jul2018-v1/MINIAOD'
    submit(config)

    ##### Zero Bias
    config.General.requestName = 'ZeroBias-Run2016B_ver1'
    config.Data.inputDataset = '/ZeroBias/Run2016B-17Jul2018_ver1-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'ZeroBias-Run2016B_ver2'
    config.Data.inputDataset = '/ZeroBias/Run2016B-17Jul2018_ver2-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'ZeroBias-Run2016C'
    config.Data.inputDataset = '/ZeroBias/Run2016C-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'ZeroBias-Run2016D'
    config.Data.inputDataset = '/ZeroBias/Run2016D-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'ZeroBias-Run2016E'
    config.Data.inputDataset = '/ZeroBias/Run2016E-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'ZeroBias-Run2016F'
    config.Data.inputDataset = '/ZeroBias/Run2016F-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'ZeroBias-Run2016G'
    config.Data.inputDataset = '/ZeroBias/Run2016G-17Jul2018-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'ZeroBias-Run2016H'
    config.Data.inputDataset = '/ZeroBias/Run2016H-17Jul2018-v1/MINIAOD'
    submit(config)

    ###################################################

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
    setrun("Run2016B")
    ###################################################

    ##################################
    config.Data.unitsPerJob = 1
    ##################################
    #$dasgoclient -query='dataset=/TT*/*12Apr2018_94X*/MINIAODSIM' 
    #config.JobType.pyCfgParams.append('isParticleGun=True')
    #config.General.requestName = 'JPsiToMuMu'
    #config.Data.inputDataset = '/JPsiToMuMu_Pt20to100-pythia8-gun/RunIIFall17MiniAODv2-PU2017RECOSIMstep_12Apr2018_94X_mc2017_realistic_v14-v1/MINIAODSIM'
    #submit(config)


    #### HMM
    for mh in [120,125,130]:
    #for mh in [120]:
        config.General.requestName = 'ttH_%d'%mh
        config.Data.inputDataset = '/ttHToMuMu_M%d_TuneCP5_PSweights_13TeV-powheg-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'%mh
        submit(config)

        config.General.requestName = 'ZH_%d'%mh
        config.Data.inputDataset = '/ZH_HToMuMu_ZToAll_M%d_TuneCP5_PSweights_13TeV_powheg_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'%mh
        submit(config)

        config.General.requestName = 'WPlusH_%d'%mh
        config.Data.inputDataset='/WplusH_HToMuMu_WToAll_M%d_TuneCP5_PSweights_13TeV_powheg_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'%mh
        submit(config)
        
        config.General.requestName = 'WMinusH_%d'%mh
        config.Data.inputDataset="/WminusH_HToMuMu_WToAll_M%d_TuneCP5_PSweights_13TeV_powheg_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM"%mh
        submit(config)
        
        config.General.requestName = 'GluGluH_%d'%mh
        config.Data.inputDataset = '/GluGluHToMuMu_M%d_TuneCP5_PSweights_13TeV_amcatnloFXFX_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'%mh
        submit(config)
        #/GluGluHToMuMu_M-125_TuneCP5_PSweights_13TeV_powheg_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM
        
        config.General.requestName = 'VBFH_%d'%mh
        config.Data.inputDataset = '/VBFHToMuMu_M%d_TuneCP5_PSweights_13TeV_amcatnlo_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'%mh
        submit(config)
        #/VBFHToMuMu_M-125_TuneCP5_PSweights_13TeV_powheg_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM

    config.General.requestName = 'bbH_HToMuMu_M125_amcatnlo-pythia8'
    config.Data.inputDataset = '/bbHToMuMu_M-125_4FS_yb2_TuneCP5_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    #config.Data.inputDBS = 'phys03'
    #config.Data.unitsPerJob = 10
    #config.General.requestName = 'VBFH_HToMuMu_M125_13TeV_powheg_pythia8'
    #config.Data.inputDataset = '/VBFH_HToMuMu_M125_13TeV_powheg_pythia8/amarini-Fall17_94X-MINIAODSIM-18783c0a07109245951450a1a4f55409/USER'
    #submit(config)

    #config.Data.inputDBS = 'phys03'
    #config.Data.unitsPerJob = 10
    #config.General.requestName = 'GluGluH_HToMuMu_M125_13TeV_powheg_pythia8'
    #config.Data.inputDataset = '/GluGlu_HToMuMu_M125_13TeV_powheg_pythia8/amarini-Fall17_94X-MINIAODSIM-18783c0a07109245951450a1a4f55409/USER'
    #submit(config)
    #config.Data.inputDBS = 'global'

    config.General.requestName = 'TTTo2L2Nu'
    config.Data.inputDataset = '/TTTo2L2Nu_TuneCUETP8M2_ttHtranche3_13TeV-powheg-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_s-channel'
    config.Data.inputDataset = '/ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_t-channel_antitop'
    config.Data.inputDataset = '/ST_t-channel_antitop_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_t-channel_top'
    config.Data.inputDataset = '/ST_t-channel_top_4f_inclusiveDecays_13TeV-powhegV2-madspin-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_tW_antitop'
    config.Data.inputDataset = '/ST_tW_antitop_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_tW_top'
    config.Data.inputDataset = '/ST_tW_top_5f_inclusiveDecays_13TeV-powheg-pythia8_TuneCUETP8M1/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTZToQQ'
    config.Data.inputDataset = '/TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTZToLL_M-1to10'
    config.Data.inputDataset = '/TTZToLL_M-1to10_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTZToLLNuNu_M-10'
    config.Data.inputDataset = '/TTZToLLNuNu_M-10_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTWToQQ'
    config.Data.inputDataset = '/TTWJetsToQQ_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTWToLNu'
    config.Data.inputDataset = '/TTWJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-madspin-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WWTo1L1Nu2Q'
    config.Data.inputDataset = '/WWTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WWTo2L2Nu'
    config.Data.inputDataset = '/WWTo2L2Nu_13TeV-powheg/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WWW'
    config.Data.inputDataset = '/WWW_4F_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WWZ'
    config.Data.inputDataset = '/WWZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZTo2L2Q'
    config.Data.inputDataset = '/WZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZTo3LNu'
    config.Data.inputDataset = '/WZTo3LNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZZ'
    config.Data.inputDataset = '/WZZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZTo2L2Q'
    config.Data.inputDataset = '/ZZTo2L2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZTo4L'
    config.Data.inputDataset = '/ZZTo4L_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM'
    submit(config)


    config.General.requestName = 'TTTT'
    config.Data.inputDataset = '/TTTT_TuneCUETP8M2T4_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'DYJetsToLL'
    config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext2-v1/MINIAODSIM'
    submit(config)

    
    config.General.requestName = 'DYJetsToMuMu_105_160'
    config.Data.inputDataset='/DYJetsToLL_M-105To160_TuneCP5_PSweights_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'DYJetsToMuMu_105_160_ext'
    config.Data.inputDataset = '/DYJetsToLL_M-105To160_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v1/MINIAODSIM'
    submit(config)

    htbins=['50','100','200','300','500','700','1000','1500','2000','Inf']
    for idx in range(0,len(htbins)-1):
        config.General.requestName = 'QCD-HT-%s-%s'%(htbins[idx],htbins[idx+1])
        config.Data.inputDataset = '/QCD_HT%sto%s_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v*/MINIAODSIM'%(htbins[idx],htbins[idx+1])
        submit(config)

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

    config.General.requestName = 'GGZZ2e2mu'
    config.Data.inputDataset = '/GluGluToContinToZZTo2e2mu_13TeV_MCFM701_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GGZZ2mu2tau'
    config.Data.inputDataset = '/GluGluToContinToZZTo2mu2tau_13TeV_MCFM701_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GGZZ2e2tau'
    config.Data.inputDataset = '/GluGluToContinToZZTo2e2tau_13TeV_MCFM701_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GGZZ4mu'
    config.Data.inputDataset = '/GluGluToContinToZZTo4mu_13TeV_MCFM701_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)
        
    config.General.requestName = 'GGZZ4tau'
    config.Data.inputDataset = '/GluGluToContinToZZTo4tau_13TeV_MCFM701_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GGZZ2mu2nu'
    config.Data.inputDataset = '/GluGluToContinToZZTo2mu2nu_13TeV_MCFM701_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-herwigpp'
    config.Data.inputDataset= '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-herwigpp_30M/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    #----- wz
    config.General.requestName = 'WZJJ_EWK_13TeV-madgraph-pythia8'
    config.Data.inputDataset= '/WZJJ_EWK_13TeV-madgraph-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZJJ_QCD_13TeV-madgraph-pythia8'
    config.Data.inputDataset= '/WZJJ_QCD_13TeV-madgraph-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZJJ_EWK_QCD_13TeV-madgraph-pythia8'
    config.Data.inputDataset= '/WZJJ_EWK_QCD_13TeV-madgraph-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    #----- SS ww
    config.General.requestName = 'WpWpJJ_EWK_TuneCUETP8M1_13TeV-powheg-pythia8'
    config.Data.inputDataset= '/WpWpJJ_EWK_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WpWpJJ_EWK_TuneCUETP8M1_13TeV-madgraph-pythia8'
    config.Data.inputDataset= '/WpWpJJ_EWK_TuneCUETP8M1_13TeV-madgraph-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WpWpJJ_QCD_TuneCUETP8M1_13TeV-madgraph-pythia8'
    config.Data.inputDataset= '/WpWpJJ_QCD_TuneCUETP8M1_13TeV-madgraph-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WpWpJJ_EWK-QCD_TuneCUETP8M1_13TeV-madgraph-pythia8'
    config.Data.inputDataset= '/WpWpJJ_EWK-QCD_TuneCUETP8M1_13TeV-madgraph-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WmWmJJ_EWK_TuneCUETP8M1_13TeV-vbfnlo-pythia8'
    config.Data.inputDataset= '/WmWmJJ_EWK_TuneCUETP8M1_13TeV-vbfnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WmWmJJ_EWK_TuneCUETP8M1_13TeV-powheg-pythia8'
    config.Data.inputDataset= '/WmWmJJ_EWK_TuneCUETP8M1_13TeV-powheg-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)
   
    config.Data.inputDBS = 'phys03'
    config.General.requestName = 'WPhadWPhadJJ_EWK_TuneCUETP8M1_13TeV-powheg-pythia8'
    config.Data.inputDataset='/WPhadWPhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8/amarini-FullSim_94X-MINIAODSIM-bd3e7bcff6c9bcad356ea4ed7e4f08b4/USER'
    submit(config)
    config.Data.inputDBS = 'global'

    config.Data.inputDBS = 'phys03'
    config.General.requestName = 'ZbbZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8'
    config.Data.inputDataset='/ZbbZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8/amarini-FullSim_94X-MINIAODSIM-bd3e7bcff6c9bcad356ea4ed7e4f08b4/USER'
    submit(config)
    config.Data.inputDBS = 'global'

    config.Data.inputDBS = 'phys03'
    config.General.requestName = 'ZnnZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8'
    config.Data.inputDataset='/ZnnZhadJJ_EWK_LO_SM_mjj100_pTj10_13TeV_madgraphMLM_pythia8/amarini-FullSim_94X-MINIAODSIM-bd3e7bcff6c9bcad356ea4ed7e4f08b4/USER'
    submit(config)
    config.Data.inputDBS = 'global'

    config.General.requestName = 'TT'
    config.Data.inputDataset = '/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZZ'
    config.Data.inputDataset = '/ZZZ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-2500ToInf_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-1200To2500_13TeV-madgraph_ext'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-1200To2500_13TeV-madgraph'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-1200To2500_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-800To1200_13TeV-madgraph'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-800To1200_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-600To800_13TeV-madgraph'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-600To800_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-400To600_13TeV-madgraph_ext'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-400To600_13TeV-madgraph'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-200To400_13TeV-madgraph_ext'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-200To400_13TeV-madgraph'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-100To200_13TeV-madgraph_ext'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3_ext1-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-100To200_13TeV-madgraph'
    config.Data.inputDataset = '/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ttZJets_13TeV_madgraphMLM-pythia8'
    config.Data.inputDataset = '/ttZJets_13TeV_madgraphMLM-pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WWTo4Q_4f_13TeV_amcatnloFXFX_madspin_pythia8'
    config.Data.inputDataset = '/WWTo4Q_4f_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WWTo4Q_13TeV-powheg'
    config.Data.inputDataset = '/WWTo4Q_13TeV-powheg/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZTo4Q_13TeV_amcatnloFXFX_madspin_pythia8'
    config.Data.inputDataset = '/ZZTo4Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZTo2Q2Nu_13TeV_powheg_pythia8'
    config.Data.inputDataset = '/ZZTo2Q2Nu_13TeV_powheg_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZTo2Q2Nu_13TeV_amcatnloFXFX_madspin_pythia8'
    config.Data.inputDataset = '/ZZTo2Q2Nu_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZToLNu2Q_13TeV_powheg_pythia8'
    config.Data.inputDataset = '/WZToLNu2Q_13TeV_powheg_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8'
    config.Data.inputDataset = '/WZTo1L1Nu2Q_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZTo2Q2Nu_13TeV_amcatnloFXFX_madspin_pythia8'
    config.Data.inputDataset = '/WZTo2Q2Nu_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZTo1L3Nu_13TeV_amcatnloFXFX_madspin_pythia8'
    config.Data.inputDataset = '/WZTo1L3Nu_13TeV_amcatnloFXFX_madspin_pythia8/RunIISummer16MiniAODv3-PUMoriond17_94X_mcRun2_asymptotic_v3-v2/MINIAODSIM'
    submit(config)


# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
