#!/bin/bash
[ "${WORKDIR}" == "" ] && { mkdir -p /tmp/$USER/ ; export WORKDIR=/tmp/${USER}/; }
cd /afs/cern.ch/work/j/jsalfeld/MIT/vdmAnalysis/CMSSW_7_4_5/src/NeroProducer/Nero
eval `scramv1 runtime -sh`
cd chargedHiggs2
rm sub_68.fail || true
rm sub_68.done || true
rm sub_68.txt || true
touch sub_68.run
cd $WORKDIR
cmsRun /afs/cern.ch/work/j/jsalfeld/MIT/vdmAnalysis/CMSSW_7_4_5/src/NeroProducer/Nero/chargedHiggs2/NeroProducer_68.py
EXIT=$?
cd /afs/cern.ch/work/j/jsalfeld/MIT/vdmAnalysis/CMSSW_7_4_5/src/NeroProducer/Nero
cd chargedHiggs2
rm sub_68.run
echo "exit status is ${EXIT}"
[ "${EXIT}" == "0" ] && { cmsMkdir /store/user/jsalfeld/ChargedHiggs/NeroTuples/Hplus_Wh_26_6_2015/  && cmsStage ${WORKDIR}/NeroNtuples.root /store/user/jsalfeld/ChargedHiggs/NeroTuples/Hplus_Wh_26_6_2015//NeroNtuples_68.root  && touch sub_68.done || echo "cmsStage fail" > sub_68.fail; }
[ "${EXIT}" == "0" ] || echo ${EXIT} > sub_68.fail

