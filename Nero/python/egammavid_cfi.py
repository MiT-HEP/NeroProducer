import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing
from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
import re,os

def initEGammaVID(process, options):
    ### INIT MODULES
    dataFormat = DataFormat.MiniAOD

    ## add the NonTrigValueMap
    toProduce={}
    toProduce['ele']={}
    toProduce['pho']={}
    
    #toProduce['ele']["RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff"] = 1
    #toProduce['ele']["RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff"] = 1
    toProduce['ele']["RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_iso_V1_cff"] = 1
    toProduce['ele']['RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V1_cff'] = 1
    
    switchOnVIDElectronIdProducer(process, dataFormat)
    ### # define which IDs we want to produce. it is silly to redifine them here hard coded
    for idmod in toProduce['ele']:
       print "* will produce", idmod
       setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
   
##ele_id_modules =  [ 'RecoEgamma.ElectronIdentification.Identification.heepElectronID_HEEPV70_cff',
##                    'RecoEgamma.ElectronIdentification.Identification.cutBasedElectronID_Fall17_94X_V1_cff',
##                    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_noIso_V1_cff', 
##                    'RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Fall17_iso_V1_cff'
##                  ]
##pho_id_modules =  [ 'RecoEgamma.PhotonIdentification.Identification.cutBasedPhotonID_Fall17_94X_V1_TrueVtx_cff.py',
##                    'RecoEgamma.PhotonIdentification.Identification.mvaPhotonID_Fall17_v1_cff',  
##                  ] 
