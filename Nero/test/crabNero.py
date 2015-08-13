from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call,check_output

import sys

config = config()

config.General.requestName = 'Nero_request_XXX'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

## JobType
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'testNero.py'
config.JobType.pyCfgParams=['isGrid=True','isData=False','nerohead='+check_output("git rev-parse HEAD",shell=True), 'nerotag='+check_output('git describe --tags',shell=True)]

### DATA configuration
config.Data.inputDataset = '/HplusToTauNu-M500/amarini-amarini_PrivateMC_HPlusToTauNu_June2015-16aa19d591b8b49c55c4508e7a7c9233/USER'
#config.Data.inputDBS = 'phys03'
config.Data.inputDBS = 'global'
config.Data.ignoreLocality = True

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = -1

tag = check_output("git describe --tags | cut -d'-' -f 1 | tr -d '\n' ",shell=True)
print "-> current tag is '"+tag + "'"
config.Data.outLFNDirBase = '/store/user/%s/Nero/%s/' % (getUsernameFromSiteDB(), tag)
config.Data.publication = False
config.Data.publishDataName =''

config.Site.storageSite = 'T2_CH_CERN'
#config.Site.blacklist = [ 'T2_US_Florida','T2_US_Vanderbilt']


if __name__ == '__main__':

    from CRABAPI.RawCommand import crabCommand
    from CRABClient.ClientExceptions import ClientException
    from httplib import HTTPException

    # We want to put all the CRAB project directories from the tasks we submit here into one common directory.
    # That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).
    config.General.workArea = 'NeroSubmission'

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
        try:
            crabCommand('submit', config = config)
        except HTTPException as hte:
            print "Failed submitting task: %s" % (hte.headers)
        except ClientException as cle:
            print "Failed submitting task: %s" % (cle)

    def setdata(value="True"):
	    for idx,par in enumerate(config.JobType.pyCfgParams):
		    if "isData" in par:
			config.JobType.pyCfgParams[idx] = "isData=" + value

    #############################################################################################
    ## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
    #############################################################################################

    ###################################################
    #setdata("False")
    #config.Data.unitsPerJob = 10
    ####################################################

    #config.General.requestName = 'WZ-25ns'
    #config.Data.inputDataset = '/WZ_TuneCUETP8M1_13TeV-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'ZZ-25ns'
    #config.Data.inputDataset = '/ZZ_TuneCUETP8M1_13TeV-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'DY-25ns'
    #config.Data.inputDataset= '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TT-25ns'
    #config.Data.inputDataset= '/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WJets-25ns'
    #config.Data.inputDataset= '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WW-25ns'
    #config.Data.inputDataset= '/WW_TuneCUETP8M1_13TeV-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TTZToQQ-25ns'
    #config.Data.inputDataset = '/TTZToQQ_TuneCUETP8M1_13TeV-amcatnlo-pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'ttHTobb-25ns'
    #config.Data.inputDataset = '/ttHTobb_M125_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'ttHToNonbb-25ns'
    #config.Data.inputDataset = '/ttHToNonbb_M125_13TeV_powheg_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-15-30-25ns'
    #config.Data.inputDataset = '/QCD_Pt_15to30_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-30-50-25ns'
    #config.Data.inputDataset = '/QCD_Pt_30to50_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-50-80-25ns'
    #config.Data.inputDataset = '/QCD_Pt_50to80_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-80-120-25ns'
    #config.Data.inputDataset = '/QCD_Pt_80to120_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-120-170-25ns'
    #config.Data.inputDataset = '/QCD_Pt_120to170_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-170-300-25ns'
    #config.Data.inputDataset = '/QCD_Pt_170to300_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-300-470-25ns'
    #config.Data.inputDataset = '/QCD_Pt_300to470_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-470-600-25ns'
    #config.Data.inputDataset = '/QCD_Pt_470to600_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-600-800-25ns'
    #config.Data.inputDataset = '/QCD_Pt_600to800_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v3/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-800-1000-25ns'
    #config.Data.inputDataset = '/QCD_Pt_800to1000_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v2/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-1000-1400-25ns'
    #config.Data.inputDataset = '/QCD_Pt_1000to1400_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-1400-1800-25ns'
    #config.Data.inputDataset = '/QCD_Pt_1400to1800_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-1800-2400-25ns'
    #config.Data.inputDataset = '/QCD_Pt_1800to2400_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-2400-3200-25ns'
    #config.Data.inputDataset = '/QCD_Pt_2400to3200_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    #
    #config.General.requestName = 'QCD-3200-Inf-25ns'
    #config.Data.inputDataset = '/QCD_Pt_3200toInf_TuneCUETP8M1_13TeV_pythia8/RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9-v1/MINIAODSIM'
    #submit(config)
    
    ###################################################
    setdata("True")
    ###################################################

    #config.General.requestName = 'MET-50ns'
    #config.Data.inputDataset = '/MET/Run2015B-17Jul2015-v1/MINIAOD'
    config.General.requestName = 'MET-50ns-PromptReco'
    config.Data.inputDataset = '/MET/Run2015B-PromptReco-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'Tau-50ns'
    #config.Data.inputDataset = '/Tau/Run2015B-17Jul2015-v1/MINIAOD'
    config.General.requestName = 'Tau-50ns-PromptReco'
    config.Data.inputDataset = '/Tau/Run2015B-PromptReco-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'SingleMuon-50ns'
    #config.Data.inputDataset = '/SingleMuon/Run2015B-17Jul2015-v1/MINIAOD'
    config.General.requestName = 'SingleMuon-50ns-PromptReco'
    config.Data.inputDataset = '/SingleMuon/Run2015B-PromptReco-v1/MINIAOD'
    submit(config)

    #config.General.requestName = 'SingleElectron-50ns'
    #config.Data.inputDataset = '/SingleElectron/Run2015B-17Jul2015-v1/MINIAOD'
    config.General.requestName = 'SingleElectron-50ns-PrompReco'
    config.Data.inputDataset = '/SingleElectron/Run2015B-PromptReco-v1/MINIAOD'
    submit(config)

    #config.General.requestName = 'SinglePhoton-50ns'
    #config.Data.inputDataset = '/SinglePhoton/Run2015B-17Jul2015-v1/MINIAOD'
    config.General.requestName = 'SinglePhoton-50ns-PromptReco'
    config.Data.inputDataset = '/SinglePhoton/Run2015B-PromptReco-v1/MINIAOD'
    #submit(config)

    ###################################################
    setdata("False")
    ###################################################
    config.General.requestName = 'DY-50ns'
    config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TT-50ns'
    config.Data.inputDataset = '/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZ-50ns'
    config.Data.inputDataset = '/WZ_TuneCUETP8M1_13TeV-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZ-50ns'
    config.Data.inputDataset = '/ZZ_TuneCUETP8M1_13TeV-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WJets-50ns'
    config.Data.inputDataset = '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WW-50ns'
    config.Data.inputDataset = '/WW_TuneCUETP8M1_13TeV-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v1/MINIAODSIM'
    submit(config)


