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
    
    toProduce['ele']["RecoEgamma.ElectronIdentification.Identification.cutBasedElectronHLTPreselecition_Summer16_V1_cff"] = 1
    toProduce['ele']["RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff"] = 1
    
    switchOnVIDElectronIdProducer(process, dataFormat)
    ### # define which IDs we want to produce. it is silly to redifine them here hard coded
    for idmod in toProduce['ele']:
       print "* will produce", idmod
       setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)
    
