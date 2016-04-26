#!/bin/bash

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
git clone git@github.com:zdemirag/NeroProducer.git
