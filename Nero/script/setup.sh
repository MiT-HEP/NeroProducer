#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_7_4_5

#CMSSW_7_4_2:
function CMSSW_7_4_2 {
	git cms-merge-topic ikrav:egm_id_74X_v0
}

#CMSSW_7_4_5: 
function CMSSW_7_4_5 {
	git cms-merge-topic -u cms-btv-pog:BoostedDoubleSVTagger-WithWeightFiles-v2_from-CMSSW_7_4_1
} 

function CMSSW_7_4_12_patch4 { 
	true  
}

# expected default
$1
