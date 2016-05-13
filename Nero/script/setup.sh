#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_7_6_4
# [Options] isData=False is25ns=True is50ns=False
# [fileList] /store/mc/RunIIFall15MiniAODv2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PU25nsData2015v1_76X_mcRun2_asymptotic_v12-v1/70000/FA0A72D5-C7B8-E511-8B1D-901B0E6459E0.root 
# [MaxEvents] 5000

function CMSSW_7_6_4 {
	git cms-init
	echo /CommonTools/PileupAlgos/ > .git/info/sparse-checkout
	echo /CommonTools/Utils/ >> .git/info/sparse-checkout
	echo /JetMETCorrections/Configuration/ >> .git/info/sparse-checkout
	echo /JetMETCorrections/Modules/ >> .git/info/sparse-checkout
	echo /JetMETCorrections/Type1MET/ >> .git/info/sparse-checkout
	echo /PhysicsTools/PatAlgos/ >> .git/info/sparse-checkout
	echo /PhysicsTools/PatUtils/ >> .git/info/sparse-checkout
	echo /RecoMET/METAlgorithms/ >> .git/info/sparse-checkout
	echo /RecoMET/METProducers/ >> .git/info/sparse-checkout
	echo /EgammaAnalysis/ElectronTools/ >> .git/info/sparse-checkout
	git cms-merge-topic amarini:egcorrection76x
	git cms-merge-topic cms-met:metTool76X
	git remote add blinkseb https://github.com/blinkseb/cmssw.git
	git fetch blinkseb
	git cherry-pick 4cca4688ae368bbbef2102e9bdc5bb00f6df959e
	git cms-merge-topic amarini:topic_met
	#git clone git@github.com:zdemirag/NeroProducer.git ## TO REMOVE
}

[ "X$1" == "X" ] && $1=$CMSSW_VERSION

$1
