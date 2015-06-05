
import os,sys
from subprocess import call


#tnFileName = "/store/user/amarini/Nero/v0.2/TTbar_HBWB_HToTauNu_M-90_13TeV_pythia6/NeroNtuples_*root"
#ttFileName = "/store/user/amarini/Nero/v0.2/TT_Tune4C_13TeV-pythia8-tauola/NeroNtuples_*root"
#tzFileName = "/store/user/amarini/Nero/v0.2/TTZJets_Tune4C_13TeV-madgraph-tauola/NeroNtuples_*root"
#qcdFileName = "/store/user/amarini/Nero/v0.2/QCD_Pt-30to50_MuEnrichedPt5_Tune4C_13TeV_pythia8/NeroNtuples_*root"
#WFileName = "/store/user/amarini/Nero/v0.2/WToTauNu_M_200_Tune4C_tauola_13TeV_pythia8/NeroNtuples_*root"


from optparse import OptionParser
parser= OptionParser()
parser.add_option("-v","--var",dest="var",type="string",help='''variable to be plotted. \n
		Examples: nBjets : 'Sum$(jetBdiscr>0.8)'
			  nLeps  : 'Length$(lepIso)'
			  nTaus  : 'Sum$(tauId > 0.5 && tauIso <10)'
			  nJets50: 'Sum$(jetP4.Pt() > 50)'
		Default=%default''',default="Sum$(jetBdiscr>0.8)");
parser.add_option("-c","--cut",dest="cut",type="string",help="Cut string. Default=%default",default="");
parser.add_option("-o","--output",dest="output",type="string",help="Output File. Default=%default",default="output.root");
parser.add_option("-s","--signal",dest="sig",type="string",help='''Comma separated list of signals.\n
		Examples: A+B+C,D+E,F
		Default=%default''',default="/store/user/amarini/Nero/v0.2/TTbar_HBWB_HToTauNu_M-90_13TeV_pythia6/NeroNtuples_*root");
parser.add_option("-b","--background",dest="bkg",type="string",help="Comma separated list of backgrounds. Default=%default",
		default="/store/user/amarini/Nero/v0.2/TT_Tune4C_13TeV-pythia8-tauola/NeroNtuples_*root,/store/user/amarini/Nero/v0.2/TTZJets_Tune4C_13TeV-madgraph-tauola/NeroNtuples_*root,/store/user/amarini/Nero/v0.2/QCD_Pt-30to50_MuEnrichedPt5_Tune4C_13TeV_pythia8/NeroNtuples_*root,/store/user/amarini/Nero/v0.2/WToTauNu_M_200_Tune4C_tauola_13TeV_pythia8/NeroNtuples_*root");
parser.add_option("-n","--nbins",dest="nbins",type="int",help="nbins. Default=%default",default=10);
parser.add_option("","--xmin",dest="xmin",type="float",help="xMin. Default=%default",default=0.);
parser.add_option("","--xmax",dest="xmax",type="float",help="xMax. Default=%default",default=10);
parser.add_option("-w","--weight",dest="weight",action="store_true",help="Assume MC is weighted. Default=%default",default=False);

opts,args = parser.parse_args()

var = opts.var
outfile = opts.output

#SigFileName = [tnFileName]
#BkgFileName = [ttFileName,tzFileName,qcdFileName,WFileName]

SigFileName = []
BkgFileName = []
for s in opts.sig.split(','):
	SigFileName.append( s )
for b in opts.bkg.split(','):
	BkgFileName.append( b )

### ROOT ###
sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()

## mount eos
cmd = "mountpoint ~/eos || /afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select -b fuse mount ~/eos"
call(cmd,shell=True)


def Load(name):
	''' Load a file name and return a TChain with the relative info'''
	chain = r.TChain("nero/events")
	all = r.TChain("nero/all") 

	for n in name.split("+"):
		if n.startswith("/store"):
			n = "~/eos/cms/" + n

		print "Adding ",n,"to the chain"
		chain.Add(n)
		all.Add(n)

	print "File",chain.GetFileNumber(),"file",chain.GetFile()
	h=chain.GetFile().Get("nero/xSec")
	if opts.weight:
		sum=r.TH1F("sum","sum of weights",1,0,2)
		all.Draw("1>>sum","mcWeight")
		nEntries = sum.GetBinContent(1)
	else:
		nEntries= all.GetEntries()
	xSec= h.GetBinContent(1)/h.GetBinContent(2) 
	
	return (chain,nEntries,xSec) 
	

if opts.weight:
	opts.cut = "mcWeight*(%s)"%opts.cut

# Load Chains
sigChains = []
sigAll = []
sigXsec = []
for name in SigFileName:
	(chain,nEntries,xSec) = Load(name)
	
	sigChains.append( chain )
	sigAll.append(  nEntries ) 
	sigXsec.append ( xSec )

# Load Chains
bkgChains = []
bkgAll = []
bkgXsec = []
for name in BkgFileName:
	(chain,nEntries,xSec) = Load(name)
	
	bkgChains.append( chain )
	bkgAll.append(  nEntries ) 
	bkgXsec.append ( xSec )

### PLOT ###

out = r.TFile(outfile,"RECREATE")
obj = []
# plot the n. of bjets
for idx,chain in enumerate(sigChains):
	h = r.TH1F("sig_%d"%idx,"sig_%d"%idx,opts.nbins,opts.xmin,opts.xmax)
	chain.Draw(var+">>sig_%d"%idx,opts.cut)	
	h.Scale( sigXsec[idx]/sigAll[idx])
	obj.append(h)

for idx,chain in enumerate(bkgChains):
	h = r.TH1F("bkg_%d"%idx,"bkg_%d"%idx,opts.nbins,opts.xmin,opts.xmax)
	chain.Draw(var+">>bkg_%d"%idx,opts.cut)	
	h.Scale( bkgXsec[idx]/bkgAll[idx])
	obj.append(h)

out.cd()
for o in obj:
	o.Write()

mySig = r.TNamed("info_sig",opts.sig)
myBkg = r.TNamed("info_bkg",opts.bkg)
myVar = r.TNamed("info_var",opts.var)
myCut = r.TNamed("info_cut",opts.cut)

myVar.Write()
myCut.Write()
mySig.Write()
myBkg.Write()

out.Close()
### UNMOUNT ###
cmd = "mountpoint ~/eos && /afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select -b fuse umount ~/eos"
call(cmd,shell=True)
