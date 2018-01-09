import os,sys
import re

from optparse import OptionParser, OptionGroup
from subprocess import call,check_output

parser = OptionParser(usage = "usage");
parser.add_option("-p","--prodhome",dest="prodhome",type="string",help="production home [%default]",default="");
parser.add_option("-t","--tag",dest="tag",type="string",help="tag [%default]",default="CMSSW_92X");
parser.add_option("-d","--datasets",dest="datasets",type="string",help="dataset file [%default]",default="datasets.txt");
parser.add_option("-v","--cmssw",dest="cmssw",type="string",help="cmssw [%default]",default="");
parser.add_option("-s","--status",dest="status",action='store_true',help="Status [%default]",default=False);
(opts,args) = parser.parse_args()

colors={ ## bash colors
        'red':"\033[01;31m",
        'green': "\033[01;32m",
        'yellow': "\033[01;33m",
        'blue': "\033[01;34m",
        'pink': "\033[01;35m",
        'cyan': "\033[01;36m",
        'white': "\033[00m",
        }

#######################################
## use fast set-up to setup the area ##
#######################################

prodhome = opts.prodhome if opts.prodhome != "" else "/afs/cern.ch/work/" + os.environ['USER'][0] + "/" + os.environ['USER']  + "/production/" 
cmssw = os.environ['CMSSW_VERSION'] if opts.cmssw =="" else opts.cmssw
nerodir = prodhome +"/" + cmssw + "/src/NeroProducer/Nero"
cmsenv = "cd " + prodhome + "/"+cmssw +"/src" + " && " "eval `scramv1 runtime -sh`"

if os.path.isdir(prodhome + cmssw + '/src'):
    print colors['cyan'] + "-> Recycling production home:" +colors['white']
    cmd = cmsenv + " && cd "+nerodir +" && " + "git describe --tags"
    curtag = check_output(cmd,shell=True).split()[0]
    print "    current tag is",curtag
    if curtag != opts.tag:
        print "tag in production area is: '" + curtag+"', while requested tag is '"+opts.tag+"'"
        raw_input("ok?")
    if opts.status:
        cmd = "cd " + nerodir + " && " + "eval `scramv1 runtime -sh`"
        cmd += " && for dir in submissions/*/*; do python script/sendOnBatch.py --status -d $dir ; done" 
        call(cmd, shell=True)
        exit(0)
else:
    if opts.status: 
        print "["+colors["red"] + "ERROR"+colors["white"]+"] asking status but area does not exist" 
        raise ValueError
    print colors['cyan'] + "-> Setting production" + colors['white']
    cmd = "python script/fastsetup.py -v "+cmssw+" -t "+opts.tag + " -d "+ prodhome
    status = call(cmd,shell=True)
    if status != 0:
        print "["+colors['red']+"ERROR"+colors['white']+"] error occur in fastsetup. Please debug and rerun."
        raise ValueError

#######################################
##          check proxy              ##
#######################################

cmd = "voms-proxy-info 2>&1 >/dev/null"
status = call(cmd,shell=True)
if status != 0 :
    cmd = "voms-proxy-init -voms cms"
    status = call(cmd,shell=True)
    cmd = "voms-proxy-info"
    status = call(cmd,shell=True)
    if status != 0: 
        print "["+ colors['red'] + "ERROR" + colors['white'] + "] unable to find proxy "
        raise ValueError


#########################################################
## use send on batch to produce the different datasets ##
#########################################################
cmsenv = "cd " + nerodir + " && " "eval `scramv1 runtime -sh`"
datasets = open(opts.datasets)
for l in datasets.readlines():
    if len(l.split()) == 0  : continue
    d=l.split()[0]
    print "->",colors['blue'] + d + colors['white']
    data="--mc"
    if 'Run2016' in d or 'Run2017' in d:
        data="--data"
    dname = '/'.join(d.split('/')[0:-1])
    destination="submissions/"+dname
   
    ## disable for resubmissions and new jobs to be submitted. Not fully supported yet.
    #condor="--condor"
    condor=""
    ## check failures
    if os.path.isdir(nerodir +"/"+ destination):
        condor="" ## reset condor for resubmissions anyhow
        cmd = cmsenv + ' && ' + ' '.join(["python script/sendOnBatch.py","-q 1nd","--only-submit","-j fail","--proxy","-d " + destination])
        #print " Calling cmd '" + cmd +"'"
        call(cmd,shell=True)

    ## create destination
    finalDestination="/eos/cms/store/group/phys_higgs/cmshmm/amarini/Nero/"+opts.tag+"/"+dname
    cmd = "eos mkdir -p "  +finalDestination
    call(cmd,shell=True)
    ## check also for extra jobs
    cmd = cmsenv + ' && ' +' ' .join(["python script/sendOnBatch.py", "-n 100" ,"-q 1nd",condor,"-i test/testNero.py"  ,"--put-in="+finalDestination, data ,"--query","--proxy","-d "+destination,"-e "+d] )
    #print " Calling cmd '" + cmd +"'"
    call(cmd,shell=True)
print ""


