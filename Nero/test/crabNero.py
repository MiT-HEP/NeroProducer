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
config.JobType.pyCfgParams=['isGrid=True','isData=False','is25ns=True','is50ns=False','nerohead='+check_output("git rev-parse HEAD",shell=True), 'nerotag='+check_output('git describe --tags',shell=True)]

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
config.Data.outLFNDirBase = '/store/user/%s/Nero/%s/' % (getUsernameFromSiteDB(), tag)
config.Data.publication = False
config.Data.outputDatasetTag ='NeroNtuples'

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
            try:
                crabCommand('submit', config = config)
            except HTTPException as hte:
                print "Failed submitting task: %s" % (hte.headers)
            except ClientException as cle:
                print "Failed submitting task: %s" % (cle)

    def setdata(value="True",is25ns=False,year='2015'):
        if year=='2016' and value=='True':
            config.Data.splitting = 'FileBased'
            config.Data.lumiMask=None
        elif value == "True":
            url = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/"
            if is25ns:
                #config.Data.lumiMask= url + "Cert_246908-258159_13TeV_PromptReco_Collisions15_25ns_JSON_v3.txt"
                # GOLDEN
                ##Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_v2.txt
                # SILVER
                #config.Data.lumiMask = url + "Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver_v2.txt"
                ## SILVER REPROCESSING
                config.Data.lumiMask = url + "Reprocessing/Cert_13TeV_16Dec2015ReReco_Collisions15_25ns_JSON_Silver_v2.txt"
            else:
                config.Data.lumiMask= url + "Cert_246908-255031_13TeV_PromptReco_Collisions15_50ns_JSON.txt"
            config.Data.splitting = 'LumiBased'
        else:
            config.Data.lumiMask = None
            config.Data.splitting = 'FileBased'

        for idx,par in enumerate(config.JobType.pyCfgParams):
            if "isData" in par:
                config.JobType.pyCfgParams[idx] = "isData=" + value
            if "is25ns" in par:
                if is25ns : config.JobType.pyCfgParams[idx] = "is25ns=True"
                else : config.JobType.pyCfgParams[idx] = "is25ns=False"
            if "is50ns" in par:
                if is25ns : config.JobType.pyCfgParams[idx] = "is50ns=False"
                else : config.JobType.pyCfgParams[idx] = "is50ns=True"
        return 
            

    #############################################################################################
    ## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
    #############################################################################################
    
    ###################################################
    ########            25ns  2016             ########
    ###################################################
    setdata("True",is25ns=True,year='2016')
    ###################################################
    config.Data.unitsPerJob = 150

    config.General.requestName = 'SingleMuon-Run2016B'
    config.Data.inputDataset = '/SingleMuon/Run2016B-PromptReco-v1/MINIAOD'
    submit(config)


    ###################################################
    ########              25ns                 ########
    ###################################################
    setdata("True",is25ns=True)
    ###################################################
    config.Data.unitsPerJob = 150

    config.General.requestName = 'SingleMuon-Run2015D'
    config.Data.inputDataset = '/SingleMuon/Run2015D-16Dec2015-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleElectron-Run2015D'
    config.Data.inputDataset = '/SingleElectron/Run2015D-16Dec2015-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'Tau-Run2015D'
    config.Data.inputDataset = '/Tau/Run2015D-16Dec2015-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'MET-Run2015D'
    config.Data.inputDataset = '/MET/Run2015D-16Dec2015-v1/MINIAOD'
    submit(config)

    ###################################################
    setdata("False", is25ns=True)
    ###################################################

    config.General.requestName = 'WZ-pythia8'
    config.Data.inputDataset = '/WZ_TuneCUETP8M1_13TeV-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZ-pythia8'
    config.Data.inputDataset = '/ZZ_TuneCUETP8M1_13TeV-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.Data.unitsPerJob = 5
    config.General.requestName = 'DY-amcatnlo'
    config.Data.inputDataset= '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_HCALDebug_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.Data.unitsPerJob = 20
    config.General.requestName = 'TTJets-amcatnlo'
    config.Data.inputDataset= '/TT_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.Data.unitsPerJob = 20
    config.General.requestName = 'TTJets-madgraph'
    config.Data.inputDataset= '/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = "ChargedHiggs_HplusTB_HplusToTauB_M-500_13TeV_amcatnlo_pythia8"
    config.Data.inputDataset="/ChargedHiggs_HplusTB_HplusToTauB_M-500_13TeV_amcatnlo_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM"
    submit(config)

    config.General.requestName = "ChargedHiggs_HplusTB_HplusToTauB_M-400_13TeV_amcatnlo_pythia8"
    config.Data.inputDataset="/ChargedHiggs_HplusTB_HplusToTauB_M-400_13TeV_amcatnlo_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM"
    submit(config)

    config.General.requestName = "ChargedHiggs_HplusTB_HplusToTauB_M-250_13TeV_amcatnlo_pythia8"
    config.Data.inputDataset="/ChargedHiggs_HplusTB_HplusToTauB_M-250_13TeV_amcatnlo_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM"
    submit(config)


    config.General.requestName = "ChargedHiggs_HplusTB_HplusToTauB_M-200_13TeV_amcatnlo_pythia8"
    config.Data.inputDataset="/ChargedHiggs_HplusTB_HplusToTauB_M-200_13TeV_amcatnlo_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM"
    submit(config)

    config.General.requestName = "ChargedHiggs_HplusTB_HplusToTauB_M-300_13TeV_amcatnlo_pythia8"
    config.Data.inputDataset="/ChargedHiggs_HplusTB_HplusToTauB_M-300_13TeV_amcatnlo_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM"
    submit(config)

    config.General.requestName = "ChargedHiggs_HplusTB_HplusToTauB_M-350_13TeV_amcatnlo_pythia8"
    config.Data.inputDataset="/ChargedHiggs_HplusTB_HplusToTauB_M-350_13TeV_amcatnlo_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM"
    submit(config)

    config.General.requestName = 'WJets'
    config.Data.inputDataset= '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WW'
    config.Data.inputDataset= '/WW_TuneCUETP8M1_13TeV-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTZToQQ'
    config.Data.inputDataset = '/TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ttHTobb'
    config.Data.inputDataset = '/ttHTobb_M125_13TeV_powheg_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ttHToNonbb'
    config.Data.inputDataset = '/ttHToNonbb_M125_13TeV_powheg_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    ##################################
    config.Data.unitsPerJob = 10
    ##################################

    config.General.requestName = 'QCD-15-30'
    config.Data.inputDataset = '/QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-30-50'
    config.Data.inputDataset = '/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-50-80'
    config.Data.inputDataset = '/QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-80-120'
    config.Data.inputDataset = '/QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-120-170'
    config.Data.inputDataset = '/QCD_Pt_120to170_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-170-300'
    config.Data.inputDataset = '/QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-300-470'
    config.Data.inputDataset = '/QCD_Pt_300to470_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-470-600'
    config.Data.inputDataset = '/QCD_Pt_470to600_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-600-800'
    config.Data.inputDataset = '/QCD_Pt_600to800_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-800-1000'
    config.Data.inputDataset = '/QCD_Pt_800to1000_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-1000-1400'
    config.Data.inputDataset = '/QCD_Pt_1000to1400_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-1400-1800'
    config.Data.inputDataset = '/QCD_Pt_1400to1800_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-1800-2400'
    config.Data.inputDataset = '/QCD_Pt_1800to2400_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-2400-3200'
    config.Data.inputDataset = '/QCD_Pt_2400to3200_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD-3200-Inf'
    config.Data.inputDataset = '/QCD_Pt_3200toInf_TuneCUETP8M1_13TeV_pythia8/RunIIFall15MiniAODv2-PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/MINIAODSIM'
    submit(config)

    ###################################################
    ########              50ns                 ########
    ###################################################
    setdata("True",is25ns=False)
    ###################################################

    ###################################################
    setdata("False",is25ns=False)
    ###################################################

# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
