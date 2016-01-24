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
parser.add_option("-n", "--nprocs", dest="nprocs", type="int", default=-1,
                  help="number of processed entries [default: %default]")
parser.add_option("-d","--debug", dest="debug", action="store_true", default=False,
                  help="Show debugging information")
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

# ===========================================================================

def deltaBetaIsolation(tree,i):
  return tree.lepChIso[i] + max(0., tree.lepPhoIso[i] + tree.lepNhIso[i] - 0.5*tree.lepPuIso[i]);

def isLooseMuon(tree,i):
  # https://twiki.cern.ch/twiki/bin/viewauth/CMS/Monojet
  if abs(tree.lepPdgId[i]) != 13: return False
  if abs(tree.lepP4[i].Eta()) > 2.4: return False
  # ID
  if (tree.lepSelBits[i] & 16) != 16: 
    if options.debug: print "failed loose muon id"
    return False
  # Isolation
  if tree.lepIso[i]/tree.lepP4[i].Pt() > 0.20: 
  # if deltaBetaIsolation(tree,i)/tree.lepP4[i].Pt() > 0.20: 
    if options.debug: print "failed loose muon isolation"
    return False
  if options.debug: print "Loose muon is found"
  return True

def isLooseElectron(tree,i):
  # https://twiki.cern.ch/twiki/bin/viewauth/CMS/Monojet
  # WARNING: we need super cluster Eta, not P4
  if abs(tree.lepPdgId[i]) != 11: return False
  if abs(tree.lepP4[i].Eta()) > 2.5: return False
  # ID
  if (tree.lepSelBits[i] & 2) != 2: 
    if options.debug: print "failed loose electron id"
    return False
  # Isolation
  if options.debug:
    print "lepIso[i]: %0.4f" % tree.lepIso[i]
    print "lepP4[i].Pt(): %0.4f" % tree.lepP4[i].Pt()
  relIso = tree.lepIso[i]/tree.lepP4[i].Pt()
  if abs(tree.lepP4[i].Eta()) > 1.479:
  # if (tree.lepSelBits[i] & 32768) != 32768:
    # endcap
    if relIso > 0.144: 
      if options.debug: print "failed loose electron iso endcap"
      return False
  else:
    # barrel
    if relIso > 0.126: 
      if options.debug: print "failed loose electron iso"
      return False
  if options.debug: print "Loose electron is found"
  return True

def isLooseLepton(tree,i):
  if tree.lepP4[i].Pt() < 10: return False
  return isLooseMuon(tree,i) or isLooseElectron(tree,i)

def getNLooseLeptons(tree):
  nLeptons = tree.lepP4.GetEntriesFast()
  if options.debug: print "nLeptons: ", nLeptons
  nLooseLeptons = 0
  for i in range(nLeptons):
    if isLooseLepton(tree,i):
      nLooseLeptons+=1
  return nLooseLeptons

def isGoodTau(tree,i):
  if tree.tauP4[i].Pt() < 18. or abs(tree.tauP4[i].Eta()) > 2.3: return False
  if (tree.tauSelBits[i]&6) != 6: return False
  if tree.tauIsoDeltaBetaCorr[i] > 5: return False
  # warning: for Bambu version use 4.5 
  return True

def getNTaus(tree):
  n = tree.tauP4.GetEntriesFast()
  nTaus = 0
  for i in range(n):
    if isGoodTau(tree,i):
      nTaus+=1
  return nTaus

def isLoosePhoton(tree,i):
  if tree.photonP4[i].Pt()<15 or abs(tree.photonP4[i].Eta()) > 2.5: return False
  if (tree.photonSelBits[i] & 8) != 8: 
    return False
  return True

def getNLoosePhotons(tree):
  n = tree.photonP4.GetEntriesFast()
  nSelected = 0
  for i in range(n):
    if isLoosePhoton(tree,i):
      nSelected+=1
  return nSelected

def isGoodJet(tree,i):
  return True

def isBJet(tree,i):
  if tree.jetP4[i].Pt()<15 or abs(tree.jetP4[i].Eta()) > 2.5: return False
  # if (tree.jetSelBits[i] & 16) != 16: return False
  if tree.jetPuId[i] <= -0.63: return False
  if tree.jetBdiscr[i] <= 0.89: return False
  # if (tree.
  #   return False
  return True

def getNBJets(tree):
  n = tree.jetP4.GetEntriesFast()
  nSelected = 0
  for i in range(n):
    if isBJet(tree,i):
      nSelected+=1
  return nSelected

# ===========================================================================

#initialize 
n_jet=0; n_met=0; n_njet=0; n_nlep=0; n_ntau=0; n_npho=0; n_dphi=0;n_2ndjet=0;
n_nbjet=0

# Check the number of entries in the tree
n_entries = input_tree.GetEntriesFast()
print 'INFO - Input tree entries: ' + str(n_entries)

# Determine number of entries to process
if options.nprocs > 0 and options.nprocs < n_entries:
  n_entries = options.nprocs

print 'INFO - Number of entries to be processed: ' + str(n_entries)

# Loop over the entries
for ientry in range(0,n_entries):
  # if ientry!=104: continue
  # Grab the n'th entry
  input_tree.GetEntry(ientry)

  #print deltaPhi(input_tree.jetP4[0].Phi(),input_tree.jetP4[1].Phi() )
  #print 'INFO ------------------------ Event '+str(ientry)+' ------------------------ '

  dphi = -10000.

  #  if(input_tree.metP4[0].Energy() > 200 ):  
  #  n_met += 1
  # if getNLooseLeptons(input_tree)>0:
  if getNLooseLeptons(input_tree)==0:
    if options.debug: print "%d (%d) \t Event: %d" % (ientry,n_nlep,input_tree.eventNum)
    n_nlep += 1
    if getNTaus(input_tree)==0:
      n_ntau += 1
      if getNLoosePhotons(input_tree)==0:
        n_npho += 1
        if getNBJets(input_tree)==0:
          n_nbjet += 1

  # Broken
  # if(input_tree.jetP4.GetEntriesFast() > 1 and input_tree.jetP4[0].Pt() > 110 and input_tree.jetMonojetId[0] and input_tree.jetMonojetIdLoose[1]) : # and input_tree.jetPuId() > -0.62) : 
  #   dphi = deltaPhi( input_tree.jetP4[0].Phi(),input_tree.jetP4[1].Phi() )
  # else:
  #    if (input_tree.jetP4.GetEntriesFast() < 2):
  #      dphi= -99.999
  #    else:
  #      dphi = -10000.

  # if (input_tree.jetP4.GetEntriesFast() > 0 and input_tree.jetP4[0].Pt() > 110 and input_tree.jetMonojetId[0]):
  #   n_jet += 1
  #   if (input_tree.jetP4.GetEntriesFast() == 1 or (input_tree.jetP4.GetEntriesFast() > 1 and input_tree.jetMonojetIdLoose[1])):
  #     n_2ndjet += 1
  #     if( fabs(dphi) < 2.5  or dphi == -99.999 ):
  #       n_dphi += 1
  #       if(input_tree.metP4[0].Energy() > 200 ):  
  #         n_met += 1
  #         if(input_tree.jetP4.GetEntriesFast() < 3): 
  #           n_njet += 1
  #           if(input_tree.lepP4.GetEntriesFast() < 1): 
  #             n_nlep += 1
  #               if(input_tree.photonP4.GetEntriesFast() < 1):
  #                 n_npho += 1

print 'INFO - Cut Flow Chart: '
print 'INFO - FULL     '+ str(input_all.GetEntries() )
print 'INFO - GoodVtx  '+ str(n_entries)  ,; PrintCompare(n_entries,50492)
print 'INFO - NLep Cut '+ str(n_nlep)     ,; PrintCompare(n_nlep,33610)
print 'INFO - NTau Cut '+ str(n_ntau)     ,; PrintCompare(n_ntau,27598)
print 'INFO - NPho Cut '+ str(n_npho)     ,; PrintCompare(n_npho,26806)
print 'INFO - NBJet Cut '+ str(n_nbjet)   ,; PrintCompare(n_nbjet,5180)

print 'INFO - Jet Cut  '+ str(n_jet)      ,; PrintCompare(n_jet,1995)
print 'INFO - 2_j Cut  '+ str(n_2ndjet)   ,; PrintCompare(n_2ndjet,1990)
print 'INFO - Phi Cut  '+ str(n_dphi)     ,; PrintCompare(n_dphi,1727)
print 'INFO - Met Cut  '+ str(n_met)      ,; PrintCompare(n_met,996)
print 'INFO - NJet Cut '+ str(n_njet)     ,; PrintCompare(n_njet,644)
