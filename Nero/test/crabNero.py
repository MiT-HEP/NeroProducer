from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call,check_output

#### THIS IS AN EXAMPLE, not tested yet

config = config()

config.General.requestName = 'Nero_request_XXX'
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

## JobType
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'testNero.py'
config.JobType.pyCfgParams=['isGrid=True','isData=False','head='+check_output("git rev-parse HEAD",shell=True), 'tag='+check_output('git describe --tags',shell=True)]

### DATA configuration
config.Data.inputDataset = '/HplusToTauNu-M500/amarini-amarini_PrivateMC_HPlusToTauNu_June2015-16aa19d591b8b49c55c4508e7a7c9233/USER'
config.Data.inputDBS = 'phys03'
config.Data.ignoreLocality = True

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = -1
config.Data.outLFNDirBase = '/store/user/%s/Nero/v1.0/' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.publishDataName ='RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9_RECO'

config.Site.storageSite = 'T2_CH_CERN'

