#! /usr/bin/env python

import os, re, array, ROOT
import sys
from optparse import OptionParser
# Get all the root classes
from ROOT import *
from math import *

gROOT.ProcessLine(".x functions.C");

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

# Open the correct input file and get the event tree
input_file = TFile.Open(options.input_file)
if input_file:
  print 'INFO - Opening input root file: ' + options.input_file
  
else:
  print 'ERROR - Cannot open input root file: ' + options.input_file + ' , exiting!'
  raise SystemExit
  
input_tree = input_file.FindObjectAny(options.input_tree)
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
print 'INFO - Full     '+ str(n_entries)
print 'INFO - Jet Cut  '+ str(n_jet)
print 'INFO - 2_j Cut  '+ str(n_2ndjet)
print 'INFO - Phi Cut  '+ str(n_dphi)
print 'INFO - Met Cut  '+ str(n_met)
print 'INFO - NJet Cut '+ str(n_njet)
print 'INFO - NLep Cut '+ str(n_nlep)
print 'INFO - NTau Cut '+ str(n_ntau)
print 'INFO - NPho Cut '+ str(n_npho)
