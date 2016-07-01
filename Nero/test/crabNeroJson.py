from CRABClient.UserUtilities import config, getUsernameFromSiteDB
from subprocess import call, check_output

import sys, os

import json

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
#config.Site.storageSite = 'T2_CH_CERNBOX'
#config.Site.blacklist = [ 'T2_US_Florida','T2_US_Vanderbilt']


if __name__ == '__main__':

    from CRABAPI.RawCommand import crabCommand
    from CRABClient.ClientExceptions import ClientException
    from httplib import HTTPException

    # We want to put all the CRAB project directories from the tasks we submit here into one common directory.
    # That's why we need to set this parameter (here or above in the configuration file, it does not matter, we will not overwrite it).

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
            print "* config is:",config.JobType.pyCfgParams
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
            

    config.General.workArea = 'NeroSubmission'
    
    ## add the json files 
    ## modify the torun.json at needs
    jsons={} ## per file
    fileList= ['torun.json']
    for jfile in fileList:
        print "-> Opening",jfile
        with open(jfile) as data_file:    
            data = json.load(data_file)

        for info in data:
            if 'none' in info:
                    continue

            if 'file' in info: 
                    ## read other info and set them as well
                    fileList.append(info['file'])
                    jsons[info['file']] = info ## save the jsons and merge them
                    continue
            if jfile in jsons: #merge the jsons
                for key in jsons[jfile]:
                    if key == 'file' : continue
                    if key in info : continue
                    #print "* adding key",key, "from configuration of file",jfile
                    info[key] = jsons[jfile][key]
            #print '->Doing Request',info['requestName'] ## DEBUG

            if 'workArea' in info: config.General.workArea= info['workArea']
            if 'unitsPerJob' in info: config.Data.unitsPerJob= info['unitsPerJob']

            if 'data' in info and info['data'] : setdata("True",is25ns=True)
            if 'data' in info and not info['data'] : setdata("False",is25ns=True)

            if 'requestName' in info: config.General.requestName=info['requestName']
            if 'inputDataset' in info: config.Data.inputDataset=info['inputDataset']
            #print "* WA",config.General.workArea
            #print "* UJ",config.Data.unitsPerJob
            #print "* DS",config.Data.inputDataset
            #print "* data", 'data' in info and info['data']
            if 'pset' in info:
                config.JobType.psetName = info['pset']  


            if 'requestName' not in info or 'inputDataset' not in info: continue


            if info['submit'] : submit(config)


# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
