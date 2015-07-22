import ROOT
import os,sys
from subprocess import call, check_output
from optparse import OptionParser
import re


usage ='''prog [opts] args
		produces validation plots
	'''
parser= OptionParser(usage=usage)



version="v1.0"

disks={}
xsections={}
nevents={}
lumi=30 ## pb

book=['DY','WZ','ZZ','WW','WJets','TTJets']
datasets='SingleMuon'
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
	base='/store/user/amarini/Nero/v1.0/PromptReco-v1'
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
	print " -> for mc ", mc,
	sys.stdout.flush()
	t=ROOT.TChain("nero/all")	
	for f in ReadFromEos( disks[mc] ):
		t.Add(f)
	SW=0.
	for i in range(0,t.GetEntries() ):
		t.GetEntry(i)
		SW += t.weight
	nevents[mc]=SW
	print SW

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
	ltx.SetTextFont(42)
	ltx.SetTextSize(0.03)
	ltx.SetTextAlign(13)
	ltx.DrawLatex(.96,.96,"%.1fpb^{-1}(13TeV)"%Lumi)
	ltx.SetTextAlign(31)
	ltx.SetTextSize(0.05)
	ltx.DrawLatex(.17,.89,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}")

def DrawHistograms( dict ) :
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
	data= dict['data']
	data.SetMarkerStyle(20)
	data.SetMarkerColor(ROOT.kBlack)
	data.SetLineColor(ROOT.kBlack)

	data.Draw("AXIS")
	
	s.Draw("HIST SAME")
		
	data.Draw("PE SAME")	
	data.Draw("AXIS SAME")
	data.Draw("AXIS X+ Y+ SAME")


## Loop
llM={}
llPt={}
rho={}
llM["data"] =ROOT.TH1D("llM","llM;M_{ll};Events",150,0,300)
llPt["data"]=ROOT.TH1D("llPt","llPt",150,0,300)
rho["data"] =ROOT.TH1D("rho","rho",150,0,300)

print "-> compute observables"
for mc in book:
	print " * for mc", mc,
	sys.stdout.flush()
	t=ROOT.TChain("nero/events")	
	for f in ReadFromEos( disks[mc] ):
		t.Add(f)

	llM[mc] = llM["data"].Clone("%s_%s"%(llM["data"].GetName(),mc))
	llPt[mc] = llPt["data"].Clone("%s_%s"%(llPt["data"].GetName(),mc))
	rho[mc] = rho["data"].Clone("%s_%s"%(rho["data"].GetName(),mc))

	for i in range(0,t.GetEntries() ):
		t.GetEntry(i)
		if t.lepP4.GetEntries()<2 : continue ## 2leptons
		if t.lepP4[1].Pt() < 20 : continue ## pt 20
		if t.lepPdgId[0]* t.lepPdgId[1] != -13*13 : continue ## OS SF muon, leading two
		ll = t.lepP4[0] + t.lepP4[1]
		llM[mc].Fill( ll.M(), t.weight * xsections[mc] )
		llPt[mc].Fill( ll.Pt() )
		rho[mc].Fill( t.rho )
	print "DONE"

for data in datasets:
	print " * for data",
	t=ROOT.TChain("nero/events")
	for f in ReadFromEos( disks[mc] ):
	      t.Add(f)
	for i in range(0,t.GetEntries() ):
                if t.lepP4.GetEntries()<2 : continue ## 2leptons
                if t.lepP4[1].Pt() < 20 : continue ## pt 20
                if t.lepPdgId[0]* t.lepPdgId[1] != -13*13 : continue ## OS SF muon, leading two
                ll = t.lepP4[0] + t.lepP4[1]
                llM[mc].Fill( ll.M() )
                llPt[mc].Fill( ll.Pt() )
                rho[mc].Fill( t.rho )
	print "DONE"

print "-> Preparing canvas"
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

canvas=[]


c=PrepareCanvas("llM")
DrawHistograms(llM)
CMS()
canvas.append(c)

c=PrepareCanvas("llPt")
DrawHistograms(llPt)
CMS()
canvas.append(c)

c=PrepareCanvas("rho")
DrawHistograms(rho)
CMS()
canvas.append(c)

raw_input("Looks ok?")
for c in canvas:
	c.SaveAs("plot/" + c.GetName() + ".pdf")
	c.SaveAs("plot/" + c.GetName() + ".png")
