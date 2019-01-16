OUT="Pileup.txt"
#JSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco//Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
#JSON=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/Final/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt
JSON=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/ReReco/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON_v1.txt
PUJSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/PileUp/pileup_latest.txt"

#  KEY: TH1D     PU-pileup;1     pileup
#  KEY: TH1D     PU-pileupUp;1   pileup
#  KEY: TH1D     PU-pileupDown;1 pileup

echo "-> Doing nominal pileup with JSON=$JSON"
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  \
	--calcMode true \
	--minBiasXsec 69200 \
	--maxPileupBin 100 --numPileupBins 100  \
	--pileupHistName=PU-pileup \
	pileupNominal.root

echo "-> Doing nominal pileup Up"
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  \
	--calcMode true \
	--minBiasXsec 72660 \
	--maxPileupBin 100 --numPileupBins 100  \
	--pileupHistName=PU-pileupUp \
	pileupUp.root

echo "-> Doing nominal pileup Down"
pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  \
	--calcMode true \
	--minBiasXsec 65740 \
	--maxPileupBin 100 --numPileupBins 100  \
	--pileupHistName=PU-pileupDown \
	pileupDown.root
echo "-> Merging"
hadd pileup.root pileupNominal.root pileupUp.root pileupDown.root
echo "-> DONE"
