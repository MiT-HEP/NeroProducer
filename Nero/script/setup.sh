#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_10_2_9
# [Options] isData=False
# [fileList] /store/mc/RunIIAutumn18MiniAOD/DYJetsToLL_M-105To160_TuneCP5_PSweights_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/100000/48A13F85-97BC-8348-A555-9AC79DF6628B.root
# [MaxEvents] 5000
# [Tag] CMSSW_102X

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
        git cms-merge-topic cms-egamma:EgammaID_949
        ## additional variables for qg: (axis1,cmult,nmult). Optional 
        git cms-merge-topic amarini:topic_qgmorvar_94X 
        git cms-merge-topic cms-met:METFixEE2017_949_v2
        git clone ssh://git@gitlab.cern.ch:7999/uhh-cmssw/fsr-photon-recovery.git FSRPhotonRecovery
        #git clone -b final_CMSSW_9_4_9 ssh://git@gitlab.cern.ch:7999/uhh-cmssw/CAST.git
        #cd CAST &&  echo "/.gitignore" > .git/info/sparse_checkout && echo "/FSRPhotons/" >> .git/info/sparse-checkout && git read-tree -mu HEAD && cd -
}


#git cms-merge-topic cms-egamma:EgammaPostRecoTools #just adds in an extra file to have a setup function to make things easier 
#git cms-merge-topic cms-egamma:PhotonIDValueMapSpeedup1029 #optional but speeds up the photon ID value module so things fun faster
#git cms-merge-topic cms-egamma:slava77-btvDictFix_10210 #fixes the Run2018D dictionary issue, see https://github.com/cms-sw/cmssw/issues/26182, may not be necessary for later releases, try it first and see if it works
#git cms-addpkg EgammaAnalysis/ElectronTools
#rm EgammaAnalysis/ElectronTools/data -rf
#git clone git@github.com:cms-data/EgammaAnalysis-ElectronTools.git EgammaAnalysis/ElectronTools/data

function CMSSW_10_2_9 {
        git cms-init
        git cms-merge-topic cms-egamma:EgammaPostRecoTools
        git cms-merge-topic cms-egamma:EgammaID_1023
        git cms-merge-topic cms-egamma:slava77-btvDictFix_10210 #fixes the Run2018D dictionary issue, see https://github.com/cms-sw/cmssw/issues/26182, may not be necessary for later releases, try it first and see if it works
        #now to add the scale and smearing for 2018 (eventually this will not be necessary in later releases but is harmless to do regardless)
        git cms-addpkg EgammaAnalysis/ElectronTools
        rm EgammaAnalysis/ElectronTools/data -rf
        git clone git@github.com:cms-data/EgammaAnalysis-ElectronTools.git EgammaAnalysis/ElectronTools/data
        git cms-merge-topic cms-met:METFixEE2017_949_v2_backport_to_102X
        ## additional variables for qg: (axis1,cmult,nmult). Optional 
        git cms-merge-topic amarini:topic_qgmorvar_102X 
        git cms-merge-topic amarini:fix_102x_met_ptdepjer
        git clone ssh://git@gitlab.cern.ch:7999/uhh-cmssw/fsr-photon-recovery.git FSRPhotonRecovery

        cd FSRPhotonRecovery &&  patch -l -p1 <<EOF
diff --git a/FSRPhotons/plugins/FSRRecoveryProducer.cc b/FSRPhotons/plugins/FSRRecoveryProducer.cc
index 33994ce..2b8dd24 100644
--- a/FSRPhotons/plugins/FSRRecoveryProducer.cc
+++ b/FSRPhotons/plugins/FSRRecoveryProducer.cc
@@ -129,7 +129,8 @@ std::vector<pat::PFParticle> FSRRecoveryProducer::MakeHybridPhotons(
        pat::PackedCandidate associatedPhotonMatch = *(patpho.associatedPackedPFCandidates().at(matching_index));
        double dR = deltaR(hybridpho.p4(), associatedPhotonMatch.p4());
        if(abs(hybridpho.pt() - associatedPhotonMatch.pt()) < 1e-3 && dR < 0.05){
-         math::XYZTLorentzVector pho_corrP4 = patpho.p4() * patpho.userFloat("ecalEnergyPostCorr") / patpho.energy();
+         //math::XYZTLorentzVector pho_corrP4 = patpho.p4() * patpho.userFloat("ecalEnergyPostCorr") / patpho.energy();
+         math::XYZTLorentzVector pho_corrP4 = patpho.p4() / patpho.energy();
          if(modifyP4){
            hybridpho.setP4(pho_corrP4);
          }
EOF
}


# expected default
[ "X$1" == "X" ] && $1=$CMSSW_VERSION
$1
