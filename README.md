# NeroProducer
NtuplE fRamewOrk

### Table of Contents
- [Directories](#user-content-directories)
- [Nero](#user-content-nero)
- [CMSSW](#user-content-cmssw)
- [Crab](#user-content-crab-submit)
- [Batch](#user-content-batch-submit)
- [Core](#user-content-core)
- [Skim](#user-content-skim)

## Directories
* Core -> ROOT/C++ structure of the ntuples.
* Nero -> CMSSW plugins and MiniAOD code.
* Skim -> Skimmer for the MiniAOD. 
* Bambu -> Code to produce from Bambu. 

## Nero
The Nero directory contains the ntuple producer. 
It is designed to run on MiniAOD within the CMSSW framework.


### CMSSW compile
* Notice that cmssw should be init when the src directory is empty.
Currently the supported version are CMSSW\_7\_4\_15\_patch1 (checking out the branch 74X) and CMSSW\_7\_6\_3\_patch2 (master).
```
cmsrel CMSSW_7_6_4
cd CMSSW_7_6_4/src
cmsenv
wget --no-check-certificate 'https://raw.githubusercontent.com/MiT-HEP/NeroProducer/master/Nero/script/setup.sh' -O /tmp/$USER/setup.sh
source /tmp/$USER/setup.sh
```
* Clone the git repository and compile it
```
cd ${CMSSW_BASE}/src
git clone git@github.com:MiT-HEP/NeroProducer.git
cd ${CMSSW_BASE}/srcNeroProducer/Nero
scram b -j 16
cd ${CMSSW_BASE}/src/NeroProducer/Skim
scram b -j 16
cd ${CMSSW_BASE}/src/NeroProducer/Core
scram b -j 16
```

### How to Run
If running on data, set the isData flag to True, else False.
```
cd NeroProducer/Nero/test
cmsRun testNero.py isData=True
```

To check the simple cut flow for the monojet analysis
```
cd NeroProducer/Nero/scripts
./cutflow.py -f FILENAME -n NUMBEROFEVENTS
```
The numbers should match: https://twiki.cern.ch/twiki/bin/view/CMS/DMS13TeVSynch

### Crab submit
```
cd NeroProducer/Nero/test
```
edit crabCfg.py to use _test/testNero.py_
crab submit

### Batch submit
* MINIAOD
* edit the ../test/testNero.py to be sure to use the right configuration (ChargedHiggs or MonoJet) 
```
cd NeroProducer/Nero/script
dir=TT_Tune4C_13TeV-pythia8-tauola;vers=v0.2; python sendOnBatch.py -i ../test/testNero.py -e /store/mc/Phys14DR/${dir} --put-in=/store/user/${USER}/Nero/${vers}/${dir} -d ${dir} -n 100 -q 8nh
```
* MINIAOD-SKIMMED
* edit the ../test/testNeroSkimmed.py to be sure to use the right configuration (ChargedHiggs or MonoJet) 
```
cd NeroProducer/Nero/script
vers=v0.2;dir=TTbar_HBWB_HToTauNu_M-90_13TeV_pythia6 ;python sendOnBatch.py -i ../test/testNeroSkimmed.py -e /store/user/amarini/v0.1/${dir} --put-in=/store/user/amarini/Nero/${vers}/${dir} -d ${dir} -n 10 -q 8nh
```

### Grid submit
* MINIAOD
* edit test/crabNero.py to match the datasets you want
```
python crabNero.py
```
* check the status with
```
python multicrab.py  -c status -w NeroSubmission
```
or 
```
crab status NeroSubmission/mydir
```

## Core
Core can be either compiled within CMSSW or standalone.
* Compile
```
cd NeroProducer/Core
make -j 16
```
* Use it, for example
```
root
gSystem->Load("bin/libBare.so");
TChain *f = new TChain("nero/events")
f->Add("~/eos/cms/store/user/amarini/Nero/v0.2/TTbar_HBWB_HToTauNu_M-90_13TeV_pythia6/NeroNtuples_*root")

BareJets *j = new BareJets()
j->setBranchAddresses(f)

BarePhotons *p = new BarePhotons()
p->setBranchAddresses(f)

f->GetEntry(0)

j->p4->GetEntries()
((TLorentzVector*)j->p4->At(0))->Pt()
```

## Skim

### CMSSW compile
```
cmsenv
cd NeroProducer/Skim
scram b -j 16
```

### Crab submit
```
cd NeroProducer/Nero/test
```
edit crabCfg.py to use _test/runSkim.py_
```
crab submit
```



