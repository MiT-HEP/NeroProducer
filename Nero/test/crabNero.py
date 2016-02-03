from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call,check_output

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
config.General.transferLogs = True

## JobType
config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'testNero.py'
config.JobType.pyCfgParams=['isGrid=True','isData=False','is25ns=True','is50ns=False','nerohead='+check_output("git rev-parse HEAD",shell=True), 'nerotag='+check_output('git describe --tags',shell=True)]

# request shipping of the JEC V4 -- local
#config.JobType.inputFiles=['jec/Summer15_50nsV4_DATA.db','jec/Summer15_50nsV4_MC.db']
config.JobType.inputFiles=['jec']

### DATA configuration
config.Data.inputDataset = '/TTbarDMJets_pseudoscalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
#config.Data.inputDBS = 'phys03'
config.Data.inputDBS = 'global'
config.Data.ignoreLocality = True

config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 20
config.Data.totalUnits = -1

tag = check_output("git describe --tags | cut -d'-' -f 1 | tr -d '\n' ",shell=True)
print "-> current tag is '"+tag + "'"
config.Data.outLFNDirBase = '/store/user/%s/Nero/%s/' % (getUsernameFromSiteDB(), tag)
config.Data.publication = False
config.Data.outputDatasetTag ='NeroNtuples'

config.Site.storageSite = 'T2_CH_CERN'
config.Site.blacklist = [ 'T2_US_Purdue','T2_ES_IFCA','T2_EE_Estonia','T2_IT_Pisa' ]


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
	config.Data.outputDatasetTag = config.General.requestName
        try:
            crabCommand('submit', config = config)
        except HTTPException as hte:
            print "Failed submitting task: %s" % (hte.headers)
        except ClientException as cle:
            print "Failed submitting task: %s" % (cle)

    def setdata(value="True",is25ns=False):
	    if value == "True":
		    url = "https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/13TeV/"
		    if is25ns:
		    	#config.Data.lumiMask= url + "Cert_246908-258159_13TeV_PromptReco_Collisions15_25ns_JSON_v3.txt"
			# GOLDEN
			##Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON.txt
			# SILVER
			config.Data.lumiMask= url + "Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver.txt"
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
			

    #############################################################################################
    ## From now on that's what users should modify: this is the a-la-CRAB2 configuration part. ##
    #############################################################################################

    ###################################################
    setdata("False",is25ns=True)
    config.Data.unitsPerJob = 7
    ###################################################
    
    config.General.requestName = 'DMV_NNPDF30_Axial_Mphi-2000_Mchi'
    config.Data.inputDataset  = '/DMV_NNPDF30_Axial_Mphi-2000_Mchi-1_gSM-1p0_gDM-1p0_13TeV-powheg/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'DYJetsToLL_M-50_HT-100to200'
    config.Data.inputDataset  = '/DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-Asympt25ns_74X_mcRun2_asymptotic_v2_ext1-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'DYJetsToLL_M-50_HT-200to400'
    config.Data.inputDataset  = '/DYJetsToLL_M-50_HT-200to400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-Asympt25ns_74X_mcRun2_asymptotic_v2_ext1-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'DYJetsToLL_M-50_HT-400to600'
    config.Data.inputDataset  = '/DYJetsToLL_M-50_HT-400to600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-Asympt25ns_74X_mcRun2_asymptotic_v2_ext1-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'DYJetsToLL_M-50_HT-600toInf'
    config.Data.inputDataset  = '/DYJetsToLL_M-50_HT-600toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-Asympt25ns_74X_mcRun2_asymptotic_v2_ext1-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GJets_HT-40To100'
    config.Data.inputDataset  = '/GJets_HT-40To100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GJets_HT-100To200'
    config.Data.inputDataset  = '/GJets_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GJets_HT-200To400'
    config.Data.inputDataset  = '/GJets_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GJets_HT-400To600'
    config.Data.inputDataset  = '/GJets_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GJets_HT-600ToInf'
    config.Data.inputDataset  = '/GJets_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M110_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M110_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M125_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M125_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M150_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M150_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M200_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M200_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M300_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M300_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M400_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M400_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M500_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M500_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'GluGlu_HToInvisible_M600_13TeV'
    config.Data.inputDataset  = '/GluGlu_HToInvisible_M600_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT100to200_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT200to300_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT300to500_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT500to700_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT700to1000_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT1000to1500_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT1500to2000_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'QCD_HT2000toInf_TuneCUETP8M1_13TeV'
    config.Data.inputDataset  = '/QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_s-channel_4f_leptonDecays_13TeV'
    config.Data.inputDataset  = '/ST_s-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_t-channel_4f_leptonDecays_13TeV'
    config.Data.inputDataset  = '/ST_t-channel_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_t-channel_5f_leptonDecays_13TeV'
    config.Data.inputDataset  = '/ST_t-channel_5f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_t-channel_antitop_4f_leptonDecays_13TeV'
    config.Data.inputDataset  = '/ST_t-channel_antitop_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_t-channel_top_4f_leptonDecays_13TeV'
    config.Data.inputDataset  = '/ST_t-channel_top_4f_leptonDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_tW_antitop_5f_inclusiveDecays_13TeV'
    config.Data.inputDataset  = '/ST_tW_antitop_5f_inclusiveDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ST_tW_top_5f_inclusiveDecays_13TeV'
    config.Data.inputDataset  = '/ST_tW_top_5f_inclusiveDecays_13TeV-amcatnlo-pythia8_TuneCUETP8M1/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTJets'
    config.Data.inputDataset  = '/TTJets_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v3/MINIAODSIM'
    submit(config)

    config.General.requestName = 'TTbarDMJets_pseudoscalar_Mchi'
    config.Data.inputDataset  = '/TTbarDMJets_pseudoscalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M110_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M110_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M125_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M125_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M150_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M150_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M200_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M200_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M300_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M300_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M400_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M400_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M500_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M500_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'VBF_HToInvisible_M600_13TeV'
    config.Data.inputDataset  = '/VBF_HToInvisible_M600_13TeV_powheg_pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WJetsToLNu_HT-100To200'
    config.Data.inputDataset  = '/WJetsToLNu_HT-100To200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WJetsToLNu_HT-200To400'
    config.Data.inputDataset  = '/WJetsToLNu_HT-200To400_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WJetsToLNu_HT-400To600'
    config.Data.inputDataset  = '/WJetsToLNu_HT-400To600_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WJetsToLNu_HT-600ToInf'
    config.Data.inputDataset  = '/WJetsToLNu_HT-600ToInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WW'
    config.Data.inputDataset  = '/WW_TuneCUETP8M1_13TeV-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'WZ'
    config.Data.inputDataset  = '/WZ_TuneCUETP8M1_13TeV-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZZ'
    config.Data.inputDataset  = '/ZZ_TuneCUETP8M1_13TeV-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-100To200_13TeV'
    config.Data.inputDataset  = '/ZJetsToNuNu_HT-100To200_13TeV-madgraph/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-200To400_13TeV'
    config.Data.inputDataset  = '/ZJetsToNuNu_HT-200To400_13TeV-madgraph/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-400To600_13TeV'
    config.Data.inputDataset  = '/ZJetsToNuNu_HT-400To600_13TeV-madgraph/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    config.General.requestName = 'ZJetsToNuNu_HT-600ToInf_13TeV'
    config.Data.inputDataset  = '/ZJetsToNuNu_HT-600ToInf_13TeV-madgraph/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM'
    submit(config)

    ###################################################
    ########              25ns                 ########
    ###################################################
    setdata("True",is25ns=True)
    ###################################################
    config.Data.unitsPerJob = 50

    config.General.requestName = 'SingleMuon-Run2015D-v1'
    config.Data.inputDataset = '/SingleMuon/Run2015D-05Oct2015-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleMuon-Run2015D-v4'
    config.Data.inputDataset = '/SingleMuon/Run2015D-PromptReco-v4/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleElectron-Run2015D-v1'
    config.Data.inputDataset = '/SingleElectron/Run2015D-05Oct2015-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SingleElectron-Run2015D-v4'
    config.Data.inputDataset = '/SingleElectron/Run2015D-PromptReco-v4/MINIAOD'
    submit(config)

    config.General.requestName = 'SinglePhoton-Run2015D-v1'
    config.Data.inputDataset = '/SinglePhoton/Run2015D-05Oct2015-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'SinglePhoton-Run2015D-v4'
    config.Data.inputDataset = '/SinglePhoton/Run2015D-PromptReco-v4/MINIAOD'
    submit(config)

    config.General.requestName = 'MET-Run2015D-v1'
    config.Data.inputDataset = '/MET/Run2015D-05Oct2015-v1/MINIAOD'
    submit(config)

    config.General.requestName = 'MET-Run2015D-v4'
    config.Data.inputDataset = '/MET/Run2015D-PromptReco-v4/MINIAOD'
    submit(config)
