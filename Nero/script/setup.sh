#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_8_0_20
# [Options] isData=False
# [fileList] /store/mc/RunIISummer16MiniAODv2/QCD_Pt-15to7000_TuneCUETP8M1_Flat_13TeV_pythia8/MINIAODSIM/PUFlat0to70_magnetOn_80X_mcRun2_asymptotic_2016_TrancheIV_v4-v1/90000/FEB7D1E4-6987-E611-B656-0090FAA57B40.root 
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
	#git cms-merge-topic ikrav:egm_id_80X_v1
	git cms-merge-topic -u cms-met:fromCMSSW_8_0_20_postICHEPfilter
    git cms-merge-topic ahinzmann:METRecipe_8020_Moriond17
	git cms-merge-topic cms-met:METRecipe_8020
    git cms-merge-topic emanueledimarco:ecal_smear_fix_80X
    git clone -b ICHEP2016_v2 https://github.com/ECALELFS/ScalesSmearings.git EgammaAnalysis/ElectronTools/data/ScalesSmearings
	git cms-merge-topic ikrav:egm_id_80X_v2
	git clone -b egm_id_80X_v1 https://github.com/ikrav/RecoEgamma-ElectronIdentification.git RecoEgamma/ElectronIdentification/data.new
	rsync -avP RecoEgamma/ElectronIdentification/data.new/* RecoEgamma/ElectronIdentification/data/
}

function CMSSW_9_0_0_pre1 {
	git cms-init
}

# expected default
[ "X$1" == "X" ] && $1=$CMSSW_VERSION
$1
