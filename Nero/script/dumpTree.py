#! /usr/bin/env python

import os, re, array, ROOT
import sys
from optparse import OptionParser

# - S U B R O U T I N E S -----------------------------------------------------------------------------

# Print Event information
def printEventInfo(tree):
  print '{0:10s}: {1:10d} | {2:10s}: {3:10d} | {4:10s}: {5:10d}'\
  .format('Run',tree.runNum,'LumiSec',tree.lumiNum,'EventNum',tree.eventNum) 
  return

# Print MET information
def printMetInfo(tree):
  print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.1f}'\
  .format('PFMet',tree.metP4[0].Energy(),'Size',tree.metP4.GetEntriesFast()) 
  return

#Print Jet information
def printJetInfo(tree):
  # Num jets
  print '{0:10s}: {1:10d}'\
  .format('nJets',tree.jetP4.GetEntriesFast()) 
  for i in range (0,tree.jetP4.GetEntriesFast()):
    # Jet
    print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f} | {4:10s}: {5:10.4f}'\
    .format('JetPt',tree.jetP4[i].Pt(),'JetEta',tree.jetP4[i].Eta(),'JetBtag',tree.jetBdiscr[i]) 
  return

# Print FatJet information
def printFatJetInfo(tree):
  # Num jets
  print '{0:10s}: {1:10d}'\
  .format('nFatJets',tree.fatjetP4.GetEntriesFast()) 
  for i in range (0,tree.fatjetP4.GetEntriesFast()):
    # Fat Jet
    print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f}'\
    .format(' FatJetPt',tree.fatjetP4[i].Pt(),'FatJetEta',tree.fatjetP4[i].Eta()) 
    # Masses
    print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f} | {4:10s}: {5:10.4f} | {6:10s}: {7:10.4f} | {8:10s}: {9:10.4f}'\
    .format('   Mass',tree.fatjetP4[i].M(),'MassPrun',tree.fatjetPrunedMass[i],'MassFilt',tree.fatjetFilteredMass[i],'MassTrim',tree.fatjetTrimmedMass[i], 'MassSoftDrop', tree.fatjetSoftdropMass[i]) 
    # Sub-jettiness
    print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f} | {4:10s}: {5:10.4f}'\
    .format('   Tau1',tree.fatjetTau1[i],'Tau2',tree.fatjetTau2[i],'Tau3',tree.fatjetTau3[i]) 
    # Subjets
    if (tree.ak8_subjet.GetEntriesFast() > (2*i)+1):
      for j in range (2*i,(2*i)+2):
        print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f} | {4:10s}: {5:10.4f}'\
        .format('   SJetPt',tree.ak8_subjet[j].Pt(),'SJetEta',tree.ak8_subjet[j].Eta(),'SJetBtag',tree.ak8subjet_btag[j]) 
  return

# Print Lepton information
def printLepInfo(tree):
  # Num leptons
  print '{0:10s}: {1:10d}'\
  .format('nLeptons',tree.lepP4.GetEntriesFast()) 
  for i in range (0,tree.lepP4.GetEntriesFast()):
    # First Lepton
    print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f} | {4:10s}: {5:10.1f}'\
    .format('LepPt',tree.lepP4[i].Pt(),'LepEta',tree.lepP4[i].Eta(),'LepId',tree.lepPdgId[i]) 

  return

# Print Photon information
def printPhoInfo(tree):
  # Num photons
  print '{0:10s}: {1:10d}'\
  .format('nPhotons',tree.photonP4.GetEntriesFast()) 
  for i in range (0,tree.photonP4.GetEntriesFast()):
    # Photon
    print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f}'\
    .format('PhoPt',tree.photonP4[i].Pt(),'PhoEta',tree.photonP4[i].Eta()) 
  return

# Print Taus information
def printTauInfo(tree):
  # Num taus
  print '{0:10s}: {1:10d}'\
  .format('nTaus',tree.tauP4.GetEntriesFast()) 
  # Tau Lepton
  for i in range (0,tree.tauP4.GetEntriesFast()):
    print '{0:10s}: {1:10.4f} | {2:10s}: {3:10.4f}'\
    .format('TauPt',tree.tauP4[i].Pt(),'TauEta',tree.tauP4[i].Eta()) 
  return

# - M A I N ----------------------------------------------------------------------------------------

# Prepare the command line parser
parser = OptionParser()
parser.add_option("-f", "--file", dest="input_file", default='NeroNtuple.root',
                  help="input root file [default: %default]")
parser.add_option("-t", "--treename", dest="input_tree", default='events',
                  help="root tree name [default: %default]")
parser.add_option("-n", "--nprocs", dest="nprocs", type="int", default=10,
                  help="number of processed entries [default: %default]")
(options, args) = parser.parse_args()

# Get all the root classes
from ROOT import *

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
n_jet=0; n_met=0; n_njet=0; n_nlep=0; n_ntau=0; n_npho=0;

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
  print 'INFO ------------------------ Event '+str(ientry)+' ------------------------ '
  # Print event information
  #print '\n'
  printEventInfo(input_tree)
  
  # Print MET information
  #print '\n'
  printMetInfo(input_tree)
  
  # Print Jet information
  #print '\n'
  printJetInfo(input_tree)
     
  # Print FatJet information
  #print '\n'
  printFatJetInfo(input_tree)
     
  # Print Lepton information
  #print '\n'
  printLepInfo(input_tree)

  # Print Photon information
  #print '\n'
  printPhoInfo(input_tree)

  # Print Tau information
  #print '\n'
  printTauInfo(input_tree)

  print '\n'

