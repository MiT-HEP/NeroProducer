import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
import re,os

def initEGammaVID(process, options):
	replace = {'bx' : '25ns', 'vs' : 'V1'}
	## if options.is25ns:
	## 	replace = {'bx' : '25ns', 'vs' : 'V1'}
	## if options.is50ns:
	## 	replace = {'bx' : '50ns', 'vs' : 'V2'}
	
	toProduce={}
	for obj in ['ele','pho']:
	  toProduce[obj]={}
	  if obj=='ele': directory = 'RecoEgamma.ElectronIdentification'
	  if obj=='pho': directory = 'RecoEgamma.PhotonIdentification'
	  for ID in ['veto','medium','loose','tight']:
	      if obj == 'pho' and ID == 'veto' : continue
	      if obj == 'pho' : 
	           replace['vs'] = 'V2p2'
	
	      replace['id'] = ID
	      cmd = 'string = process.nero.' + obj + ID.title() + 'IdMap.value()'
	      exec(cmd)
	      cmd = 'process.nero.'+obj + ID.title() + 'IdMap = cms.InputTag("' + string % replace+ '")'
	      print 'executing replacement:',cmd
	      exec(cmd)
	
	      myid = (string%replace ).replace('-','_').split(':')[1]
	      myid = re.sub('_standalone.*','',myid)
	      if obj=="ele":
		      myid="cutBasedElectronID_Summer16_80X_%(vs)s"%replace
	      #toProduce[obj][ directory + '.Identification.' + myid + "_cff"] = 1 #remove duplicates
	      if obj != 'pho': ## photon is different in Spring16, added below 
	      	toProduce[obj][ directory + '.Identification.' + myid + "_cff"] = 1 #remove duplicates
	### INIT MODULES
	dataFormat = DataFormat.MiniAOD

	## add the NonTrigValueMap
	toProduce['ele']["RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff"] = 1
	toProduce['ele']["RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff"] = 1
	toProduce['pho']['RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Spring16_V2p2_cff'] =1

	switchOnVIDElectronIdProducer(process, dataFormat)
	### # define which IDs we want to produce. it is silly to redifine them here hard coded
	for idmod in toProduce['ele']:
	   print "will produce", idmod
	   setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
	### ### PHOTONS
	switchOnVIDPhotonIdProducer(process, dataFormat) ### PHOTON
	for idmod in toProduce['pho']:
	      print "will produce", idmod
	      setupAllVIDIdsInModule(process,idmod,setupVIDPhotonSelection)
	
