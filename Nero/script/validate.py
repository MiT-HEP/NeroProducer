import ROOT
import os,sys
from subprocess import call, check_output
from optparse import OptionParser
import re


usage ='''prog [opts] args
		produces validation plots
	'''
parser= OptionParser(usage=usage)
parser.add_option("-b","--batch",dest="batch",action="store_true",help="Run in Batch mode",default=False);
parser.add_option("-l","--limit",dest="limit",type='int',help="max number of entries for mc.",default=-1); 

opts, args = parser.parse_args()

if opts.batch:
	ROOT.gROOT.SetBatch()


version="v1.0"

disks={}
xsections={}
nevents={}
lumi=30 ## pb

fileLimit=-1

#book=['DY','WZ','ZZ','WW','WJets','TTJets'] ## WJets is very big with low eff for double muon
book=['DY','WZ','ZZ','WW','TTJets']
datasets=['SingleMuon','SingleElectron']
##configure
if True:
	#### Asympt 50 ns v2
	base='/store/user/amarini/Nero/v1.0/RunIISpring15DR74/Asympt50ns_MCRUN2_74_V9A-v2/'
	disks['DY']=base+'DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8'
	disks['WZ']=base+'WZ_TuneCUETP8M1_13TeV-pythia8'
	disks['ZZ']=base+'ZZ_TuneCUETP8M1_13TeV-pythia8'
	### 50ns v1
	base='/store/user/amarini/Nero/v1.0/RunIISpring15DR74/Asympt50ns_MCRUN2_74_V9A-v1/'
	disks['WW']=base+'WW_TuneCUETP8M1_13TeV-pythia8'
	disks['WJets']=base+'WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8'
	disks['TTJets']=base+'TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8'
	### DATA
	base='/store/user/amarini/Nero/v1.0/PromptReco-v1/'
	disks['SingleMuon']=base+'SingleMuon'
	disks['SingleElectron']=base+'SingleElectron'
	disks['SinglePhoton']=base+'SinglePhoton'
	disks['MET']=base+'MET'
## xsections
if True:
	#https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat13TeV
	xsections['DY']=2008.4 #NNLO
	xsections['TTJets']=831.76 ## mtop=172.5; xsec=815.96 mtop=173.2 NNLO
	#LO
	xsections['WW']=64.5856059293
	xsections['WZ']=23.1768776388
	xsections['ZZ']=10.2357724104
	xsections['WJets']=42048.4493573 ## internal NLO; for NNLO we need to use the leptonic samples

def ReadFromEos(eos):
	EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
	cmd = EOS+ " find -f " + eos
	outputList = check_output(cmd,shell=True)
	fileList0 = outputList.split()
	fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 ]
	return fileList

## compute nevents
print "-> compute nevents"
for mc in book:
	stdout = " * for mc " +  mc
	print stdout,
	sys.stdout.flush()
	t=ROOT.TChain("nero/all")	
	n=0
	ntries = 0 
	for f in ReadFromEos( disks[mc] ):
		n += 1 
		if fileLimit>=0 and n > fileLimit: 
			print "File Limit reached for mc", mc, "at",n,"files"
			break
		t.Add(f)
		if opts.limit>=0:
			ntries = t.GetEntries()
			if ntries > opts.limit: fileLimit = n
	stdout += " "+str(n) +" files added"
	print "\r"+stdout,
	sys.stdout.flush()
	SW=0.
	for i in range(0,t.GetEntries()):
		if i&1023 == 1:
			print "\r"+ stdout ,i,"/",t.GetEntriesFast(),
			sys.stdout.flush()
		t.GetEntry(i)
		SW += t.mcWeight
	nevents[mc]=SW
	print "\r" + stdout,"SumW=",SW

## plot stuff
def PrepareCanvas(name="c"):
	c=ROOT.TCanvas(name,name,800,800)
	c.SetTopMargin(0.05)
	c.SetRightMargin(0.05)
	c.SetLeftMargin(0.15)
	c.SetBottomMargin(0.15)
	return c

def CMS():
	ltx=ROOT.TLatex()
	ltx.SetNDC()
	ltx.SetTextFont(42)
	ltx.SetTextSize(0.03)
	ltx.SetTextAlign(31)
	ltx.DrawLatex(.96,.96,"%.1fpb^{-1}(13TeV)"%lumi)
	ltx.SetTextAlign(13)
	ltx.SetTextSize(0.05)
	ltx.DrawLatex(.17,.93,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}")

def DrawHistograms( dict, target="data" ) :
	s=ROOT.THStack()
	s.SetName( dict["data"].GetName() + "_mcstack")
	for mc in book:
		dict[mc].SetLineColor(ROOT.kBlack)
		dict[mc].SetLineWidth(2)
		if   mc == 'DY' : dict[mc].SetFillColor( ROOT.kCyan )
		elif mc == 'TTJets' : dict[mc].SetFillColor( ROOT.kMagenta+2 )
		elif mc == 'WJets' : dict[mc].SetFillColor( ROOT.kGreen+2 )
		elif mc == 'WW' : dict[mc].SetFillColor( ROOT.kGreen )
		elif mc == 'WZ' : dict[mc].SetFillColor( ROOT.kGreen )
		elif mc == 'ZZ' : dict[mc].SetFillColor( ROOT.kGreen )
		else            : dict[mc].SetFillColor( ROOT.kGray )
		s.Add(dict[mc])
	data= dict[target]
	data.SetMarkerStyle(20)
	data.SetMarkerColor(ROOT.kBlack)
	data.SetLineColor(ROOT.kBlack)

	data.Draw("AXIS")
	
	s.Draw("HIST SAME")
	data.Draw("PE SAME")	

	data.Draw("AXIS SAME")
	data.Draw("AXIS X+ Y+ SAME")


## Loop
mmM={}
mmPt={}
mmMet={}
mmRho={}
eeM={}
eePt={}
eeMet={}
eeRho={}

mmM["data"] =ROOT.TH1D("mmM","mmM;M_{ll};Events",150,0,300)
mmPt["data"]=ROOT.TH1D("mmPt","mmPt",150,0,300)
mmMet["data"]=ROOT.TH1D("mmMet","mmMet",150,0,300)
mmRho["data"] =ROOT.TH1D("mmRho","mmRho",150,0,300)

eeM["data"] =ROOT.TH1D("eeM","eeM;M_{ll};Events",150,0,300)
eePt["data"]=ROOT.TH1D("eePt","eePt",150,0,300)
eeMet["data"]=ROOT.TH1D("eeMet","eeMet",150,0,300)
eeRho["data"] =ROOT.TH1D("eeRho","eeRho",150,0,300)

print "-> compute observables"
for mc in book:
	stdout=" * for mc "+mc
	print stdout,
	sys.stdout.flush()
	t=ROOT.TChain("nero/events")	
	n=0
	for f in ReadFromEos( disks[mc] ):
		n += 1
		if fileLimit>=0 and n > fileLimit:
			break
			print "File Limit reached for mc", mc, "at",n,"files"
		t.Add(f)

	mmM[mc] = mmM["data"].Clone("%s_%s"%(mmM["data"].GetName(),mc))
	mmPt[mc] = mmPt["data"].Clone("%s_%s"%(mmPt["data"].GetName(),mc))
	mmMet[mc] = mmMet["data"].Clone("%s_%s"%(mmMet["data"].GetName(),mc))
	mmRho[mc] = mmRho["data"].Clone("%s_%s"%(mmRho["data"].GetName(),mc))

	eeM[mc] = eeM["data"].Clone("%s_%s"%(eeM["data"].GetName(),mc))
	eePt[mc] = eePt["data"].Clone("%s_%s"%(eePt["data"].GetName(),mc))
	eeMet[mc] = eeMet["data"].Clone("%s_%s"%(eeMet["data"].GetName(),mc))
	eeRho[mc] = eeRho["data"].Clone("%s_%s"%(eeRho["data"].GetName(),mc))

	for i in range(0, t.GetEntries() ):
		if i&1023 == 1:
			print "\r"+stdout ,i,"/",t.GetEntriesFast(),
			sys.stdout.flush()
		t.GetEntry(i)
		if t.lepP4.GetEntries()<2 : continue ## 2leptons
		if t.lepP4[1].Pt() < 20 : continue ## pt 20

		w = t.mcWeight * xsections[mc] * lumi/ nevents[mc]
		if t.lepPdgId[0]* t.lepPdgId[1] == -13*13 :  ## OS SF muon, leading two
			ll = t.lepP4[0] + t.lepP4[1]
			mmRho[mc].Fill( t.rho ,w)
			mmM[mc].Fill( ll.M(), w)
			#print "mc pass", ll.M(), ll.Pt() ## DEBUG
			if ll.M() <60 or ll.M() >120 : continue
			mmPt[mc].Fill( ll.Pt() ,w)
			mmMet[mc].Fill( t.metP4[0].Pt(),w )
		if t.lepPdgId[0]* t.lepPdgId[1] == -11*11 :  ## OS SF muon, leading two
			ll = t.lepP4[0] + t.lepP4[1]
			eeRho[mc].Fill( t.rho,w )
			eeM[mc].Fill( ll.M(), w)
			if ll.M() <60 or ll.M() >120 : continue
			eePt[mc].Fill( ll.Pt() ,w)
			eeMet[mc].Fill( t.metP4[0].Pt() ,w)
	print '\r'+stdout+" DONE                            "

for data in datasets:

	stdout=" * for data",data
	print stdout,
	t=ROOT.TChain("nero/events")
	n=0
	for f in ReadFromEos( disks[data] ):
	      t.Add(f)
	      n +=1
	stdout += " "+ str(n) + " files"
	print '\r' + stdout,

	for i in range(0,t.GetEntries() ):
		if i&1023 == 1:
			print "\r"+stdout ,i,"/",t.GetEntriesFast(),
			sys.stdout.flush()
		t.GetEntry(i)
                if t.lepP4.GetEntries()<2 : continue ## 2leptons
                if t.lepP4[1].Pt() < 20 : continue ## pt 20
                if t.lepPdgId[0]* t.lepPdgId[1] == -13*13 and data=='SingleMuon' : ## OS SF muon, leading two
                	ll = t.lepP4[0] + t.lepP4[1]
			#print "data pass", ll.M(), ll.Pt() ## DEBUG
                	mmM["data"].Fill( ll.M() )
                	mmRho["data"].Fill( t.rho )
			if ll.M() <60 or ll.M() >120 : continue
                	mmPt["data"].Fill( ll.Pt() )
			mmMet["data"].Fill( t.metP4[0].Pt() )
                if t.lepPdgId[0]* t.lepPdgId[1] == -11*11 and data=='SingleElectron': # OS SF muon, leading two
                	ll = t.lepP4[0] + t.lepP4[1]
                	eeM["data"].Fill( ll.M() )
                	eeRho["data"].Fill( t.rho )
			if ll.M() <60 or ll.M() >120 : continue
                	eePt["data"].Fill( ll.Pt() )
			eeMet["data"].Fill( t.metP4[0].Pt() )
	print "\r"+stdout+" DONE                               "

print "-> Preparing canvas"
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

canvas=[]

for name in ['mmM','mmPt','mmRho','mmMet',
		'eeM','eePt','eeRho','eeMet']:

	c=PrepareCanvas(name)
	exec('DrawHistograms('+name+')')
	CMS()
	canvas.append(c)

if not opts.batch:
	raw_input("Looks ok?")

for c in canvas:
	c.SaveAs("plot/" + c.GetName() + ".pdf")
	c.SaveAs("plot/" + c.GetName() + ".png")
	c.SaveAs("plot/" + c.GetName() + ".root")
