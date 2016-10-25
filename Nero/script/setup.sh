#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_8_0_12
# [Options] isData=False
# [fileList] /store/mc/RunIISpring16MiniAODv1/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUFlat0to50_80X_mcRun2_asymptotic_2016_v3-v1/20000/626CD584-6AF3-E511-986F-001E67DDBEDA.root
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

## CMSSW_8_0_10 removed because metTool80X will merge all CMSSW
## >CMSSW_8_0_13 conflicts there

function CMSSW_8_0_12 {
	git cms-init
        #echo /PhysicsTools/PatUtils/ >> .git/info/sparse-checkout
	git cms-merge-topic cms-met:metTool80X
	git cms-merge-topic -u cms-met:CMSSW_8_0_X-METFilterUpdate
	git cms-merge-topic emanueledimarco:ecal_smear_fix_80X
	git clone -b ICHEP2016_v2 https://github.com/ECALELFS/ScalesSmearings.git EgammaAnalysis/ElectronTools/data/ScalesSmearings
}

function CMSSW_8_0_20 {
	git cms-init
	git cms-merge-topic ikrav:egm_id_80X_v1
}

# expected default
[ "X$1" == "X" ] && $1=$CMSSW_VERSION
$1
