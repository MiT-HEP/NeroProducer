#!/bin/bash
[ "${WORKDIR}" == "" ] && { mkdir -p /tmp/$USER/ ; export WORKDIR=/tmp/${USER}/; }
cd /afs/cern.ch/work/j/jsalfeld/MIT/vdmAnalysis/CMSSW_7_4_5/src/NeroProducer/Nero
eval `scramv1 runtime -sh`
cd chargedHiggs2
rm sub_69.fail || true
rm sub_69.done || true
rm sub_69.txt || true
touch sub_69.run
cd $WORKDIR
cmsRun /afs/cern.ch/work/j/jsalfeld/MIT/vdmAnalysis/CMSSW_7_4_5/src/NeroProducer/Nero/chargedHiggs2/NeroProducer_69.py
EXIT=$?
cd /afs/cern.ch/work/j/jsalfeld/MIT/vdmAnalysis/CMSSW_7_4_5/src/NeroProducer/Nero
cd chargedHiggs2
rm sub_69.run
echo "exit status is ${EXIT}"
[ "${EXIT}" == "0" ] && { cmsMkdir /store/user/jsalfeld/ChargedHiggs/NeroTuples/Hplus_Wh_26_6_2015/  && cmsStage ${WORKDIR}/NeroNtuples.root /store/user/jsalfeld/ChargedHiggs/NeroTuples/Hplus_Wh_26_6_2015//NeroNtuples_69.root  && touch sub_69.done || echo "cmsStage fail" > sub_69.fail; }
[ "${EXIT}" == "0" ] || echo ${EXIT} > sub_69.fail

