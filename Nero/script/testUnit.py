#!/usr/bin/env python

import os, re, array
import ROOT
import sys
from subprocess import call
from optparse import OptionParser

if __name__ == "__main__":
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="file", default='NeroNtuple.root', help="input root file [default: %default]")
    parser.add_option("-c", "--core", dest="core", action='store_true',default=False, help="Only Test Core [default: %default]")
    (opts,args) = parser.parse_args()

colors= {
        "green"	:"\033[01;32m",
        "yellow":"\033[01;33m",
        "red"	:"\033[01;31m",
        "white"	:"\033[00m"
        }
def NoColors():
    colors["green"	]=""
    colors["yellow"	]=""
    colors["red"	]=""
    colors["white"	]=""

def PrintDone():
    print colors["green"]+"DONE"+colors["white"]

def PrintFail():
    print colors["red"]+"FAIL"+colors["white"]

def PrintWarning(info):
    #no new line
    print colors["yellow"]+"WARNING"+colors["white"],
    print ": " + info

def PrintError(info):
    #no new line
    print " -> ",
    print colors["red"]+"ERROR"+colors["white"],
    print info


def PrintCompare(a,b):
    if a == b: PrintDone()
    else : 
        print " instead of ",b,
        PrintFail()

def TestCore():
    ### TEST THE CORE LIBRARY
    print " *** Compiling Core Library ***"
    if os.environ["CMSSW_BASE"] == "": 
        PrintError(": do cmsenv")
        exit(1)
    cmd = "cd "+ os.environ["CMSSW_BASE"]  + "/src/NeroProducer/Core && make -j 16" 
    print " -> calling cmd '"+cmd+"'"
    call(cmd,shell=True)

    print " *** Loading Core Library ***"
    ROOT.gSystem.Load(  os.environ["CMSSW_BASE"] + "/src/NeroProducer/Core/bin/libBare.so" ) 

    print " *** Loading File ***"

    fROOT = ROOT.TFile.Open(opts.file)

    if fROOT == None : 
        PrintError(": '"+opts.file+"': no such file or directory")
        exit(1)

    tree = fROOT.Get("nero/events")
    all = fROOT.Get("nero/all")

    if tree == None:
        PrintError(": 'nero/events': no such tree in file", opts.file)
        exit(1)
    if all == None:
        PrintError(": 'nero/all': no such tree in file", opts.file)
        exit(1)

    if tree.GetEntries() == 0 or all.GetEntries() == 0 :
        PrintError(": trees have 0 entries")
        exit(1)

    objs= [ "BareFatJets","BareLeptons","BareMonteCarlo","BareTaus","BareVertex","BareEvent","BareJets","BareMet","BarePhotons","BareTrigger"]

    print " *** Testing Event Tree ***"
    garbage = []
    for o in objs:
        print " -> Constructing objects: '"+o+"':",
        sys.stdout.flush()
        tmp = ROOT.__getattr__(o)()
        tmp.setBranchAddresses(tree)
        garbage.append(tmp)
        tree.GetEntry(0)
        ## specific to each 
        warn = False
        if o in ['BarePhotons','BareJets','BareMet','BareLeptons','BareTaus']:
            n = tmp.p4.GetEntries()
            iEntry = 0 
            while n==0:
                iEntry += 1;
                tree.GetEntry(iEntry);
                n = tmp.p4.GetEntries()
                if iEntry >= 100 :
                    PrintWarning("no event with '"+o+"' in the first "+str(iEntry)+" entries")
                    warn=True
                    break;
        if not warn:
            PrintDone()

    print " *** Testing All Tree ***"

    objs2 = [ "BareAll"]

    for o in objs2:
        print " -> Constructing objects: '"+o+"':",
        sys.stdout.flush()
        tmp = ROOT.__getattr__(o)()
        tmp.setBranchAddresses(all)
        garbage.append(tmp)
        all.GetEntry(0)
        PrintDone()
    fROOT.Close()
    return	

def TestChargedHiggs():
    ''' Need to define some sample and some efficiency for that cuts'''
    print " *** Testing Cut Flow on a standard tree ***"

    fROOT = ROOT.TFile.Open(opts.file)
    tree = fROOT.Get("nero/events")

    ## init flow
    nEntries = 0
    nTaus = 0 
    nLeptons = 0 

    for iEntry in range(0, tree.GetEntries() ) :
        tree.GetEntry(iEntry)
        nEntries += 1
        if tree.lepP4.GetEntriesFast()  == 0 : continue
        nLeptons += 1
        if tree.tauP4.GetEntriesFast()  == 0 : continue
        nTaus += 1

    print "nEntries = ",nEntries, 
    PrintCompare(nEntries,3797)
    print "nLeptons = ",nLeptons,
    PrintCompare(nLeptons,7)
    print "nTaus = ",nTaus,
    PrintCompare(nTaus,4)

def TestMonoJet():
    call("python cutflow.py -f "+opts.file + " -n 1000000",shell=True)

# test that loading the library and try to access it is fine
if __name__ == "__main__":
    TestCore()
    if not opts.core:
        TestChargedHiggs()
        TestMonoJet()

# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
