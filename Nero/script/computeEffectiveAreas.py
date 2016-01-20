#!/bin/env python

import os,sys,re
import math
from subprocess import call, check_output

from optparse import OptionParser
parser= OptionParser()

parser.add_option("-e","--eos",dest="eos",type="string",help="Comma separated list of backgrounds. [%default]",default="/store/user/amarini/Nero/v1.2/SingleElectron/");
parser.add_option("-j","--json",dest="json",type='string',help="json file",default=""); 
parser.add_option("","--pt",dest="pt",type='string',help="pt binning [%default]",default="0,8000"); 
parser.add_option("","--eta",dest="eta",type='string',help="aeta binning [%default]",default="0,1,1.479,2.0,2.5"); 
parser.add_option("-d","--dir",dest="dir",type='string',help="output dir to save plots and root files",default="plot"); 

opts,args = parser.parse_args()

etabins= [float(s) for s in opts.eta.split(',')] 
ptbins= [float(s) for s in opts.pt.split(',')] 

sys.argv=[]
import ROOT as r
r.gROOT.SetBatch()

EOS = "/afs/cern.ch/project/eos/installation/0.3.84-aquamarine/bin/eos.select"

cmd = EOS+ " find -f " + opts.eos
outputList = check_output(cmd,shell=True)
fileList0 = outputList.split() ## change lines into list
fileList = [ re.sub("/eos/cms","root://eoscms//",f) for f in fileList0 if '/failed/' not in f ]

n=0
chain = r.TChain("nero/events")
for f in fileList:
	n+=chain.Add(f)
if n==0:
	print "no file: trying local"
	n+=chain.Add(opts.eos)

print "-> Added ",n,"files to the chain. Chain has",chain.GetEntries(),"entries"

chain.SetBranchStatus("*",0)
chain.SetBranchStatus("lep*",1)
chain.SetBranchStatus("rho",1)
chain.SetBranchStatus("npv",1)
## for json
chain.SetBranchStatus("runNum",1)
chain.SetBranchStatus("lumiNum",1)
chain.SetBranchStatus("eventNum",1)

### if a json is given parse it
if opts.json != "":
	print "-> Parsing JSON FIXME",opts.json
	jstring = open(opts.json).read() 
	goodLumis = json.loads( jstring )

### histo container
histos={}

def findRange(value,list):
	for i in range(0,len(list) ):
		if i+1 >= len(list) : continue
		if value>= list[i] and value < list[i+1]: return (list[i],list[i+1])
	return (-1,-1)

def fill( type, pt, eta,x,y):
	''' Fill value of isolation and if not exists create the profile for the bin corresponding to pt,eta.
		Use Global Variables ptbins, etabins
	'''
	pt0,pt1 = findRange( pt, ptbins)
	eta0,eta1 = findRange( abs(eta), etabins)

	name= "%s_pt%.0f_%.0f_eta%.1f_%.1f"%(type,pt0,pt1,eta0,eta1)

	if name not in histos:
		print "\n  * Creating histo with name",name
		if type.startswith('th2d'):
			histos[name]= r.TH2D(name,name,200,0,100,100,0,30)
		elif type.startswith('tprofile'):
			histos[name]= r.TProfile(name,name,200,0,100,100,0,30)
		elif type.startswith('th1d'):
			histos[name]= r.TH1D(name,name,200,0,100)
	histos[name].Fill(x,y)

def mean(l):
	tot=sum(l)
	n= len(l)
	if n==0: return 0
	return float(tot)/float(n)

def rms(l):
	m=mean(l)
	n= len(l)
	s=0.
	for v in l: s+=  (v-m)**2

	if n<=1: return 0
	return s/float(n-1)

def findQuantile(h, quantile, ntoys=100):
	''' return cut at quantile and error '''
	rand = r.TRandom3(122345)
	tot= h.Integral()
	mybin=-1
	for ibin in range(1,h.GetNbinsX() +1 ):
		if h.Integral(1,ibin) > quantile : 
			mybin= ibin
			break
	mymean = h.GetBinCenter(ibin)
	mylist=[]
	for itoy in range(0,ntoys):
		toy = h.Clone("toy%d"%itoy)
		for ibin in range(1,h.GetNbinsX() +1):
			c = toy.GetBinContent(ibin)
			toy.SetBinContent(ibin, rand.Poisson(c))
		tot= h.Integral()
		for ibin in range(1,h.GetNbinsX() +1):
			if toy.Integral(1,ibin) > quantile : 
				mylist.append(toy.GetBinCenter(ibin))
				break
		toy.Delete()
	return (mymean,math.sqrt(rms(mylist) ) )

print "-> Setting up Profiling"
sw=[]  ### PROFILE
for i in range(0,10): ## PROFILE
	sw.append(r.TStopwatch() ) ## PROFILE
	sw[i].Reset() ## PROFILE

last=0
def Start(i):
	global last
	if i>0: sw[i-1].Stop()
	elif i==0: sw[last].Stop()
	last=i
	sw[i].Start(0) ## 0 = do not reset
	return

def Reset():
	for s in sw:
		s.Stop()
		s.Reset()
	return

### LOOP ###
print "-> Start Loop"
try:
   for i in range(0,chain.GetEntries() ):
	if i&1023 == 1:
		print "\r Doing Entries" ,i,"/",chain.GetEntriesFast(),
		print " PROFILE (CPU,REAL):",
		print " GetEntry (" + "%.2f"%(sw[0].CpuTime()) + "," +"%.2f"%(sw[0].RealTime()) +")",
		print " TProfile-1 (" + "%.2f"%(sw[1].CpuTime()) + ","+ "%.2f"%(sw[1].RealTime()) +")",
		## print " TProfile-2 (" + str(sw[2].CpuTime()) + ","+ str(sw[2].RealTime()) +")",
		print " TH2D (" + "%.2f"%(sw[3].CpuTime()) + ","+ "%.2f"%(sw[3].RealTime()) +")",
		sys.stdout.flush()
		Reset()

	Start(0) ## PROFILE
	chain.GetEntry(i)

	Start(1) ## PROFILE
	if chain.lepP4.GetEntries()==0 or abs(chain.lepPdgId[0] ) != 11	: continue
	#fill("tprofile_ch_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepChIso[0])
	#fill("tprofile_nh_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepNhIso[0])
	#fill("tprofile_ph_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepPhoIso[0])
	#fill("tprofile_pu_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepPuIso[0])
	#fill("tprofile_tot_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepIso[0])

	Start(2) ## PROFILE
	##fill("tprofile_ch_npv",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.npv,chain.lepChIso[0])
	##fill("tprofile_nh_npv",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.npv,chain.lepNhIso[0])
	##fill("tprofile_ph_npv",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.npv,chain.lepPhoIso[0])
	##fill("tprofile_pu_npv",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.npv,chain.lepPuIso[0])
	##fill("tprofile_tot_npv",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.npv,chain.lepIso[0])

	Start(3) ## PROFILE
	fill("th2d_ch_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepChIso[0])
	fill("th2d_nh_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepNhIso[0])
	fill("th2d_ph_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepPhoIso[0])
	fill("th2d_pu_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepPuIso[0])
	fill("th2d_tot_rho",chain.lepP4[0].Pt(),chain.lepP4[0].Eta(),chain.rho,chain.lepIso[0])

except KeyboardInterrupt: 
	print "Caught Keyboard Interrupt. Stop Looping."
	print "Loop ended at entry",i
	pass

## Produce profile
for h in histos.keys():
	if h.startswith("th2d"):
		name=re.sub("th2d","tprofile",h)
		histos[name]=histos[h].ProfileX()

line=r.TF1("line","[0]+x*[1]",0,100)
quad=r.TF1("quad","[0]+x*[1]+x*x*[2]",0,100)

quad.SetLineColor(r.kBlue)
quad.SetLineStyle(3)

text=r.TLatex()
text.SetNDC()
text.SetTextFont(42)
text.SetTextSize(0.04)
text.SetTextAlign(11)

txt=open(opts.dir+"/effArea.txt","w")
fOut=r.TFile.Open(opts.dir+"/effArea.root","RECREATE") ## save histos 
for h in histos.keys():
	histos[h].Write()
	## draw the canvas
	c=r.TCanvas("c_"+h,h)
	## mean eff area
	if h.startswith('tprofile'):
		histos[h].Draw("PE")
		## fit with a line
		histos[h].Fit(line,"Q")
		histos[h].Fit(line,"QM")

		histos[h].Fit(quad,"Q")
		histos[h].Fit(quad,"QM")
	elif h.startswith('th2d'):
		## compute effective area in 90%
		name=re.sub('th2d','th1d', h)
		#TH1D * 	ProjectionX (const char *name="_px", Int_t firstybin=0, Int_t lastybin=-1, Option_t *option="") const 
		th1d = r.TH1D(name,name,200,0,100) ## this is the equivalent of the profile to be fitted
		for i in range(0, th1d.GetNbinsX()):
			ibin = i+1
			iso = histos[h].ProjectionY("_py",ibin, ibin ) 
			v,e = findQuantile(iso, .90, 100)
			th1d.SetBinContent(ibin,v)
			th1d.SetBinError(ibin,v)
		th1d.Write()
		th1d.Draw("PE")
		th1d.Fit(line,"Q")
		th1d.Fit(line,"QM")

		th1d.Fit(quad,"Q")
		th1d.Fit(quad,"QM")
	
	line.Draw("L SAME")
	quad.Draw("L SAME")
	text.DrawLatex(0.12,.87,"m=%.3f"%line.GetParameter(0))
	text.DrawLatex(0.12,.83,"q=%.3f"%line.GetParameter(1))
	txt.write(h + " " + str(line.GetParameter(0)) + " " +str(line.GetParameter(1))  + " quad "+ str(quad.GetParameter(0))+ str(quad.GetParameter(1))+ " "+str(quad.GetParameter(2))+"\n")
	## save the canvas
	c.SaveAs(opts.dir + "/" + h + ".pdf")
	c.SaveAs(opts.dir + "/" + h + ".png")
	c.SaveAs(opts.dir + "/" + h + ".root")
	##delete the canvas
	c.Delete()

fOut.Close()
