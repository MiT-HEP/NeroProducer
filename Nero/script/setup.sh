#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_7_6_3_patch2
# [Options] isData=False is25ns=True is50ns=False
# [fileList] /store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA0A72D5-C7B8-E511-8B1D-901B0E6459E0.root 
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
