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
config.Data.outLFNDirBase = '/afs/cern.ch/work/z/zdemirag/work/run2/monojet/CMSSW_7_4_5/src/NeroProducer/Nero/test/%s/' % (tag)
config.Data.publication = False
config.Data.publishDataName =''

config.Site.storageSite = 'T2_CH_CERN'

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
    config.Data.unitsPerJob = 10
    ###################################################

    ###################################################
    setdata("True")
    ###################################################

    config.General.requestName = 'MET-50ns'
    config.Data.inputDataset = '/MET/Run2015B-17Jul2015-v1/MINIAOD'
    submit(config)

    #config.General.requestName = 'SingleMuon-50ns'
    #config.Data.inputDataset = '/SingleMuon/Run2015B-17Jul2015-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'SingleElectron-50ns'
    #config.Data.inputDataset = '/SingleElectron/Run2015B-17Jul2015-v1/MINIAOD'
    #submit(config)

    #config.General.requestName = 'SinglePhoton-50ns'
    #config.Data.inputDataset = '/SinglePhoton/Run2015B-17Jul2015-v1/MINIAOD'
    #submit(config)

    ###################################################
    #setdata("False")
    ###################################################
    #config.General.requestName = 'DY-50ns'
    #config.Data.inputDataset = '/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v2/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'TT-50ns'
    #config.Data.inputDataset = '/TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v1/MINIAODSIM'
    #submit(config)

    #config.General.requestName = 'WJets-50ns'
    #config.Data.inputDataset = '/WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15DR74-Asympt50ns_MCRUN2_74_V9A-v1/MINIAODSIM'
    #submit(config)
