#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_8_0_3_patch3
# [Options] isData=False is25ns=True is50ns=False
# [fileList] /store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/626CD584-6AF3-E511-986F-001E67DDBEDA.root
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
	git cms-merge-topic matteosan1:smearer_76X
}

function CMSSW_8_0_3_patch3 {
	git cms-merge-topic matteosan1:smearer_76X
}

# expected default
$1
