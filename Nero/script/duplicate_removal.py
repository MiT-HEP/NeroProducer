#!/usr/bin/env python

import os, sys
from optparse import OptionParser, OptionGroup

import ROOT
ROOT.gROOT.SetBatch();


from subprocess import call, check_output
import re
import math
EOS = "/usr/bin/eos"

usage = '''
	script to remove duplicates when running on data. The script is meant for multiple submission, 
	when the new set of files will completely contain the luminosity processed in one old file.
	Assumptions: 
	* No duplicates inside a single processed file (CMSSW do it).
	* Data inside a luminosity block are full processed by each job.
	Output:
	* the output file contains a list of file to be removed (fname)
	* a list of file with only some duplicate (XXX fname (ONLY SOME DUPLICATE))
	'''

parser = OptionParser(usage = usage)
parser.add_option("-e","--eos",dest="eos",type="string",help="eos directory to scout [%default]",default="/store/group/phys_higgs/ceballos/Data/Nero/v2.2/MuonEG/MuonEG_Run2016H_ver2");
parser.add_option("-o","--output",dest="output",type="string",help="output file [%default]",default="toremove.txt");
parser.add_option("-v","--verbose",dest="verbose",action="store_true",help="Print More",default=False);
(opts, args) = parser.parse_args()



def FindFiles(path):
	cmd = EOS+ " find -f " + path
	outputList = check_output(cmd,shell=True)
	fileList0 = outputList.split() ## change lines into list
	removed = [ f for f in fileList0 if '/failed/' in f ]
	for f in removed:
		print>>sys.stderr, "ignoring failed file:",f
	filelist = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]
	return filelist

def LsEos(path):
	''' EG. /store/group/phys_higgs/ceballos/Data/Nero/v2.2/MuonEG/MuonEG_Run2016H_ver2
	170213_114423
	170210_161755
	'''
	## reversed ordered, last directories, come first
	cmd = EOS + " ls " + path + " | sort -nr "
	outputList = check_output(cmd,shell=True)
	return outputList.split()


#event = fin.Get("nero/all")
#runNum:lumiNum

processed={} ## run -> [ lumi1,lumi2...]

def AddProcessedRunLumi(run,lumi,f):
	global processed
	if run not in processed:
		processed[run] = {lumi:f}
		return 0
	elif lumi not in processed[run]:
		processed[run][lumi]=f
		return 0
	elif processed[run][lumi] == f :
		## same file, ok
		return 0 
	else:
		return 1

def PrintStatus(done,tot,n=30):
        #if done % 10000 != 0  and done < tot-1: return
        #a=int(math.floor(float(done)*n/tot ))
        #a=int(round(float(done)*n/tot )
        a=int(math.floor(float(done)*n/(tot-1) ))
        l="\r["
        if a==n: l += "="*n
        elif a> 0: l +=  "="*(a-1) + ">" + " "*(n-a)
        else: l += " "*n
        l+="] %.1f%%"%( float(done)*100./(tot-1) )
        if a==n: l+="\n"
        print l,
        sys.stdout.flush()

if __name__=="__main__":
	out=open(opts.output,"w")
	dirList=LsEos(opts.eos)
	for d in dirList:
		print "-> entering dir", opts.eos + "/" + d 
		fileList= FindFiles( opts.eos +"/" + d )
		for ifile, f in enumerate(fileList):
			if opts.verbose:
				print " * file ",ifile,"of",len(fileList)
			else:
				PrintStatus(ifile,len(fileList))
			fIn = ROOT.TFile.Open(f)
			t = fIn.Get("nero/all")
			if t==None:
				print "\n[ERROR] Unable to find nero/all tree in ",f
				raise IOError
			t.SetBranchStatus("*",0)
			t.SetBranchStatus("runNum",1)
			t.SetBranchStatus("lumiNum",1)
			#t.SetBranchStatus("eventNum",1)
			existsDuplicate = False
			allDuplicates = True
			for ientry in range(0,t.GetEntries() ):
				t.GetEntry(ientry)
				if AddProcessedRunLumi(t.runNum, t.lumiNum,f ):
					## this is a duplicate event
					existsDuplicate=True
				else:
					allDuplicates = False
			if allDuplicates:
				if opts.verbose: print "-> File",f, "can be safely removed (all duplicates)"
				print >> out,f
			elif existsDuplicate:
				if opts.verbose: print "-> File",f, "contains duplicates (but not all of them are!)"
				print >> out,"XXX",f,"(ONLY SOME DUPLICATES)"


