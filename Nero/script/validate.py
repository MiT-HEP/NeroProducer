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
#parser.add_option("","--jec",dest="jec",type='string',help="apply jet energy corrections. eg 'jec/Summer15_50nsV2_MC'  [CMSSW]",default=""); 

opts, args = parser.parse_args()

if opts.batch:
	ROOT.gROOT.SetBatch()


version="v1.0"

disks={}
xsections={}
nevents={}
pu={}
lumi=opts.lumi ## pb

fileLimit=-1

book=['DY','WZ','ZZ','WW','WJets','TTJets'] ## WJets is very big with low eff for double muon
#book=['DY','WZ','ZZ','WW','TTJets']
datasets=['SingleMuon','SingleElectron','Tau']
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
	fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 ]
	return fileList

def DeactivateBranches(t):
	''' deactivate used branches and speed up '''
	t.SetBranchStatus("jet*",0)
	t.SetBranchStatus("fatjet*",0)
	t.SetBranchStatus("tau*",0)
	t.SetBranchStatus("trigger*",0)
	t.SetBranchStatus("pdf*",0)
	t.SetBranchStatus("gen*",0)
	t.SetBranchStatus("photon*",0)
	t.SetBranchStatus("ak8*",0)
	t.SetBranchStatus("jetP4",1)
	t.SetBranchStatus("tauP4",1)
	t.SetBranchStatus("tauIso",1)
	t.SetBranchStatus("tauId",1)

### if a json is given parse it
if opts.json != "":
	jstring = open(opts.json).read() 
	goodLumis = json.loads( jstring )

## compute nevents
print "-> compute nevents"
puTarget=ROOT.TH1D("pu","pu",100,0,50) # rho

for mc in book:
	stdout = " * for mc " +  mc
	print stdout,
	sys.stdout.flush()
	t=ROOT.TChain("nero/all")	
	te=ROOT.TChain("nero/events")	
	n=0
	ntries = 0 
	for f in ReadFromEos( disks[mc] ):
		n += 1 
		if fileLimit>=0 and n > fileLimit: 
			print "File Limit reached for mc", mc, "at",n,"files"
			break
		t.Add(f)
		te.Add(f)
		if opts.limit>=0:
			ntries = t.GetEntries()
			if ntries > opts.limit: fileLimit = n
	stdout += " "+str(n) +" files added"
	print "\r"+stdout,
	sys.stdout.flush()
	SW=0.
	t.SetBranchStatus("*",0)
	t.SetBranchStatus("mcWeight",1)
	for i in range(0,t.GetEntries()):
		if i&1023 == 1:
			print "\r"+ stdout ,i,"/",t.GetEntriesFast(),
			sys.stdout.flush()
		t.GetEntry(i)
		SW += t.mcWeight
	nevents[mc]=SW
	stdout += "SumW=" + str(SW)
	print "\r" + stdout, "pu",
	pu[mc] = puTarget.Clone("pu_"+mc)
	te.Draw("rho>>pu_"+mc,"lepP4[1].Pt()>20","goff")
	print "DONE"

print "->Getting pu for data"

for data in datasets:
	stdout=" * for data "+ data
	print stdout,
	sys.stdout.flush()
	t=ROOT.TChain("nero/events")
	n=0
	for f in ReadFromEos( disks[data] ):
	      t.Add(f)
	      n +=1
	stdout += " "+ str(n) + " files"
	print '\r' + stdout,
	pu[data]= puTarget.Clone("pu_"+data)
	# I need to apply the json
	#t.Draw("rho>>pu_"+data,"lepP4[1].Pt()>20","goff")
	t.SetBranchStatus("*",0)
	t.SetBranchStatus("runNum",1)
	t.SetBranchStatus("lumiNum",1)
	t.SetBranchStatus("rho",1)
	t.SetBranchStatus("lepP4",1)
	for i in range(0,t.GetEntries() ):
		t.GetEntry(i)
		if opts.json != "":
			# filter accordingly to a json file
			run= str(t.runNum)
			if run not in goodLumis.keys(): continue
			isGood=False
			for lumis in goodLumis[run] :
				if t.lumiNum >= lumis[0] and t.lumiNum <= lumis[1] : isGood = True
			if not isGood: continue
		if t.lepP4.GetEntries() <2 : continue
		if t.lepP4[1].Pt() <20 :continue
		pu[data].Fill( t.rho )
	stdout += " pu DONE "
	print '\r' + stdout



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
mmPt={}
mmMet={}
mmRho={}

mmIso={}
mmChIso={}
mmNhIso={}
mmPhoIso={}
mmPuIso={}

mmNjets={}
mmLeadJetPt={}


eeM={}
eePt={}
eeMet={}
eeRho={}

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
mmPt["data"]=ROOT.TH1D("mmPt","mmPt;p_{T}^{#mu#mu};Events",150,0,300)
mmMet["data"]=ROOT.TH1D("mmMet","mmMet;E_{T}^{miss}(DY#rightarrow#mu#mu); Events",150,0,300)
mmRho["data"] =ROOT.TH1D("mmRho","mmRho;#rho [GeV]; Events",100,0,50)

mmIso["data"] =ROOT.TH1D("mmIso","mmIso;I_{l1} [GeV]; Events",100,0,10)
mmChIso["data"] =ROOT.TH1D("mmChIso","mmChIso;I_{l1} [GeV]; Events",100,0,10)
mmNhIso["data"] =ROOT.TH1D("mmNhIso","mmNhIso;I_{l1} [GeV]; Events",100,0,10)
mmPhoIso["data"] =ROOT.TH1D("mmPhoIso","mmPhoIso;I_{l1} [GeV]; Events",100,0,10)
mmPuIso["data"] =ROOT.TH1D("mmPuIso","mmPuIso;I_{l1} [GeV]; Events",100,0,10)

mmNjets["data"] = ROOT.TH1D("mmNjets","mmNjets;N_{jets} (Z#rightarrow#mu#mu); Events",10,0,10)
mmLeadJetPt["data"] = ROOT.TH1D("mmLeadJetPt","mmLeadJetPt;p_{T}^{j1} (Z#rightarrow#mu#mu); Events",100,0,100)

eeM["data"] =ROOT.TH1D("eeM","eeM;M_{ee};Events",150,0,300)
eePt["data"]=ROOT.TH1D("eePt","eePt;p_{T}^{ee};Events",150,0,300)
eeMet["data"]=ROOT.TH1D("eeMet","eeMet;E_{T}^{miss}(DY#rightarrow ee); Events",150,0,300)
eeRho["data"] =ROOT.TH1D("eeRho","eeRho;#rho [GeV];Events",100,0,50)

eeIso["data"] =ROOT.TH1D("eeIso","eeIso;I_{l1} [GeV]; Events",100,0,10)
eeChIso["data"] =ROOT.TH1D("eeChIso","eeChIso;I_{l1} [GeV]; Events",100,0,10)
eeNhIso["data"] =ROOT.TH1D("eeNhIso","eeNhIso;I_{l1} [GeV]; Events",100,0,10)
eePhoIso["data"] =ROOT.TH1D("eePhoIso","eePhoIso;I_{l1} [GeV]; Events",100,0,10)
eePuIso["data"] =ROOT.TH1D("eePuIso","eePuIso;I_{l1} [GeV]; Events",100,0,10)

eeNjets["data"] = ROOT.TH1D("eeNjets","eeNjets;N_{jets} (Z#rightarrow#mu#mu); Events",10,0,10)
eeLeadJetPt["data"] = ROOT.TH1D("eeLeadJetPt","eeLeadJetPt;p_{T}^{j1} (Z#rightarrow#mu#mu); Events",100,0,100)

ttM["data"] =ROOT.TH1D("ttM","ttM;M_{#tau#tau};Events",150,0,300)
ttPt["data"]=ROOT.TH1D("ttPt","ttPt;p_{T}^{#tau#tau};Events",150,0,300)
ttMet["data"]=ROOT.TH1D("ttMet","ttMet;E_{T}^{miss}(DY#rightarrow #tau#tau); Events",150,0,300)
ttRho["data"] =ROOT.TH1D("ttRho","ttRho;#rho [GeV];Events",100,0,50)
ttIso["data"] =ROOT.TH1D("ttIso","ttIso;I_{l1} [GeV]; Events",100,0,10)
ttLeadPt["data"] =ROOT.TH1D("ttLeadPt","ttLeadPt;p_{T}^{#tau 1} [GeV]; Events",100,0,10)

## normalize pu
print "-> normalizing pu: "
for what in book + datasets:
	print what,
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
			print "File Limit reached for mc", mc, "at",n,"files"
		t.Add(f)

	mmM[mc] = mmM["data"].Clone("%s_%s"%(mmM["data"].GetName(),mc))
	mmPt[mc] = mmPt["data"].Clone("%s_%s"%(mmPt["data"].GetName(),mc))
	mmMet[mc] = mmMet["data"].Clone("%s_%s"%(mmMet["data"].GetName(),mc))
	mmRho[mc] = mmRho["data"].Clone("%s_%s"%(mmRho["data"].GetName(),mc))

	mmIso[mc] = mmIso["data"].Clone("%s_%s"%(mmIso["data"].GetName(),mc))
	mmChIso[mc] = mmChIso["data"].Clone("%s_%s"%(mmChIso["data"].GetName(),mc))
	mmNhIso[mc] = mmNhIso["data"].Clone("%s_%s"%(mmNhIso["data"].GetName(),mc))
	mmPhoIso[mc] = mmPhoIso["data"].Clone("%s_%s"%(mmPhoIso["data"].GetName(),mc))
	mmPuIso[mc] = mmPuIso["data"].Clone("%s_%s"%(mmPuIso["data"].GetName(),mc))

	eeM[mc] = eeM["data"].Clone("%s_%s"%(eeM["data"].GetName(),mc))
	eePt[mc] = eePt["data"].Clone("%s_%s"%(eePt["data"].GetName(),mc))
	eeMet[mc] = eeMet["data"].Clone("%s_%s"%(eeMet["data"].GetName(),mc))
	eeRho[mc] = eeRho["data"].Clone("%s_%s"%(eeRho["data"].GetName(),mc))

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
		for i in range(0, t.jetP4.GetEntries() ):
			if t.lepP4.GetEntries()>0 and t.jetP4[i].DeltaR(t.lepP4[0] ) <0.3  :continue
			if t.lepP4.GetEntries()>1 and t.jetP4[i].DeltaR(t.lepP4[1] ) <0.3  :continue ## remove overlap with leptons
			nJets += 1
			if leadJetPt<5 : leadJetPt = t.jetP4[i].Pt()

		## MM
		if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 20 and t.lepPdgId[0]* t.lepPdgId[1] == -13*13 :  ## OS SF muon, leading two
			den= pu[mc].GetBinContent(pu[mc].FindBin(rho)) 
			num= pu[ 'SingleMuon' ].GetBinContent(pu['SingleMuon'].FindBin(rho))
			if den==0: 
				print "\n EVENT WITH NULL PU REWEIGHT", t.eventNum
				num=0
				den=1
			puReweight = float(num)/float(den )
			w *= puReweight
			ll = t.lepP4[0] + t.lepP4[1]
			mmRho[mc].Fill( t.rho ,w)
			mmM[mc].Fill( ll.M(), w)
			#print "mc pass", ll.M(), ll.Pt() ## DEBUG
			if ll.M() <60 or ll.M() >120 : continue
			mmPt[mc].Fill( ll.Pt() ,w)
			mmMet[mc].Fill( t.metP4[0].Pt(),w )
			mmIso[mc].Fill( t.lepIso[0] ,w)
			mmChIso[mc].Fill( t.lepChIso[0] ,w)
			mmNhIso[mc].Fill( t.lepNhIso[0] ,w)
			mmPhoIso[mc].Fill( t.lepPhoIso[0] ,w)
			mmPuIso[mc].Fill( t.lepPuIso[0] ,w)
			mmNjets[mc].Fill( nJets, w) 
			mmLeadJetPt[mc].Fill( leadJetPt,w)

		## EE
		if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 20 and t.lepPdgId[0]* t.lepPdgId[1] == -11*11 :  ## OS SF muon, leading two
			den= pu[mc].GetBinContent(pu[mc].FindBin(rho)) 
			num= pu[ 'SingleElectron' ].GetBinContent(pu['SingleElectron'].FindBin(rho))
			if den==0: 
				num=0
				den=1
			puReweight = float(num)/float(den) 
			w *= puReweight
			ll = t.lepP4[0] + t.lepP4[1]
			eeRho[mc].Fill( t.rho,w )
			eeM[mc].Fill( ll.M(), w)
			if ll.M() <60 or ll.M() >120 : continue
			eePt[mc].Fill( ll.Pt() ,w)
			eeMet[mc].Fill( t.metP4[0].Pt() ,w)

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
			if t.tauP4[iT].Pt()  <30 : continue
			if abs(t.tauP4[iT].Eta()) >2.1 : continue
			if t.tauIso[iT] >1.5 : continue
			if t.tauId[iT]<0.5: continue

			if leadTau <0 : 
				leadTau = iT
				continue
			if subleadTau <0 :
				subleadTau = iT
				break

		if subleadTau >0 :
			den= pu[mc].GetBinContent(pu[mc].FindBin(rho)) 
			num= pu[ 'Tau' ].GetBinContent(pu['Tau'].FindBin(rho))
			if den==0: 
				num=0
				den=1
			puReweight = float(num)/float(den) 
			w *= puReweight
			ll = t.tauP4[leadTau] + t.tauP4[subleadTau]
			ttRho[mc].Fill( t.rho,w )
			ttM[mc].Fill( ll.M(), w)
			if ll.M() <60 or ll.M() >120 : continue
			ttPt[mc].Fill( ll.Pt() ,w)
			ttMet[mc].Fill( t.metP4[0].Pt() ,w)
			ttIso[mc].Fill( t.tauIso[leadTau] ,w)
			ttLeadPt[mc].Fill( t.tauP4[leadTau].Pt() ,w)

	print '\r'+stdout+" DONE                            "


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
                if t.lepP4[1].Pt() < 20 : continue ## pt 20
		## counts jets
		nJets=0
		leadJetPt=0
		for i in range(0, t.jetP4.GetEntries() ) :
			if t.jetP4[i].DeltaR(t.lepP4[0] ) <0.3  :continue
			if t.jetP4[i].DeltaR(t.lepP4[1] ) <0.3  :continue ## remove overlap with leptons
			nJets += 1
			if leadJetPt<5 : leadJetPt = t.jetP4[i].Pt()

                if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 20 and t.lepPdgId[0]* t.lepPdgId[1] == -13*13 and data=='SingleMuon' : ## OS SF muon, leading two
                	ll = t.lepP4[0] + t.lepP4[1]
			#print "data pass", ll.M(), ll.Pt() ## DEBUG
                	mmM["data"].Fill( ll.M() )
                	mmRho["data"].Fill( t.rho )
			if ll.M() <60 or ll.M() >120 : continue
                	mmPt["data"].Fill( ll.Pt() )
			mmMet["data"].Fill( t.metP4[0].Pt() )

			mmIso["data"].Fill( t.lepIso[0] )
			mmChIso["data"].Fill( t.lepChIso[0] )
			mmNhIso["data"].Fill( t.lepNhIso[0])
			mmPhoIso["data"].Fill( t.lepPhoIso[0])
			mmPuIso["data"].Fill( t.lepPuIso[0] )
			mmNjets["data"].Fill( nJets) 
			mmLeadJetPt["data"].Fill( leadJetPt)

                if t.lepP4.GetEntries()>=2 and t.lepP4[1].Pt() > 20 and t.lepPdgId[0]* t.lepPdgId[1] == -11*11 and data=='SingleElectron': # OS SF muon, leading two
                	ll = t.lepP4[0] + t.lepP4[1]
                	eeM["data"].Fill( ll.M() )
                	eeRho["data"].Fill( t.rho )
			if ll.M() <60 or ll.M() >120 : continue
                	eePt["data"].Fill( ll.Pt() )
			eeMet["data"].Fill( t.metP4[0].Pt() )

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
			if t.tauP4[iT].Pt()  <30 : continue
			if t.tauIso[iT] >1.5 : continue
			if t.tauId[iT]<0.5: continue
			if abs(t.tauP4[iT].Eta()) >2.1 : continue

			if leadTau <0 : 
				leadTau = iT
				continue
			if subleadTau <0 :
				subleadTau = iT
				break
		if subleadTau>0 and data=="Tau":
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
		'eeM','eePt','eeRho','eeMet',
		'eeIso','eeChIso','eeNhIso', 'eePhoIso','eePuIso',
		'eeNjets','eeLeadJetPt','mmNjets','mmLeadJetPt',
		'ttM','ttPt', 'ttMet','ttRho','ttIso', 'ttLeadPt',
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

f= ROOT.TFile.Open(opts.plotdir + "/pu.root", "RECREATE" )
f.cd()
for h in pu:
	pu[h].Write()

