import ROOT
import os,sys
from subprocess import call, check_output
from optparse import OptionParser
import re
import json


usage ='''prog [opts] args
		produces validation plots
	'''
parser= OptionParser(usage=usage)
parser.add_option("-b","--batch",dest="batch",action="store_true",help="Run in Batch mode",default=False);
parser.add_option("-l","--limit",dest="limit",type='int',help="max number of entries for mc.",default=-1); 
parser.add_option("-L","--lumi",dest="lumi",type='float',help="Luminosity.",default=30); 
parser.add_option("","--plotdir",dest="plotdir",type='string',help="plot directory [Default=%default].",default="plot"); 

parser.add_option("-j","--json",dest="json",type='string',help="json file",default=""); 

opts, args = parser.parse_args()

if opts.batch:
	ROOT.gROOT.SetBatch()


version="v1.1.1"

disks={}
xsections={}
nevents={}
pu={}
lumi=opts.lumi ## pb

fileLimit=-1

book=['DY','TTJets'] ## WJets is very big with low eff for double muon
datasets=['SingleMuon']
##configure
if True:
	#### Asympt 50 ns v2
	base='/store/user/amarini/Nero/' + version + "/"
	disks['DY']=base+'DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/'
	disks['WZ']=base+'WZ_TuneCUETP8M1_13TeV-pythia8/'
	disks['ZZ']=base+'ZZ_TuneCUETP8M1_13TeV-pythia8/'
	### 50ns v1
	disks['WW']=base+'WW_TuneCUETP8M1_13TeV-pythia8/'
	disks['WJets']=base+'WJetsToLNu_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/'
	disks['TTJets']=base+'TTJets_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/'
	### DATA -- promptReco
	base='/store/user/amarini/Nero/' + version + "/"
	disks['SingleMuon']=base+'SingleMuon'
	disks['SingleElectron']=base+'SingleElectron'
	disks['SinglePhoton']=base+'SinglePhoton'
	disks['Tau'] = base+'Tau'
	disks['MET']=base+'MET'

## xsections
if True:
	#https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat13TeV
	xsections['DY']=2008.4*3 #NNLO, Z->mm is 2008.4,  we have Z->ll
	xsections['TTJets']=831.76 ## mtop=172.5; xsec=815.96 mtop=173.2 NNLO
	#LO
	xsections['WW']=64.5856059293
	xsections['WZ']=23.1768776388
	xsections['ZZ']=10.2357724104
	xsections['WJets']=42048.4493573 ## internal NLO; for NNLO we need to use the leptonic samples

def ReadFromEos(eos):
	if not eos.startswith('/store') : return [eos]
	EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
	cmd = EOS+ " find -f " + eos
	outputList = check_output(cmd,shell=True)
	fileList0 = outputList.split()
	fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]
	return fileList

def DeactivateBranches(t):
	''' deactivate used branches and speed up '''
	t.SetBranchStatus("*",0)
	
	t.SetBranchStatus("lepP4",1)
	t.SetBranchStatus("lepPdgId",1)
	t.SetBranchStatus("puTrueInt",1)
	t.SetBranchStatus("mcWeight",1)
	t.SetBranchStatus("npv",1)
	t.SetBranchStatus("rho",1)

### if a json is given parse it
if opts.json != "":
	jstring = open(opts.json).read() 
	goodLumis = json.loads( jstring )


# get PU Target
puFiles={}
puFiles[80] = ROOT.TFile.Open("script/MyDataPileupHistogram.root")
#puFiles[65] = ROOT.TFile.Open("script/MyDataPileupHistogram_65mb.root")
#puFiles[70] = ROOT.TFile.Open("script/MyDataPileupHistogram_70mb.root")
#puFiles[71] = ROOT.TFile.Open("script/MyDataPileupHistogram_71mb.root")
#puFiles[72] = ROOT.TFile.Open("script/MyDataPileupHistogram_72mb.root")
#puFiles[73] = ROOT.TFile.Open("script/MyDataPileupHistogram_73mb.root")
#puFiles[74] = ROOT.TFile.Open("script/MyDataPileupHistogram_74mb.root")
#puFiles[75] = ROOT.TFile.Open("script/MyDataPileupHistogram_75mb.root")
#puFiles[76] = ROOT.TFile.Open("script/MyDataPileupHistogram_76mb.root")
#puFiles[84] = ROOT.TFile.Open("script/MyDataPileupHistogram_84mb.root")

for num in [69,68,67,66,64,65,70,71,72,73,74,75,76,84]:
	puFiles[num] = ROOT.TFile.Open("script/MyDataPileupHistogram_%dmb.root"%num)

for num in puFiles :
	print "getting pileup,",num
	pu["data_%d"%num] = puFiles[num].Get("pileup").Clone("pileup_%d"%num)

print "->Getting pu for data" ## TODO, for puTrueInt, this is meaningless
puTarget= pu["data_80"].Clone("puTarget")

puTarget.Reset("ACE")

## compute nevents
print "-> compute nevents and pileup histo for mc"

for mc in book:
	stdout = " * for mc " +  mc
	print stdout,
	sys.stdout.flush()
	t=ROOT.TChain("nero/all")	
	#te=ROOT.TChain("nero/events")	
	n=0
	ntries = 0 
	for f in ReadFromEos( disks[mc] ):
		n += 1 
		if fileLimit>=0 and n > fileLimit: 
			print "File Limit reached for mc", mc, "at",n,"files"
			break
		t.Add(f)
		#te.Add(f)
		if opts.limit>=0:
			ntries = t.GetEntries()
			if ntries > opts.limit: fileLimit = n
	stdout += " "+str(n) +" files added"
	print "\r"+stdout,
	sys.stdout.flush()
	t.SetBranchStatus("*",0)
	t.SetBranchStatus("mcWeight",1)
	t.SetBranchStatus("puTrue*",1)
	sumW_histo = ROOT.TH1D("sumWH","histo",1,0,2)
	t.Draw("1>>sumWH","mcWeight" )
	nevents[mc]= sumW_histo.GetBinContent(1)
	print "\r" + stdout, " pu ",
	pu[mc] = puTarget.Clone("pu_"+mc)
	pu[mc + '_reweight'] = puTarget.Clone("pu_"+mc + "_reweight") ## try to get the true reweight distribution
	#te.Draw("rho>>pu_"+mc,"lepP4[1].Pt()>20","goff")
	t.Draw("puTrueInt>>pu_"+mc,"","goff")
	print "DONE"



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
	ltx.DrawLatex(.96,.96,"%.1f pb^{-1} (13TeV)"%lumi)  ## ONLY TCANVAS
	#ltx.DrawLatex(.96,.98,"%.1f pb^{-1} (13TeV)"%lumi)
	ltx.SetTextAlign(13)
	ltx.SetTextSize(0.05)
	ltx.DrawLatex(.17,.93,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") ##ONLY TCANVAS
	#ltx.DrawLatex(.17,.95,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") 

garbage=[]
def DrawHistograms( dict, num=80 ) :
	target='data'
	pup=ROOT.TPad(dict[target].GetName()+ "pileup%d"%num + "_pad1" ,"pad1", 0,0.25, 1 ,1)
	pdn=ROOT.TPad(dict[target].GetName()+ "pileup%d"%num + "_pad2" ,"pad2", 0,0, 1 ,.25)
	garbage.append(pup)
	garbage.append(pdn)

	pup.SetTopMargin(0.05)
	pup.SetBottomMargin(0.15)
	pup.SetLeftMargin(0.15)
	pup.SetRightMargin(0.05)
	pdn.SetTopMargin(0.05)
	pdn.SetBottomMargin(0.40)
	pdn.SetLeftMargin(0.15)
	pdn.SetRightMargin(0.05)
	
	pup.Draw()
	pdn.Draw()
	pup.cd()

	data= dict[target]
	data.SetMarkerStyle(20)
	data.SetMarkerColor(ROOT.kBlack)
	data.SetLineColor(ROOT.kBlack)

	s=ROOT.THStack(data.GetName() + "_mcstack",data.GetTitle())
	#data.SetName( data.GetName() + "_data")

	all=data.GetName() + "_allmc"
	dict[ all ] = None
	mybook = [ mc + "_pileup%d"%num for mc in book ]
	for mc in mybook:
		if dict[all] == None : dict[all] = dict[mc].Clone(all)
		else: dict[all].Add(dict[mc])

		dict[mc].SetLineColor(ROOT.kBlack)
		dict[mc].SetLineWidth(2)
		if  'DY' in mc: dict[mc].SetFillColor( ROOT.kCyan )
		elif 'TTJets' in mc : dict[mc].SetFillColor( ROOT.kMagenta+2 )
		elif 'WJets' in mc: dict[mc].SetFillColor( ROOT.kGreen+2 )
		elif 'WW' in mc : dict[mc].SetFillColor( ROOT.kGreen )
		elif 'WZ' in mc : dict[mc].SetFillColor( ROOT.kGreen )
		elif 'ZZ' in mc : dict[mc].SetFillColor( ROOT.kGreen )
		else            : dict[mc].SetFillColor( ROOT.kGray )

		s.Add(dict[mc])

	garbage.append(s)
	
	s.Draw("HIST")
	s.GetXaxis().SetTitle( data.GetXaxis().GetTitle() )
	s.GetYaxis().SetTitle( data.GetYaxis().GetTitle() )
	s.GetYaxis().SetTitleOffset(1.2)
	#data.Draw("AXIS")
	#data.GetYaxis().SetTitleOffset(1.2)
	data.Draw("PE SAME")	

	s.GetXaxis().SetLabelFont(43)
	s.GetXaxis().SetTitleFont(43)
	s.GetYaxis().SetLabelFont(43)
	s.GetYaxis().SetTitleFont(43)
	s.GetXaxis().SetLabelSize(26)
	s.GetYaxis().SetLabelSize(26)
	s.GetXaxis().SetTitleSize(28)
	s.GetYaxis().SetTitleSize(28)
	s.GetXaxis().SetTitleOffset(1.2)
	s.GetYaxis().SetTitleOffset(1.2)

	data.GetXaxis().SetLabelFont(43) ## for the ratio
	data.GetXaxis().SetTitleFont(43)
	data.GetYaxis().SetLabelFont(43)
	data.GetYaxis().SetTitleFont(43)
	data.GetXaxis().SetLabelSize(26)
	data.GetYaxis().SetLabelSize(26)
	data.GetXaxis().SetTitleSize(28)
	data.GetYaxis().SetTitleSize(28)

	s.Draw("AXIS SAME")
	s.Draw("AXIS X+ Y+ SAME")

	pdn.cd()
	pdn.SetGridy()
	for i in range(0, dict[all].GetNbinsX() ) : dict[all].SetBinError(i+1,0)
	dict[ target + "_ratio"+"_pileup%d"%num ] = dict[target] . Clone( target + "pileup_%d"%num +  "_ratio")
	dict[ target + "_ratio"+"_pileup%d"%num ] . Divide( dict [ all ] ) 
	dict[ target + "_ratio"+"_pileup%d"%num ] . Draw(" PE ") 
	dict[ target + "_ratio"+"_pileup%d"%num ] . GetYaxis() . SetRangeUser(0,2)
	dict[ target + "_ratio"+"_pileup%d"%num ] . GetYaxis() . SetNdivisions(202)
	dict[ target + "_ratio"+"_pileup%d"%num ] . GetYaxis() . SetTitle("data/MC")
	dict[ target + "_ratio"+"_pileup%d"%num ] . GetYaxis() . SetTitleOffset(1.2)
	dict[ target + "_ratio"+"_pileup%d"%num ] . GetXaxis() . SetTitleOffset(4)

	chiSquare=0.
	for i in range(0,dict["data"].GetNbinsX()):
		iBin=i+1
		den = dict["data"].GetBinContent(iBin)
		if den<=0 : den= 1
		chiSquare +=  (dict[all].GetBinContent(iBin) - dict["data"].GetBinContent(iBin) )**2 / den

	print "chiSquare for num=",num," is ", chiSquare
	pup.cd()
	return chiSquare


## Loop
mmNpv={}
mmNpv["data"] =ROOT.TH1D("mmNpv","mmNpv;#npv; Events",50,0,50)


## normalize pu
print "-> normalizing pu: "
for what in book :
	print what,
	if pu[what].Integral():
		pu[what].Scale( 1./ pu[what].Integral() ) 
for num in puFiles :
	what = "data_%d"%num
	print what,
	if pu[what].Integral():
		pu[what].Scale( 1./ pu[what].Integral() ) 
print "DONE"

print "-> compute observables"
for mc in book:
	stdout=" * for mc "+mc
	print stdout,
	sys.stdout.flush()
	t=ROOT.TChain("nero/events")	
	DeactivateBranches(t)
	n=0
	for f in ReadFromEos( disks[mc] ):
		n += 1
		if fileLimit>=0 and n > fileLimit:
			break
			print "File Limit reached for mc", mc,"at",n,"files"
		t.Add(f)

	for num in puFiles:
		mmNpv[mc + "_pileup%d"%num ] = mmNpv["data"].Clone("%s_%s_pileup%d"%(mmNpv["data"].GetName(),mc,num))

	for i in range(0, t.GetEntries() ):
		if i&1023 == 1:
			print "\r"+stdout ,i,"/",t.GetEntriesFast(),
			sys.stdout.flush()
		t.GetEntry(i)
		puTrueInt = t.puTrueInt
		w = t.mcWeight * xsections[mc] * lumi / nevents[mc]
		if w == 0 : 
			print "MC weight for mc",mc,"is 0"
			print "\t * mcWeight",t.mcWeight
			print "\t * xsec",xsections[mc]
			print "\t * lumi",lumi
			print "\t * sumW",nevents[mc]
		

		## counts jets
		nJets=0
		leadJetPt=0
		leadJetIdx=-1
	
		puReweight={}
		for puNum in puFiles:
			den= pu[mc].GetBinContent(pu[mc].FindBin(puTrueInt)) 
			num= pu[ 'data_%d'%puNum ].GetBinContent(pu['data_%d'%puNum].FindBin(puTrueInt))
			if den==0: 
				print "\n EVENT WITH NULL PU REWEIGHT", t.eventNum
				num=0
				den=1
			puReweight[puNum] = float(num)/float(den )

		if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 20 and t.lepPdgId[0]* t.lepPdgId[1] == -13*13 :  ## OS SF muon, leading two
			#print '* DEBUG MM Event', t.lepP4[1].Pt(), t.lepPdgId[0]
			ll = t.lepP4[0] + t.lepP4[1]
			#print "mc pass", ll.M(), ll.Pt() ## DEBUG
			if ll.M() <60 or ll.M() >120 : continue
			for num in puFiles:
				mmNpv[mc+"_pileup%d"%num].Fill( t.npv ,w*puReweight[num])

	print '\r'+stdout+" DONE                            "

for data in datasets:
	stdout=" * for data "+ data
	print stdout,
	sys.stdout.flush()
	t=ROOT.TChain("nero/events")
	DeactivateBranches(t)
	n=0
	for f in ReadFromEos( disks[data] ):
	      t.Add(f)
	      n += 1
	stdout += " "+ str(n) + " files"
	print '\r' + stdout,
	sys.stdout.flush()

	for i in range(0,t.GetEntries() ):

		if i&1023 == 1:
			print "\r"+stdout ,i,"/",t.GetEntriesFast(), "%.2f%%"%(float(i)/t.GetEntriesFast())
			sys.stdout.flush()
		t.GetEntry(i)
		## JSON
		if opts.json != "":
			# filter accordingly to a json file
			run= str(t.runNum)
			if run not in goodLumis.keys(): continue
			isGood=False
			for lumis in goodLumis[run] :
				if t.lumiNum >= lumis[0] and t.lumiNum <= lumis[1] : isGood = True
			if not isGood: continue

                if t.lepP4.GetEntries()<2 : continue ## 2leptons
                if t.lepP4[1].Pt() < 20 : continue ## pt 20

                if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 20 and t.lepPdgId[0]* t.lepPdgId[1] == -13*13 and data=='SingleMuon' : ## OS SF muon, leading two
                	ll = t.lepP4[0] + t.lepP4[1]
			#print "data pass", ll.M(), ll.Pt() ## DEBUG
			if ll.M() <60 or ll.M() >120 : continue
                	mmNpv["data"].Fill( t.npv )

	print "\r"+stdout+" DONE                               "

print "-> Preparing canvas"
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

canvas=[]

th1d_chi2=ROOT.TH1D("chi2","chi2",100,0-.5,100-.5)
for num in puFiles:
	c=PrepareCanvas("mmNpv_pileup%d"%num)
	chi2=DrawHistograms(mmNpv,num)
	CMS()
	canvas.append(c)
	bin=th1d_chi2.FindBin(num)
	th1d_chi2.SetBinContent(bin,chi2)


if not opts.batch:
	raw_input("Looks ok?")

for c in canvas:
	c.SaveAs(opts.plotdir + "/" + c.GetName() + ".pdf")
	c.SaveAs(opts.plotdir + "/" + c.GetName() + ".png")
	c.SaveAs(opts.plotdir + "/" + c.GetName() + ".root")


f= ROOT.TFile.Open(opts.plotdir + "/pu.root", "RECREATE" )
f.cd()
for h in pu:
	pu[h].Write()
th1d_chi2.Write()
f.Close()

############ SUMMARY OF ALL THE PLOTS ###################
f= ROOT.TFile.Open(opts.plotdir + "/mmNpv.root", "RECREATE" )
f.cd()
for h in mmNpv:
	mmNpv[h].Write()
f.Close()
#########################################################
