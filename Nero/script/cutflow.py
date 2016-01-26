#! /usr/bin/env python
# Synchronized to MonoJet analysis on 25/01/2016 
# https://twiki.cern.ch/twiki/bin/view/CMS/Monojet?rev=54
# Input dataset: /TTbarDMJets_pseudoscalar_Mchi-1_Mphi-100_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/RunIISpring15MiniAODv2-74X_mcRun2_asymptotic_v2-v1/MINIAODSIM

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
  if (tree.jetSelBits[i] & 8) != 8: return False
  if tree.jetPuId[i] <= -0.63: return False
  if tree.jetBdiscr[i] <= 0.89: return False
  return True

def getNBJets(tree):
  n = tree.jetP4.GetEntriesFast()
  nSelected = 0
  for i in range(n):
    if isBJet(tree,i):
      nSelected+=1
  return nSelected

def isGoodLooseJet(tree,i):
  if tree.jetP4[i].Pt()<30 or abs(tree.jetP4[i].Eta()) > 2.5: return False
  if (tree.jetSelBits[i] & 8) != 8: return False
  if tree.jetPuId[i] <= -0.63: return False
  return True
  
def findLeadingJet(tree):
  for i in range(tree.jetP4.GetEntriesFast()):
    if not isGoodLooseJet(tree,i): continue
    if tree.jetP4[i].Pt()>100: 
      return i
    else:
      return None
  return None

def isCleanJet(tree,i):
  if (tree.jetSelBits[i] & 1024) != 1024: return False
  return True

def deltaPhi(phi1,phi2):
  return acos(cos(phi2-phi1))


def passedDPhi(tree):
  nGoodJets = 0
  matched = False
  for i in range(tree.jetP4.GetEntriesFast()):
    if tree.jetP4[i].Pt()>30 and abs(tree.jetP4[i].Eta()) < 2.5 and \
          (tree.jetSelBits[i] & 8) == 8: 
      nGoodJets += 1 
      if nGoodJets > 4: break
      if deltaPhi(tree.jetP4[i].Phi(),tree.metP4[0].Phi())<0.5: 
        matched = True
  if matched: 
    return False
  else:
    return True

# ===========================================================================

#initialize 
n_jet=0; n_met=0; n_njet=0; n_nlep=0; n_ntau=0; n_npho=0; n_dphi=0;n_2ndjet=0;
n_nbjet=0
n_cleanjet = 0

# Check the number of entries in the tree
n_entries = input_tree.GetEntriesFast()
print 'INFO - Input tree entries: ' + str(n_entries)

# Determine number of entries to process
if options.nprocs > 0 and options.nprocs < n_entries:
  n_entries = options.nprocs

print 'INFO - Number of entries to be processed: ' + str(n_entries)

# Loop over the entries
for ientry in range(0,n_entries):
  input_tree.GetEntry(ientry)

  if getNLooseLeptons(input_tree)==0:
    if options.debug: print "%d (%d) \t Event: %d" % (ientry,n_nlep,input_tree.eventNum)
    n_nlep += 1
    if getNTaus(input_tree)==0:
      n_ntau += 1
      if getNLoosePhotons(input_tree)==0:
        n_npho += 1
        if getNBJets(input_tree)==0:
          n_nbjet += 1
          if input_tree.metP4[0].Pt() > 200:  
            n_met += 1
            leading_jet = findLeadingJet(input_tree)
            if leading_jet != None:
              n_jet += 1
              if isCleanJet(input_tree,leading_jet):
                n_cleanjet += 1
                if passedDPhi(input_tree):
                  n_dphi += 1

print 'INFO - Cut Flow Chart: '
print 'INFO - FULL         '+ str(input_all.GetEntries() )
print 'INFO - GoodVtx      '+ str(n_entries)  ,; PrintCompare(n_entries,50492)
print 'INFO - NLep Cut     '+ str(n_nlep)     ,; PrintCompare(n_nlep,33610)
print 'INFO - NTau Cut     '+ str(n_ntau)     ,; PrintCompare(n_ntau,27598)
print 'INFO - NPho Cut     '+ str(n_npho)     ,; PrintCompare(n_npho,26806)
print 'INFO - NBJet Cut    '+ str(n_nbjet)    ,; PrintCompare(n_nbjet,5167)
print 'INFO - Met Cut      '+ str(n_met)      ,; PrintCompare(n_met,1461)
print 'INFO - Leading Jet  '+ str(n_jet)      ,; PrintCompare(n_jet,1318)
print 'INFO - Jet Cleaning '+ str(n_cleanjet) ,; PrintCompare(n_cleanjet,1303)
print 'INFO - Phi Cut      '+ str(n_dphi)     ,; PrintCompare(n_dphi,1044)
