#!/usr/bin/env python


import sys, os
import ROOT
import re
from subprocess import check_output, call
from optparse import OptionParser


parser = OptionParser()
parser.add_option("-e", "--eos", dest="eos", default='', help="eos path to scout for duplicates [default: %default]")
parser.add_option("-r","--recursive", dest='rec', action= 'store_true', help="do same for each subdir", default =False);
(opts,args) = parser.parse_args()

EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"

if opts.rec:
	cmd = EOS +" find -d "+opts.eos
	list = check_output(cmd,shell=True);
	for line in list.split('\n'):
		if line =='': continue
		print "Line is '"+line+"'"
		dir = line.split() [0]
		ndir = line.split() [1]
		nfiles = line.split() [2]
		#
		nd = float(re.sub('ndir=','',ndir) )
		nf = float(re.sub('nfiles=','',nfiles) )
		dir = re.sub('/eos/cms','',dir)
		if dir[-1] == '/' : dir = dir[:-1] # remove trailing slash
		label = re.sub('.*/','',dir)
		if nd==0 and nf >0 :
			print "Found one directory:",dir
			cmd = "python %s -e %s "%( sys.argv[0] , dir )
			print "going to execute",cmd
			call(cmd,shell=True)
	exit(0)

cmd = EOS+ " find -f " + opts.eos
outputList = check_output(cmd,shell=True)
fileList0 = outputList.split() ## change lines into list
filelist = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 ]

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

duplicate=False
elem={}

for f in filelist:
	fROOT = ROOT.TFile.Open(f)
	t = fROOT.Get("nero/events")
	t.SetBranchStatus("*",0)
	t.SetBranchStatus("runNum",1)
	t.SetBranchStatus("lumiNum",1)
	t.SetBranchStatus("eventNum",1)
	for i in range(0,t.GetEntries() ) :
		t.GetEntry(i)
		id = (t.runNum,t.lumiNum,t.eventNum)
		if id in elem:
			print "Duplicate found in file: "+colors['red'] + f + colors['white'] + ' and ' + elem[id] + " runNum=",t.runNum,"lumi=",t.lumiNum,"event=",t.eventNum
			duplicate=True
		elem[id] = f

if duplicate:
	print colors['red']+"DONE"+colors['white']
else:
	print colors['green']+"DONE"+colors['white']
