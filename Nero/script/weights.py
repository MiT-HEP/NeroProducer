#!env python

import re
import os,sys
import math
from optparse import OptionParser
from subprocess import call, check_output
from array import array

parser = OptionParser(usage = "usage");
parser.add_option("-e","--eos",dest="eos",type="string",help="eos directory to scout.",default="")
parser.add_option("-x","--xsec",dest="xsec",type="float",help="Use this cross section instead of the internal one.",default=-1)
parser.add_option("-d","--dir",dest="dir",type="string",help="Output Directory [Default=%default]",default="weights")
parser.add_option("","--put-back",dest="putback",type='string',help="Take the weights and push them to this eos location. [TODO]",default="")
parser.add_option("","--pu-target",dest="putarget",type='string',help="PU target for PU reweighting. [TODO]",default="")

(opts,args)=parser.parse_args()

EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"


### GET file list from eos
cmd = EOS+ " find -f " + opts.eos
outputList = check_output(cmd,shell=True)
fileList0 = outputList.split() ## change lines into list
fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 ]

#import root
import ROOT as r
r.gROOT.SetBatch()

#check if directory exist
outdir=opts.dir

status=call("[ -d " + outdir + " ] ", shell = True) 
if status == 0:
    print "Directory",outdir,"already exists. I'm trying to remove it."
    cmd =  "rmdir " + outdir
    status = call(cmd, shell = True)
    if status !=0:
        print "Directory",outdir,"is not empty."
            print " -> PLEASE REMOVE IT!"
            raise Exception('Directory not empty')

call(["mkdir",outdir])


myTmp= r.TFile.Open("/tmp/" + os.environ["USER"] + "/mytmp.root","RECREATE")
myTmp.cd()
sum=r.TH1F("SumWeights","Sum of mcWeights",1,0,2)

print "-> First run"
n=0
for idx,fName in enumerate(fileList):
    print "processing file:",idx,"/",len(fileList)," : ", fName
    fROOT = r.TFile.Open( fName )
    if idx == 0:
        myTmp.cd()
        h_xSec = fROOT.Get("nero/xSec").Clone("myxSec")
    else:
        h = fROOT.Get("nero/xSec")
        print "\txSec in file is ",h.GetBinContent(1)/h.GetBinContent(2),"+/-",math.sqrt(1./h.GetBinContent(2))
        h_xSec.Add( h )

    t = fROOT.Get("nero/all")
    mysum=r.TH1F("mysum","Sum of mcWeights",1,0,2)
    t.Draw("1>>mysum","mcWeight") ##>>+ doesn't work
    sum.Add(mysum)
    n += t.GetEntries()
    fROOT.Close()

if opts.xsec<0: xsec = h_xSec.GetBinContent(1)/h_xSec.GetBinContent(2)
else: xsec = opts.xsec

sumW=sum.GetBinContent(1)

print "---------------------------------------------"
print "xSec = ", xsec
print "SumWeights = ", sumW
print "Tot Entries = ", n
print "---------------------------------------------"


r.gROOT.ProcessLine(' struct eventInfo{ \
        ULong64_t eventNum; \
        int lumiNum; \
        int runNum; \
        };')
from ROOT import eventInfo

print "-> Second run"
for idx,fName in enumerate(fileList):
    print "processing file:",idx,"/",len(fileList)," : ", fName
    outname = "NeroWeight_" + re.sub('.*_','',fName) ## I suppose is in the form ***_num.root
    print "outfile is:",outdir+"/"+outname
    fOut = r.TFile.Open(outdir+"/"+outname, "RECREATE")
    fOut.mkdir("nero")
    fOut.cd("nero")
    tWeights= r.TTree("weights","weights")
    w=array( 'f', [ 0. ] )
    tWeights.Branch("xsecWeight",w,"xsecWeight/F")
    #copy also runNum, lumiNum, eventNum
    e = eventInfo()
    tWeights.Branch("runNum",r.AddressOf(e,'runNum'),"runNum/I")
    tWeights.Branch("lumiNum",r.AddressOf(e,'lumiNum'),"lumiNum/I")
    tWeights.Branch("eventNum",r.AddressOf(e,'eventNum'),"eventNum/l")

    fIn = r.TFile.Open(fName)
    t = fIn.Get("nero/events")
    fOut.cd("nero")
    for i in range(0,t.GetEntries() ) :
        t.GetEntry(i)
        w= t.mcWeight * xsec / sumW
        # event info
        e.runNum = t.runNum
        e.lumiNum = t.lumiNum
        e.eventNum = t.eventNum
        # Fill the tree
        tWeights.Fill()
    fOut.cd("nero")
    ## index probably work with TTree or global TChain, 
    ## merging the TChain with the index saved did not work so fare
    #################### 20bit=0 - 2097151 , 12 bit = 4095
    ### the major and minor are sorted summing the major<<31 + minor
    ###Assuming a parent Tree T and a friend Tree TF, the following cases are supported:
    ###  CASE 1: T->GetEntry(entry) is called
    ###          In this case, the serial number entry is used to retrieve
    ###          the data in both Trees.
    ###  CASE 2: T->GetEntry(entry) is called, TF has a TreeIndex
    ###          the expressions given in major/minorname of TF are used
    ###          to compute the value pair major,minor with the data in T.
    ###          TF->GetEntryWithIndex(major,minor) is then called (tricky case!)
    ###  CASE 3: T->GetEntryWithIndex(major,minor) is called.
    ###          It is assumed that both T and TF have a TreeIndex built using
    ###          the same major and minor name.
    #################### 20bit=0 - 2097151 , 12 bit = 4095
    ### 
    ### print "Building index using run<<20 + lumiNum&111... and eventNum.",
    ### tWeights.BuildIndex("lumiNum","eventNum");
    ### print tWeights.BuildIndex("runNum<<20 + (lumiNum&11111111111111111111)","eventNum");
    ### tWeights.GetTreeIndex().Write("index") ## ok this does not w
    tWeights.Write("",r.TObject.kOverwrite)
    fOut.Close()
    fIn.Close()
print "-> DONE"



# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
