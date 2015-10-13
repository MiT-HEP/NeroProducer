#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_7_4_12_patch4
# [Options] isData=True is25ns=True is50ns=False

#CMSSW_7_4_5: 
function CMSSW_7_4_5 {
	git cms-merge-topic -u cms-btv-pog:BoostedDoubleSVTagger-WithWeightFiles-v2_from-CMSSW_7_4_1
} 

#CMSSW_7_4_12
function CMSSW_7_4_12_patch4 { 
	true
}
##
function CMSSW_7_4_14{
	true
}

function CMSSW_7_4_15{
	true
}

# expected default
$1
