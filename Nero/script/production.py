import os,sys
import re

from optparse import OptionParser, OptionGroup
from subprocess import call,check_output

parser = OptionParser(usage = "usage");
parser.add_option("-p","--prodhome",dest="prodhome",type="string",help="production home [%default]",default="");
parser.add_option("-t","--tag",dest="tag",type="string",help="tag [%default]",default="CMSSW_92X");
parser.add_option("-d","--datasets",dest="datasets",type="string",help="dataset file [%default]",default="datasets.txt");
parser.add_option("-v","--cmssw",dest="cmssw",type="string",help="cmssw [%default]",default="");
(opts,args) = parser.parse_args()

#######################################
## use fast set-up to setup the area ##
#######################################

prodhome = opts.prodhome if opts.prodhome != "" else "/afs/cern.ch/work/" + os.environ['USER'][0] + "/" + os.environ['USER']  + "/production/" 
cmssw = os.environ['CMSSW_VERSION'] if opts.cmssw =="" else opts.cmssw
cmsenv = "cd " + prodhome + " && " "eval `scramv1 runtime -sh`"

if os.path.isdir(prodhome + cmssw + '/src'):
    print "-> Recycling production home:"
    cmd = cmsenv + " && " + "git describe --tags"
    curtag = check_output(cmd,shell=True).split()[0]
    if curtag != opts.tag:
        print "tag in production area is: '" + curtag+"', while requested tag is '"+opts.tag+"'"
        raw_input("ok?")
else:
    print "-> Setting production"
    cmd = "python script/fastsetup.py -v "+cmssw+" -t "+opts.tag + " -d "+ prodhome
    status = call(cmd,shell=True)
    if status != 0:
        print "[ERROR] error occur in fastsetup. Please debug and rerun."
        raise ValueError

#######################################
##          check proxy              ##
#######################################

cmd = "voms-proxy-info"
status = call(cmd,shell=True)
if status != 0 :
    cmd = "voms-proxy-init -voms cms"
    status = call(cmd,shell=True)



#########################################################
## use send on batch to produce the different datasets ##
#########################################################
nerodir = prodhome + "/NeroProducer/Nero"
cmsenv = "cd " + prodhome + "/NeroProducer/Nero" + " && " "eval `scramv1 runtime -sh`"
datasets = open(opts.datesets)
for d in datasets.split():
    print "->",d
    data="--mc"
    if 'Run2016' in d or 'Run2017' in d:
        data="--data"
    dname = '/'.join(d.split('/')[0:-1])
    destination="submissions/"+dname

    ## check failures
    if os.path.isdir(nerodir +"/"+ destination):
        cmd = ' '.join(["python script/sendOnBatch.py","-q 1nd","--only-submit","-j fail"])
        call(cmd,shell=True)

    ## check extra jobs
    cmd = ' ' .join(["python script/sendOnBatch.py", "-n 100" ,"-q 1nd" ,"--condor" ,"--put-in=/store/group/phys_higgs/cmshmm/Nero/"+opts.tag+"/"+dname, data ,"--query","-d "+destination,"-e "+d] )
    call(cmd,shell=True)
print ""


