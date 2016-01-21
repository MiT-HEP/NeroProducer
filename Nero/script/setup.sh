#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_7_6_3_patch2
# [Options] isData=False is25ns=True is50ns=False
# [fileList] /store/mc/RunIIFall15DR76/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/25nsFlat10to25TSG_HCALDebug_76X_mcRun2_asymptotic_v11-v1/40000/FEEBF471-F987-E511-BC13-002590D0B002.root,/store/mc/RunIIFall15DR76/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/25nsFlat10to25TSG_HCALDebug_76X_mcRun2_asymptotic_v11-v1/40000/D0A3314C-1788-E511-9A86-002590D0B098.root
# [MaxEvents] 5000

#CMSSW_7_4_12
function CMSSW_7_4_12_patch4 { 
	true
}
##
function CMSSW_7_4_14 {
	true
}

function CMSSW_7_4_15 {
	true
}

function CMSSW_7_6_3_patch2 {
	true
}

# expected default
$1
