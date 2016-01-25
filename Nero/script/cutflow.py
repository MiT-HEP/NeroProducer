#! /usr/bin/env python

import os, re, array, ROOT
import sys
from optparse import OptionParser
# Get all the root classes
from ROOT import *
from math import *

gROOT.ProcessLine(".x " + os.path.dirname(os.path.realpath(__file__)) + "/functions.C");

# - M A I N ----------------------------------------------------------------------------------------
# Usage: ./cutflow.py -f ROOTFILE -n NUMBEROFEVENTS

# Prepare the command line parser
parser = OptionParser()
parser.add_option("-f", "--file", dest="input_file", default='NeroNtuple.root',
        help="input root file [default: %default]")
parser.add_option("-t", "--treename", dest="input_tree", default='events',
        help="root tree name [default: %default]")
parser.add_option("-n", "--nprocs", dest="nprocs", type="int", default=10,
        help="number of processed entries [default: %default]")
(options, args) = parser.parse_args()

from testUnit import PrintCompare

# Open the correct input file and get the event tree
input_file = TFile.Open(options.input_file)
if input_file:
    print 'INFO - Opening input root file: ' + options.input_file

else:
    print 'ERROR - Cannot open input root file: ' + options.input_file + ' , exiting!'
  raise SystemExit

input_tree = input_file.FindObjectAny(options.input_tree)
input_all  = input_file.Get("nero/all");
if input_tree:
    print 'INFO - Opening root tree: ' + options.input_tree

else:
    print 'ERROR - Cannot open root tree: ' + options.input_tree + ' , exiting!'
  raise SystemExit

#initialize 
n_jet=0; n_met=0; n_njet=0; n_nlep=0; n_ntau=0; n_npho=0; n_dphi=0;n_2ndjet=0;

# Check the number of entries in the tree
n_entries = input_tree.GetEntriesFast()
print 'INFO - Input tree entries: ' + str(n_entries)

# Determine number of entries to process
if options.nprocs > 0 and options.nprocs < n_entries:
    n_entries = options.nprocs

print 'INFO - Number of entries to be processed: ' + str(n_entries)

# Loop over the entries
for ientry in range(0,n_entries):
    # Grab the n'th entry
  input_tree.GetEntry(ientry)

  #print deltaPhi(input_tree.jetP4[0].Phi(),input_tree.jetP4[1].Phi() )
  #print 'INFO ------------------------ Event '+str(ientry)+' ------------------------ '

  dphi = -10000.

  if(input_tree.jetP4.GetEntriesFast() > 1 and input_tree.jetP4[0].Pt() > 110 and input_tree.jetMonojetId[0] and input_tree.jetMonojetIdLoose[1]) : # and input_tree.jetPuId() > -0.62) : 
      dphi = deltaPhi( input_tree.jetP4[0].Phi(),input_tree.jetP4[1].Phi() )
  else:
      if (input_tree.jetP4.GetEntriesFast() < 2):
          dphi= -99.999
     else:
         dphi = -10000.

  if (input_tree.jetP4.GetEntriesFast() > 0 and input_tree.jetP4[0].Pt() > 110 and input_tree.jetMonojetId[0]):
      n_jet += 1
    if (input_tree.jetP4.GetEntriesFast() == 1 or (input_tree.jetP4.GetEntriesFast() > 1 and input_tree.jetMonojetIdLoose[1])):
        n_2ndjet += 1
      if( fabs(dphi) < 2.5  or dphi == -99.999 ):
          n_dphi += 1
        if(input_tree.metP4[0].Energy() > 200 ):  
            n_met += 1
          if(input_tree.jetP4.GetEntriesFast() < 3): 
              n_njet += 1
            if(input_tree.lepP4.GetEntriesFast() < 1): 
                n_nlep += 1
              if(input_tree.tauP4.GetEntriesFast() < 1): 
                  n_ntau += 1
                if(input_tree.photonP4.GetEntriesFast() < 1):
                    n_npho += 1

print 'INFO - Cut Flow Chart: '
print 'INFO - FULL     '+ str(input_all.GetEntries() )
print 'INFO - GoodVtx  '+ str(n_entries)  ,; PrintCompare(n_entries,3797)
print 'INFO - Jet Cut  '+ str(n_jet)      ,; PrintCompare(n_jet,1995)
print 'INFO - 2_j Cut  '+ str(n_2ndjet)   ,; PrintCompare(n_2ndjet,1990)
print 'INFO - Phi Cut  '+ str(n_dphi)     ,; PrintCompare(n_dphi,1727)
print 'INFO - Met Cut  '+ str(n_met)      ,; PrintCompare(n_met,996)
print 'INFO - NJet Cut '+ str(n_njet)     ,; PrintCompare(n_njet,644)
print 'INFO - NLep Cut '+ str(n_nlep)     ,; PrintCompare(n_nlep,644)
print 'INFO - NTau Cut '+ str(n_ntau)     ,; PrintCompare(n_ntau,641)
print 'INFO - NPho Cut '+ str(n_npho)     ,; PrintCompare(n_npho,637)

# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
