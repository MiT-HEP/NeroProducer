#!/bin/bash

# Instruct builder to use a particular CMSSW release
# [CMSSW] CMSSW_10_6_13
# [Options] isData=False
# [fileList] /store/mc/RunIIAutumn18MiniAOD/DYJetsToLL_M-105To160_TuneCP5_PSweights_13TeV-amcatnloFXFX-pythia8/MINIAODSIM/102X_upgrade2018_realistic_v15-v1/100000/48A13F85-97BC-8348-A555-9AC79DF6628B.root
# [MaxEvents] 5000
# [Tag] CMSSW_106X




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

function CMSSW_10_2_15 {
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
        #git cms-merge-topic amarini:fix_102x_met_ptdepjer
        git cms-merge-topic amarini:fix_102_15_met_ptdepjer
        git clone ssh://git@gitlab.cern.ch:7999/uhh-cmssw/fsr-photon-recovery.git FSRPhotonRecovery

}

function CMSSW_10_6_13 {
        git cms-init

        #TODO
        git cms-merge-topic amarini:topic_qgmorevar_106X 
        
        # CHECK if is working
        git clone ssh://git@gitlab.cern.ch:7999/uhh-cmssw/fsr-photon-recovery.git FSRPhotonRecovery
        
        ### CLEAN UP
        git cms-merge-topic jainshilpi:ULV1_backport106X_forUsers
        git clone https://github.com/jainshilpi/EgammaPostRecoTools.git -b ULV0  ./EgammaPostRecoTools_tmp
        mv EgammaPostRecoTools_tmp/python/EgammaPostRecoTools.py RecoEgamma/EgammaTools/python/.
        rm -rf EgammaPostRecoTools_tmp
        git cms-addpkg EgammaAnalysis/ElectronTools
        rm EgammaAnalysis/ElectronTools/data -rf
        git clone https://github.com/jainshilpi/EgammaAnalysis-ElectronTools.git -b UL2017SSV2 EgammaAnalysis/ElectronTools/data/
}


# expected default
[ "X$1" == "X" ] && $1=$CMSSW_VERSION
$1
