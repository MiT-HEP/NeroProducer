#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_7_4_14
# [Options] isData=False is25ns=True is50ns=False
# [fileList] /store/mc/RunIISpring15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/74X_mcRun2_asymptotic_v2-v1/50000/E4F89698-DE6E-E511-8681-0025905A60F4.root 
# [MaxEvents] 5000

#CMSSW_7_4_5: 
function CMSSW_7_4_5 {
	git cms-merge-topic -u cms-btv-pog:BoostedDoubleSVTagger-WithWeightFiles-v2_from-CMSSW_7_4_1
} 

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

# expected default
$1
