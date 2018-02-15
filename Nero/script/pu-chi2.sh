OUT="Pileup.txt"
#JSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/ReReco//Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt"
JSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/Final/Cert_294927-306462_13TeV_PromptReco_Collisions17_JSON.txt"
#PUJSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt"
PUJSON="/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions17/13TeV/PileUp/pileup_latest.txt"

#  KEY: TH1D     PU-pileup;1     pileup
#  KEY: TH1D     PU-pileupUp;1   pileup
#  KEY: TH1D     PU-pileupDown;1 pileup

mkdir pileup
cd pileup

for pu in 78000 77000 76000 75000 74000 73000 72000 71000 70800 70600 70400 70200 70000 69800 69600 69400 69200 69000 68000 67000 66000 65000 64000 63000
do
	echo "-> Doing nominal pileup with JSON=$JSON"
	pileupCalc.py -i $JSON --inputLumiJSON $PUJSON  \
		--calcMode true \
		--minBiasXsec ${pu} \
		--maxPileupBin 100 --numPileupBins 100  \
		NewPileupHistogram_${pu}nb.root
done
echo "-> DONE"
#--pileupHistName=PU-pileup \
