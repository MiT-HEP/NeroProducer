#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_9_4_9
# [Options] isData=False
# [fileList] /store/user/amarini/Sync/5AC9148F-9842-E811-892B-3417EBE535DA.root
# [MaxEvents] 5000
# [Tag] CMSSW_92X

function CMSSW_8_0_26_patch1 {
        git cms-init
        git cms-merge-topic zdemirag:conflict_met_resolved
        #git cms-merge-topic emanueledimarco:ecal_smear_fix_80X
        git cms-merge-topic rafaellopesdesa:Regression80XEgammaAnalysis_v2
        git cms-merge-topic rafaellopesdesa:RegressionCheckNegEnergy
        git cms-merge-topic shervin86:Moriond17_23Jan
        git cms-merge-topic ikrav:egm_id_80X_v2
        git clone -b egm_id_80X_v1 https://github.com/ikrav/RecoEgamma-ElectronIdentification.git RecoEgamma/ElectronIdentification/data.new
        git cms-addpkg RecoEgamma/ElectronIdentification
        rsync -avP RecoEgamma/ElectronIdentification/data.new/* RecoEgamma/ElectronIdentification/data/
        git cms-merge-topic ikrav:egm_id_80X_v3_photons
        git clone -b egm_id_80X_v1 https://github.com/ikrav/RecoEgamma-PhotonIdentification.git RecoEgamma/PhotonIdentification/data.new
        rsync -avP RecoEgamma/PhotonIdentification/data.new/* RecoEgamma/PhotonIdentification/data/
        git cms-addpkg EgammaAnalysis/ElectronTools
        git cms-merge-topic shervin86:Moriond2017_JEC_energyScales
        git clone git@github.com:ECALELFS/ScalesSmearings.git EgammaAnalysis/ElectronTools/data/ScalesSmearings.new
        rsync -avP EgammaAnalysis/ElectronTools/data/ScalesSmearings.new/* EgammaAnalysis/ElectronTools/data/ScalesSmearings/
        git cms-merge-topic amarini:topic_qgmorevar
        git cms-merge-topic amarini:eleMva_exception
}

function CMSSW_8_0_28_patch1 {
        git cms-init
        git cms-merge-topic ikrav:egm_id_80X_v3_photons_rebasedTo_8026patch2
        git cms-merge-topic ikrav:egm_id_80X_v2
}

function CMSSW_9_2_4 {
        git cms-init
        #git cms-merge-topic ikrav:egm_id_80X_v3_photons_rebasedTo_CMSSW_9_2_X_2017-05-29-1100
        #git cms-merge-topic ikrav:egm_id_80X_v2_rebased_CMSSW_9_0_X_2016-12-07-2300
}

function CMSSW_9_4_1 {
        git cms-init
        git cms-merge-topic lsoffi:CMSSW_9_4_0_pre3_TnP
        git cms-merge-topic guitargeek:ElectronID_MVA2017_940pre3
        git clone https://github.com/lsoffi/RecoEgamma-PhotonIdentification.git RecoEgamma/PhotonIdentification/data.new -b CMSSW_9_4_0_pre3_TnP
        git clone https://github.com/lsoffi/RecoEgamma-ElectronIdentification.git RecoEgamma/ElectronIdentification/data.new -b CMSSW_9_4_0_pre3_TnP
        rsync -avP RecoEgamma/ElectronIdentification/data.new/* RecoEgamma/ElectronIdentification/data/
        rsync -avP RecoEgamma/PhotonIdentification/data.new/* RecoEgamma/PhotonIdentification/data/
        #git cms-merge-topic ikrav:egm_id_80X_v3_photons_rebasedTo_CMSSW_9_2_X_2017-05-29-1100
        #git cms-merge-topic ikrav:egm_id_80X_v2_rebased_CMSSW_9_0_X_2016-12-07-2300
        #add the repository with the updated Egamma package
        git cms-merge-topic cms-egamma:EGM_94X_v1
        # download the txt files with the corrections
        git clone https://github.com/ECALELFS/ScalesSmearings.git EgammaAnalysis/ElectronTools/data/ScalesSmearings -b Run2017_17Nov2017_v1
        #rsync -avP EgammaAnalysis/ElectronTools/data/ScalesSmearings.new/* EgammaAnalysis/ElectronTools/data/ScalesSmearings/
        git cms-merge-topic amarini:topic_qgmorvar_94X 
}

function CMSSW_9_4_9 {
        git cms-init
        ## postReco for EGamma
        git cms-merge-topic cms-egamma:EgammaPostRecoTools_940
        ## additional variables for qg: (axis1,cmult,nmult). Optional 
        git cms-merge-topic amarini:topic_qgmorvar_94X 
        git cms-merge-topic cms-met:METFixEE2017_949_v2
}

function CMSSW_10_1_6 {
        git cms-init
}


# expected default
[ "X$1" == "X" ] && $1=$CMSSW_VERSION
$1
