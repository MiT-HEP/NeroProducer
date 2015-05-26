# NeroProducer
NtuplE fRamewOrk
## Nero
The Nero directory contains the ntuple producer

### CMSSW compile
```
cmsenv
cd NeroProducer/Nero
source script/setup.sh
scram b -j 16
cd NeroProducer/Skim
scram b -j 16
```

### Crab submit
```
cd NeroProducer/Nero/test
```
edit crabCfg.py to use _test/testNero.py_
crab submit

### Batch submit
* MINIAOD
```
cd NeroProducer/Nero/script
dir=TT_Tune4C_13TeV-pythia8-tauola;vers=v0.2; python sendOnBatch.py -i ../test/testNero.py -e /store/mc/Phys14DR/${dir} --put-in=/store/user/${USER}/Nero/${vers}/${dir} -d ${dir} -n 100 -q 8nh
```
* MINIAOD-SKIMMED
```
cd NeroProducer/Nero/script
vers=v0.2;dir=TTbar_HBWB_HToTauNu_M-90_13TeV_pythia6 ;python sendOnBatch.py -i ../test/testNeroSkimmed.py -e /store/user/amarini/v0.1/${dir} --put-in=/store/user/amarini/Nero/${vers}/${dir} -d ${dir} -n 10 -q 8nh
```

### Bare Compilation
* Compile
```
cd NeroProducer/Nero
make -j 16
```
* Use it
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
