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
parser.add_option("-p","--pileup",dest="pileup",type='string',help="Pileup File.",default="script/MyDataPileupHistogram.root"); 
parser.add_option("","--plotdir",dest="plotdir",type='string',help="plot directory [Default=%default].",default="plot"); 

parser.add_option("-j","--json",dest="json",type='string',help="json file",default=""); 
#parser.add_option("","--jec",dest="jec",type='string',help="apply jet energy corrections. eg 'jec/Summer15_50nsV2_MC'  [CMSSW]",default=""); 

opts, args = parser.parse_args()

if opts.batch:
	ROOT.gROOT.SetBatch()


#version="v1.1.2"
version="v1.3.1"

disks={}
xsections={}
nevents={}
pu={}
lumi=opts.lumi ## pb

fileLimit=-1

#book=['DY','WZ','ZZ','WW','WJets','TTJets'] ## WJets is very big with low eff for double muon
#book=['DY','TTJets','WJets','WW','WZ','ZZ'] ## WJets is very big with low eff for double muon
book=['DY','WZ','ZZ','WW','TTJets']
#datasets=['SingleMuon','SingleElectron','Tau']
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
	base='/store/user/amarini/Nero/' + version +"/" #+ "/data/"
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
	EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"
	cmd = EOS+ " find -f " + eos
	outputList = check_output(cmd,shell=True)
	fileList0 = outputList.split()
	fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]
	return fileList

def DeactivateBranches(t):
	''' deactivate used branches and speed up '''

	t.SetBranchStatus("*",0)
	
	t.SetBranchStatus("jetP4",1)
	t.SetBranchStatus("tauP4",1)
	t.SetBranchStatus("tauIso",1)
	t.SetBranchStatus("metPuppi",1)
	t.SetBranchStatus("metP4",1)
	t.SetBranchStatus("metNoHF",1)
	t.SetBranchStatus("lep*",1)
	t.SetBranchStatus("puTrueInt",1)
	t.SetBranchStatus("mcWeight",1)
	t.SetBranchStatus("runNum",1)
	t.SetBranchStatus("lumiNum",1)
	t.SetBranchStatus("eventNum",1)
	t.SetBranchStatus("npv",1)
	t.SetBranchStatus("rho",1)
	t.SetBranchStatus("triggerFired",1)

### if a json is given parse it
if opts.json != "":
	jstring = open(opts.json).read() 
	goodLumis = json.loads( jstring )


# get PU Target
puFile = ROOT.TFile.Open(opts.pileup)
doPileup=True
if puFile == None:
	print "-> ERROR: Pileup File does not exists. Not Reweighting"
	doPileup=False
#puTarget=ROOT.TH1D("pu","pu",100,0,50) # rho
if doPileup:
	puTarget=puFile.Get("pileup")
	if puTarget == None:
		print "-> ERROR: pileup file does not contains a pileup histo"
		print "-> NOT REWEIGHTING"
		doPileup=False

print "->Getting pu for data" ## TODO, for puTrueInt, this is meaningless
if doPileup:
	pu['data'] = puTarget.Clone("puData")

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
	sumW_histo = ROOT.TH1D("sumWH","histo",1,0,2)
	t.Draw("1>>sumWH","mcWeight" )
	nevents[mc]= sumW_histo.GetBinContent(1)
	if doPileup:
		print "\r" + stdout, " pu ",
		pu[mc] = puTarget.Clone("pu_"+mc)
		pu[mc + '_reweight'] = puTarget.Clone("pu_"+mc + "_reweight") ## try to get the true reweight distribution
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
def DrawHistograms( dict, target="data" ) :
	pup=ROOT.TPad(dict[target].GetName()+ "_pad1" ,"pad1", 0,0.25, 1 ,1)
	pdn=ROOT.TPad(dict[target].GetName()+ "_pad2" ,"pad2", 0,0, 1 ,.25)
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
	data.SetName( data.GetName() + "_data")

	all=data.GetName() + "_allmc"
	dict[ all ] = None
	for mc in book:
		if dict[all] == None : dict[all] = dict[mc].Clone(all)
		else: dict[all].Add(dict[mc])

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
	dict[ target + "_ratio" ] = dict[target] . Clone( target + "_ratio")
	dict[ target + "_ratio" ] . Divide( dict [ all ] ) 
	dict[ target + "_ratio" ] . Draw(" PE ") 
	dict[ target + "_ratio" ] . GetYaxis() . SetRangeUser(0,2)
	dict[ target + "_ratio" ] . GetYaxis() . SetNdivisions(202)
	dict[ target + "_ratio" ] . GetYaxis() . SetTitle("data/MC")
	dict[ target + "_ratio" ] . GetYaxis() . SetTitleOffset(1.2)
	dict[ target + "_ratio" ] . GetXaxis() . SetTitleOffset(4)
	
	pup.cd()


## Loop
mmM={}
mmEta={}
mmPt={}
mmMet={}
mmMetPuppi={}
mmMetPhi={}
mmMetNoHf={}
mmRho={}
mmNpv={}

mmIso={}
mmChIso={}
mmNhIso={}
mmPhoIso={}
mmPuIso={}

mmNjets={}
mmLeadJetPt={}

emPt={}
emNjets={}

eeM={}
eeEta={}
e2Eta={}
e2dPhiJ={}
eePt={}
eeMet={}
eeMetNoHf={}
eeRho={}
eeNpv={}

eeIso={}
eeChIso={}
eeNhIso={}
eePhoIso={}
eePuIso={}

eeNjets={}
eeLeadJetPt={}

ttM={}
ttPt={}
ttMet={}
ttRho={}
ttIso={}
ttLeadPt={}

mmM["data"] =ROOT.TH1D("mmM","mmM;M_{#mu#mu};Events",200,50,150)
mmEta["data"] =ROOT.TH1D("mmEta","mmEta;Eta_{#mu#mu};Events",150,-5,5)
mmPt["data"]=ROOT.TH1D("mmPt","mmPt;p_{T}^{#mu#mu};Events",150,0,300)
mmMet["data"]=ROOT.TH1D("mmMet","mmMet;E_{T}^{miss}(DY#rightarrow#mu#mu); Events",150,0,300)
mmMetPuppi["data"]=ROOT.TH1D("mmMetPuppi","mmMetPuppi;Puppi E_{T}^{miss}(DY#rightarrow#mu#mu); Events",150,0,300)
mmMetPhi["data"]=ROOT.TH1D("mmMetPhi","mmMetPhi;#phi Puppi E_{T}^{miss}(DY#rightarrow#mu#mu); Events",150,-3.1416,3.1416)
mmMetNoHf["data"]=ROOT.TH1D("mmMetNoHf","mmMetNoHf;E_{T}^{miss}(DY#rightarrow#mu#mu); Events",150,0,300)
mmRho["data"] =ROOT.TH1D("mmRho","mmRho;#rho [GeV]; Events",100,0,50)
mmNpv["data"] =ROOT.TH1D("mmNpv","mmNpv;#npv; Events",50,0,50)

mmIso["data"] =ROOT.TH1D("mmIso","mmIso;I_{l1} [GeV]; Events",100,0,10)
mmChIso["data"] =ROOT.TH1D("mmChIso","mmChIso;I_{l1} [GeV]; Events",100,0,10)
mmNhIso["data"] =ROOT.TH1D("mmNhIso","mmNhIso;I_{l1} [GeV]; Events",100,0,10)
mmPhoIso["data"] =ROOT.TH1D("mmPhoIso","mmPhoIso;I_{l1} [GeV]; Events",100,0,10)
mmPuIso["data"] =ROOT.TH1D("mmPuIso","mmPuIso;I_{l1} [GeV]; Events",100,0,10)

mmNjets["data"] = ROOT.TH1D("mmNjets","mmNjets;N_{jets}^{30 GeV} (Z#rightarrow#mu#mu); Events",10,0,10)
mmLeadJetPt["data"] = ROOT.TH1D("mmLeadJetPt","mmLeadJetPt;p_{T}^{j1} (Z#rightarrow#mu#mu); Events",100,0,100)

emPt["data"]=ROOT.TH1D("emPt","emPt;p_{T}^{e#mu};Events",150,0,300)
emNjets["data"]=ROOT.TH1D("emNjets","emNjets;N_{jets}^{30 GeV} (Z#rightarrow e#mu);Events",10,0-.5,10-.5)

eeM["data"] =ROOT.TH1D("eeM","eeM;M_{ee};Events",200,50,150)
eeEta["data"] =ROOT.TH1D("eeEta","eeEta;Eta_{ee};Events",150,-5,5)
e2Eta["data"] =ROOT.TH1D("e2Eta","e2Eta;Eta_{e2};Events",50,-5,5)
e2dPhiJ["data"] =ROOT.TH1D("e2dPhiJ","e2dPhiJ;#Delta#phi(e2,j);Events",50,0,3.1415)
eePt["data"]=ROOT.TH1D("eePt","eePt;p_{T}^{ee};Events",150,0,300)
eeMet["data"]=ROOT.TH1D("eeMet","eeMet;E_{T}^{miss}(DY#rightarrow ee); Events",150,0,300)
eeMetNoHf["data"]=ROOT.TH1D("eeMetNoHf","eeMetNoHf;E_{T}^{miss}(DY#rightarrow ee); Events",150,0,300)
eeRho["data"] =ROOT.TH1D("eeRho","eeRho;#rho [GeV];Events",100,0,50)
eeNpv["data"] =ROOT.TH1D("eeNpv","eeNpv;#rho [GeV];Events",50,0,50)

eeIso["data"] =ROOT.TH1D("eeIso","eeIso;I_{l1} [GeV]; Events",100,0,10)
eeChIso["data"] =ROOT.TH1D("eeChIso","eeChIso;I_{l1} [GeV]; Events",100,0,10)
eeNhIso["data"] =ROOT.TH1D("eeNhIso","eeNhIso;I_{l1} [GeV]; Events",100,0,10)
eePhoIso["data"] =ROOT.TH1D("eePhoIso","eePhoIso;I_{l1} [GeV]; Events",100,0,10)
eePuIso["data"] =ROOT.TH1D("eePuIso","eePuIso;I_{l1} [GeV]; Events",100,0,10)

eeNjets["data"] = ROOT.TH1D("eeNjets","eeNjets;N_{jets}^{30GeV} (Z#rightarrow#mu#mu); Events",10,0,10)
eeLeadJetPt["data"] = ROOT.TH1D("eeLeadJetPt","eeLeadJetPt;p_{T}^{j1} (Z#rightarrow#mu#mu); Events",100,0,100)

ttM["data"] =ROOT.TH1D("ttM","ttM;M_{#tau#tau};Events",150,0,300)
ttPt["data"]=ROOT.TH1D("ttPt","ttPt;p_{T}^{#tau#tau};Events",150,0,300)
ttMet["data"]=ROOT.TH1D("ttMet","ttMet;E_{T}^{miss}(DY#rightarrow #tau#tau); Events",150,0,300)
ttRho["data"] =ROOT.TH1D("ttRho","ttRho;#rho [GeV];Events",100,0,50)
ttIso["data"] =ROOT.TH1D("ttIso","ttIso;I_{l1} [GeV]; Events",100,0,10)
ttLeadPt["data"] =ROOT.TH1D("ttLeadPt","ttLeadPt;p_{T}^{#tau 1} [GeV]; Events",1000,0,1000)

## normalize pu
print "-> normalizing pu: "
if doPileup:
   for what in book + ['data']:
          print what,
          if pu[what].Integral():
          	pu[what].Scale( 1./ pu[what].Integral() ) 

   for what in book:
   	print "Rescaling ", what, "to preserve lumi"
   	s=0.
   	for i in range (0, pu[what].GetNbinsX() ):
   		c=pu[what].GetBinCenter(i+1)
   		num = pu['data'].GetBinContent( pu['data'].FindBin(c) ) 
   		den = pu[what].GetBinContent(i+1)
                if den==0:
                           print "\n EVENT WITH NULL PU REWEIGHT", c
                           num=0
                           den=1
   		s += (num/den)  * den
   	print "-> Scale =", 1./s
   	pu[what].Scale(1./s)

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
			print "File Limit reached for mc", mc, "at",n,"files"
		t.Add(f)

	xsecSum=0.0
	mmM[mc] = mmM["data"].Clone("%s_%s"%(mmM["data"].GetName(),mc))
	mmEta[mc] = mmEta["data"].Clone("%s_%s"%(mmEta["data"].GetName(),mc))
	mmPt[mc] = mmPt["data"].Clone("%s_%s"%(mmPt["data"].GetName(),mc))
	mmMet[mc] = mmMet["data"].Clone("%s_%s"%(mmMet["data"].GetName(),mc))
	mmMetPhi[mc] = mmMetPhi["data"].Clone("%s_%s"%(mmMetPhi["data"].GetName(),mc))
	mmMetPuppi[mc] = mmMetPuppi["data"].Clone("%s_%s"%(mmMetPuppi["data"].GetName(),mc))
	mmMetNoHf[mc] = mmMetNoHf["data"].Clone("%s_%s"%(mmMetNoHf["data"].GetName(),mc))
	mmRho[mc] = mmRho["data"].Clone("%s_%s"%(mmRho["data"].GetName(),mc))
	mmNpv[mc] = mmNpv["data"].Clone("%s_%s"%(mmNpv["data"].GetName(),mc))

	mmIso[mc] = mmIso["data"].Clone("%s_%s"%(mmIso["data"].GetName(),mc))
	mmChIso[mc] = mmChIso["data"].Clone("%s_%s"%(mmChIso["data"].GetName(),mc))
	mmNhIso[mc] = mmNhIso["data"].Clone("%s_%s"%(mmNhIso["data"].GetName(),mc))
	mmPhoIso[mc] = mmPhoIso["data"].Clone("%s_%s"%(mmPhoIso["data"].GetName(),mc))
	mmPuIso[mc] = mmPuIso["data"].Clone("%s_%s"%(mmPuIso["data"].GetName(),mc))

	emPt[mc] = emPt["data"].Clone("%s_%s"%(emPt["data"].GetName(),mc))
	emNjets[mc] = emNjets["data"].Clone("%s_%s"%(emNjets["data"].GetName(),mc))

	eeM[mc] = eeM["data"].Clone("%s_%s"%(eeM["data"].GetName(),mc))
	eeEta[mc] = eeEta["data"].Clone("%s_%s"%(eeEta["data"].GetName(),mc))
	e2Eta[mc] = e2Eta["data"].Clone("%s_%s"%(e2Eta["data"].GetName(),mc))
	e2dPhiJ[mc] = e2dPhiJ["data"].Clone("%s_%s"%(e2dPhiJ["data"].GetName(),mc))
	eePt[mc] = eePt["data"].Clone("%s_%s"%(eePt["data"].GetName(),mc))
	eeMet[mc] = eeMet["data"].Clone("%s_%s"%(eeMet["data"].GetName(),mc))
	eeMetNoHf[mc] = eeMetNoHf["data"].Clone("%s_%s"%(eeMetNoHf["data"].GetName(),mc))
	eeRho[mc] = eeRho["data"].Clone("%s_%s"%(eeRho["data"].GetName(),mc))
	eeNpv[mc] = eeNpv["data"].Clone("%s_%s"%(eeNpv["data"].GetName(),mc))

	eeIso[mc] = eeIso["data"].Clone("%s_%s"%(eeIso["data"].GetName(),mc))
	eeChIso[mc] = eeChIso["data"].Clone("%s_%s"%(eeChIso["data"].GetName(),mc))
	eeNhIso[mc] = eeNhIso["data"].Clone("%s_%s"%(eeNhIso["data"].GetName(),mc))
	eePhoIso[mc] = eePhoIso["data"].Clone("%s_%s"%(eePhoIso["data"].GetName(),mc))
	eePuIso[mc] = eePuIso["data"].Clone("%s_%s"%(eePuIso["data"].GetName(),mc))

	eeNjets[mc] = eeNjets["data"].Clone("%s_%s"%(eeNjets["data"].GetName(),mc))
	eeLeadJetPt[mc] = eeLeadJetPt["data"].Clone("%s_%s"%(eeLeadJetPt["data"].GetName(),mc))
	mmNjets[mc] = mmNjets["data"].Clone("%s_%s"%(mmNjets["data"].GetName(),mc))
	mmLeadJetPt[mc] = mmLeadJetPt["data"].Clone("%s_%s"%(mmLeadJetPt["data"].GetName(),mc))

	ttM[mc] = ttM["data"].Clone("%s_%s"%(ttM["data"].GetName(),mc))
	ttPt[mc] = ttPt["data"].Clone("%s_%s"%(ttPt["data"].GetName(),mc))
	ttMet[mc] = ttMet["data"].Clone("%s_%s"%(ttMet["data"].GetName(),mc))
	ttRho[mc] = ttRho["data"].Clone("%s_%s"%(ttRho["data"].GetName(),mc))
	ttIso[mc] = ttIso["data"].Clone("%s_%s"%(ttIso["data"].GetName(),mc))
	ttLeadPt[mc] = ttLeadPt["data"].Clone("%s_%s"%(ttLeadPt["data"].GetName(),mc))

	for i in range(0, t.GetEntries() ):
		if i&1023 == 1:
			print "\r"+stdout ,i,"/",t.GetEntriesFast(),
			sys.stdout.flush()
		t.GetEntry(i)
		## if t.lepP4.GetEntries()<2 : continue ## 2leptons
		## if t.lepP4[1].Pt() < 20 : continue ## pt 20
		rho = t.rho
		puTrueInt = t.puTrueInt
		w = t.mcWeight * xsections[mc] * lumi / nevents[mc]
		if w == 0 : 
			print "MC weight for mc",mc,"is 0"
			print "\t * mcWeight",t.mcWeight
			print "\t * xsec",xsections[mc]
			print "\t * lumi",lumi
			print "\t * sumW",nevents[mc]

		xsecSum += w/lumi

		## counts jets
		nJets=0
		leadJetPt=0
		leadJetIdx=-1
		for i in range(0, t.jetP4.GetEntries() ):
			if t.lepP4.GetEntries()>0 and t.jetP4[i].DeltaR(t.lepP4[0] ) <0.3  :continue
			if t.lepP4.GetEntries()>1 and t.jetP4[i].DeltaR(t.lepP4[1] ) <0.3  :continue ## remove overlap with leptons
			if t.jetP4[i].Pt()<30 : continue;
			nJets += 1
			if leadJetPt<5 : 
				leadJetIdx = i
				leadJetPt = t.jetP4[i].Pt()
		if doPileup:
			den= pu[mc].GetBinContent(pu[mc].FindBin(puTrueInt)) 
			num= pu[ 'data' ].GetBinContent(pu['data'].FindBin(puTrueInt))
			if den==0: 
				print "\n EVENT WITH NULL PU REWEIGHT", t.eventNum
				num=0
				den=1
			puReweight = float(num)/float(den )
			w*=puReweight
			pu[ mc + "_reweight" ] . Fill( puTrueInt, w ) 
		metCorr = t.metPuppi
		
		## MM
		if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 25 and t.lepPdgId[0]* t.lepPdgId[1] == -13*13 :  ## OS SF muon, leading two
			#print '* DEBUG MM Event', t.lepP4[1].Pt(), t.lepPdgId[0]
			ll = t.lepP4[0] + t.lepP4[1]
			mmRho[mc].Fill( t.rho ,w)
			mmNpv[mc].Fill( t.npv ,w)
			mmM[mc].Fill( ll.M(), w)
			#print "mc pass", ll.M(), ll.Pt() ## DEBUG
			if ll.M() <60 or ll.M() >120 : continue
			mmEta[mc].Fill( ll.Eta(), w)
			mmPt[mc].Fill( ll.Pt() ,w)
			mmMet[mc].Fill( t.metP4[0].Pt(),w )
			mmMetPhi[mc].Fill( metCorr.Phi(),w )
			mmMetPuppi[mc].Fill( metCorr.Pt(),w )
			mmMetNoHf[mc].Fill( t.metNoHF.Pt(),w )
			mmIso[mc].Fill( t.lepIso[0] ,w)
			mmChIso[mc].Fill( t.lepChIso[0] ,w)
			mmNhIso[mc].Fill( t.lepNhIso[0] ,w)
			mmPhoIso[mc].Fill( t.lepPhoIso[0] ,w)
			mmPuIso[mc].Fill( t.lepPuIso[0] ,w)
			mmNjets[mc].Fill( nJets, w) 
			mmLeadJetPt[mc].Fill( leadJetPt,w)

		if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 25 and t.lepPdgId[0]* t.lepPdgId[1] == -13*11 :  ## OS OF muon, leading two
			ll = t.lepP4[0] + t.lepP4[1]
			emPt[mc].Fill( ll.Pt(), w ) 
			emNjets[mc].Fill( nJets, w ) 

		## EE
		if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 25 and t.lepPdgId[0]* t.lepPdgId[1] == -11*11 :  ## OS SF muon, leading two
			#print '* DEBUG EE Event', t.lepP4[1].Pt(), t.lepPdgId[0]
			ll = t.lepP4[0] + t.lepP4[1]
			eeRho[mc].Fill( t.rho,w )
			eeNpv[mc].Fill( t.npv,w )
			eeM[mc].Fill( ll.M(), w)
			if ll.M() <60 or ll.M() >120 : continue
			eeEta[mc].Fill( ll.Eta(), w)
			e2Eta[mc].Fill( t.lepP4[1].Eta(), w)
			if leadJetIdx >=0 : e2dPhiJ[mc].Fill( abs(t.lepP4[1].DeltaPhi( t.jetP4[leadJetIdx]) ), w)
			eePt[mc].Fill( ll.Pt() ,w)
			eeMet[mc].Fill( t.metP4[0].Pt() ,w)
			eeMetNoHf[mc].Fill( t.metNoHF.Pt() ,w)

			eeIso[mc].Fill( t.lepIso[0] ,w)
			eeChIso[mc].Fill( t.lepChIso[0] ,w)
			eeNhIso[mc].Fill( t.lepNhIso[0] ,w)
			eePhoIso[mc].Fill( t.lepPhoIso[0] ,w)
			eePuIso[mc].Fill( t.lepPuIso[0] ,w)

			eeNjets[mc].Fill( nJets, w) 
			eeLeadJetPt[mc].Fill( leadJetPt,w)
		
		## TT
		leadTau = -1
		subleadTau = -1
		for iT in range(0,t.tauP4.GetEntries()) :
			if t.tauP4[iT].Pt()  <51 : continue
			if abs(t.tauP4[iT].Eta()) >2.1 : continue
			if t.tauIso[iT] >1.5 : continue
			#if t.tauSelBits[iT]<0.5: continue

			if leadTau <0 : 
				leadTau = iT
				continue
			if subleadTau <0 :
				subleadTau = iT
				break

		if subleadTau >0 :
			w *= 1./8. #prescale
			ll = t.tauP4[leadTau] + t.tauP4[subleadTau]
			ttRho[mc].Fill( t.rho,w )
			ttM[mc].Fill( ll.M(), w)
			if ll.M() <60 or ll.M() >120 : continue
			ttPt[mc].Fill( ll.Pt() ,w)
			ttMet[mc].Fill( t.metP4[0].Pt() ,w)
			ttIso[mc].Fill( t.tauIso[leadTau] ,w)
			ttLeadPt[mc].Fill( t.tauP4[leadTau].Pt() ,w)

	print '\r'+stdout+" DONE                            "
	print "xsec mc is:", xsections[mc],"~~" ,xsecSum

for data in datasets:
	stdout=" * for data "+ data
	print stdout,
	t=ROOT.TChain("nero/events")
	DeactivateBranches(t)
	n=0
	for f in ReadFromEos( disks[data] ):
	      t.Add(f)
	      n +=1
	stdout += " "+ str(n) + " files"
	print '\r' + stdout,

	t.GetEntry(0)
	triggerNames=t.GetFile().Get("nero/triggerNames").GetTitle()
	trIdx=-1
	for i in range(0,len( triggerNames.split(',') )):
		if 'HLT_LooseIsoPFTau50_Trk30_eta2p1_v' in triggerNames.split(','):
			trIdx= i
			break
	if trIdx<0:
		print "TRIGGER ERROR"

	for i in range(0,t.GetEntries() ):
		if i&1023 == 1:
			print "\r"+stdout ,i,"/",t.GetEntriesFast(),
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
                if t.lepP4[1].Pt() < 25 : continue ## pt 20
		## counts jets
		nJets=0
		leadJetPt=0
		leadJetIdx=-1
		for i in range(0, t.jetP4.GetEntries() ) :
			if t.jetP4[i].DeltaR(t.lepP4[0] ) <0.3  :continue
			if t.jetP4[i].DeltaR(t.lepP4[1] ) <0.3  :continue ## remove overlap with leptons
			if t.jetP4[i].Pt()<30 : continue;
			nJets += 1
			if leadJetPt<5 : 
				leadJetIdx=i
				leadJetPt = t.jetP4[i].Pt()
		#px = t.metPuppi.Px() - metCorrect['x_data'].Eval( t.npv)
		#py = t.metPuppi.Py() - metCorrect['y_data'].Eval(t.npv)
		#metCorr = ROOT.TLorentzVector()
		#metCorr.SetPxPyPzE(px,py,0 ,ROOT.TMath.Sqrt(px*px+py*py))
		metCorr = t.metPuppi

                if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 25 and t.lepPdgId[0]* t.lepPdgId[1] == -13*13 and data=='SingleMuon' : ## OS SF muon, leading two
                	ll = t.lepP4[0] + t.lepP4[1]
			#print "data pass", ll.M(), ll.Pt() ## DEBUG
                	mmM["data"].Fill( ll.M() )
                	mmRho["data"].Fill( t.rho )
                	mmNpv["data"].Fill( t.npv )
			if ll.M() <60 or ll.M() >120 : continue
                	mmEta["data"].Fill( ll.Eta() )
                	mmPt["data"].Fill( ll.Pt() )
			mmMet["data"].Fill( t.metP4[0].Pt() )
			mmMetPhi["data"].Fill( metCorr.Phi() )
			mmMetPuppi["data"].Fill( metCorr.Pt() )
			mmMetNoHf["data"].Fill( t.metNoHF.Pt() )

			mmIso["data"].Fill( t.lepIso[0] )
			mmChIso["data"].Fill( t.lepChIso[0] )
			mmNhIso["data"].Fill( t.lepNhIso[0])
			mmPhoIso["data"].Fill( t.lepPhoIso[0])
			mmPuIso["data"].Fill( t.lepPuIso[0] )
			mmNjets["data"].Fill( nJets) 
			mmLeadJetPt["data"].Fill( leadJetPt)

		if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 25 and t.lepPdgId[0]* t.lepPdgId[1] == -13*11 and data=="SingleMuon":  ## OS OF muon, leading two
			ll = t.lepP4[0] + t.lepP4[1]
			emPt["data"].Fill( ll.Pt()) 
			emNjets["data"].Fill( nJets ) 

                if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 25 and t.lepPdgId[0]* t.lepPdgId[1] == -11*11 and data=='SingleElectron': # OS SF muon, leading two
                	ll = t.lepP4[0] + t.lepP4[1]
                	eeM["data"].Fill( ll.M() )
                	eeRho["data"].Fill( t.rho )
                	eeNpv["data"].Fill( t.npv )
			if ll.M() <60 or ll.M() >120 : continue
                	eeEta["data"].Fill( ll.Eta() )
                	e2Eta["data"].Fill( t.lepP4[1].Eta() )
			if leadJetIdx >=0 : e2dPhiJ["data"].Fill( abs(t.lepP4[1].DeltaPhi( t.jetP4[leadJetIdx]) ))
                	eePt["data"].Fill( ll.Pt() )
			eeMet["data"].Fill( t.metP4[0].Pt() )
			eeMetNoHf["data"].Fill( t.metNoHF.Pt() )

			eeIso["data"].Fill( t.lepIso[0] )
			eeChIso["data"].Fill( t.lepChIso[0] )
			eeNhIso["data"].Fill( t.lepNhIso[0])
			eePhoIso["data"].Fill( t.lepPhoIso[0])
			eePuIso["data"].Fill( t.lepPuIso[0] )
			eeNjets["data"].Fill( nJets ) 
			eeLeadJetPt["data"].Fill( leadJetPt )

		## TT
		leadTau = -1
		subleadTau = -1
		for iT in range(0,t.tauP4.GetEntries()) :
			if t.tauP4[iT].Pt()  <51 : continue
			if t.tauIso[iT] >1.5 : continue
			#if t.tauId[iT]<0.5: continue
			if abs(t.tauP4[iT].Eta()) >2.1 : continue

			if leadTau <0 : 
				leadTau = iT
				continue
			if subleadTau <0 :
				subleadTau = iT
				break
		if subleadTau>0 and data=="Tau" and t.triggerFired[trIdx]: 
			ll = t.tauP4[leadTau] + t.tauP4[subleadTau]
			ttRho["data"].Fill( t.rho )
			ttM["data"].Fill( ll.M())
			if ll.M() <60 or ll.M() >120 : continue
			ttPt["data"].Fill( ll.Pt())
			ttMet["data"].Fill( t.metP4[0].Pt() )
			ttIso["data"].Fill( t.tauIso[leadTau] )
			ttLeadPt["data"].Fill( t.tauP4[leadTau].Pt() )

	print "\r"+stdout+" DONE                               "

print "-> Preparing canvas"
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

canvas=[]

for name in ['mmM','mmPt','mmRho','mmMet',
		'mmIso','mmChIso','mmNhIso', 'mmPhoIso','mmPuIso',
		'mmMetNoHf','mmEta',
		'eeM','eePt','eeRho','eeMet',
		'eeIso','eeChIso','eeNhIso', 'eePhoIso','eePuIso',
		'eeNjets','eeLeadJetPt','mmNjets','mmLeadJetPt',
		'eeMetNoHf','eeEta','e2Eta','e2dPhiJ',
		'ttM','ttPt', 'ttMet','ttRho','ttIso', 'ttLeadPt',
		'eeNpv','mmNpv',
		'mmMetPuppi','mmMetPhi',
		'emPt','emNjets',
		]:

	c=PrepareCanvas(name)
	exec('DrawHistograms('+name+')')
	CMS()
	canvas.append(c)

if not opts.batch:
	raw_input("Looks ok?")

for c in canvas:
	c.SaveAs(opts.plotdir + "/" + c.GetName() + ".pdf")
	c.SaveAs(opts.plotdir + "/" + c.GetName() + ".png")
	c.SaveAs(opts.plotdir + "/" + c.GetName() + ".root")
	## SAVE ALSO THE HISTOGRAMS
	f= ROOT.TFile.Open(opts.plotdir + "/" + c.GetName() + ".root","UPDATE")
	f.cd()
	exec("dict="+ c.GetName() )
	for histo in dict: # c.GetName is also the dictionary
		dict[histo].Write()
	f.Close()

if doPileup:
   f= ROOT.TFile.Open(opts.plotdir + "/pu.root", "RECREATE" )
   f.cd()
   for h in pu:
        pu[h].Write()

print "--- END ---"

