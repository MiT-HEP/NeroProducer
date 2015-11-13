import ROOT
from optparse import OptionParser

parser = OptionParser(usage="usage")
parser.add_option("-f","--file",type="string",help="Input File",default="plotPu2/mmNpv.root")
parser.add_option("","--base",type="string",help="base",default="67")
parser.add_option("","--envelope",type="string",help="envelope numbers comma separated",default="69,71")

ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)

opts,args= parser.parse_args()

# mmNpv
# mmNpv_DY_pileup75

fROOT = ROOT.TFile.Open(opts.file)
data=fROOT.Get("mmNpv")
base=fROOT.Get("mmNpv_DY_pileup%d"%( int (opts.base )) )
lumi=579

if base == None:
	print "BASE IS NONE"

envelope = []
for n in opts.envelope.split(','):
	num=int(n)
	h = fROOT.Get("mmNpv_DY_pileup%d"%( num ) )
	if h==None:
		print "NUM %d IS NONE"%(num)
	envelope.append(h)

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
	#ltx.SetTextAlign(13)
	ltx.SetTextAlign(33)
	ltx.SetTextSize(0.05)
	ltx.DrawLatex(.90,.93,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") ##ONLY TCANVAS
	#ltx.DrawLatex(.17,.95,"#bf{CMS},#scale[0.75]{#it{ Preliminary}}") 




data.SetMarkerStyle(20)
data.SetMarkerColor(ROOT.kBlack)
data.SetLineColor(ROOT.kBlack)

 ## NORMALIZE
integral = data.Integral()
base.Scale(integral / base.Integral() )

for mc in envelope:
	mc.Scale( integral/mc.Integral())


base.SetLineColor(ROOT.kBlack)
base.SetLineWidth(2)
base.SetFillColor( ROOT.kCyan )
base.GetYaxis().SetTitleOffset(1.2)
base.GetXaxis().SetTitle( data.GetXaxis().GetTitle() )
base.GetYaxis().SetTitle( data.GetYaxis().GetTitle() )
base.GetXaxis().SetLabelFont(43)
base.GetXaxis().SetTitleFont(43)
base.GetYaxis().SetLabelFont(43)
base.GetYaxis().SetTitleFont(43)
base.GetXaxis().SetLabelSize(26)
base.GetYaxis().SetLabelSize(26)
base.GetXaxis().SetTitleSize(28)
base.GetYaxis().SetTitleSize(28)
base.GetXaxis().SetTitleOffset(1.2)
base.GetYaxis().SetTitleOffset(1.2)

data.GetXaxis().SetLabelFont(43) ## for the ratio
data.GetXaxis().SetTitleFont(43)
data.GetYaxis().SetLabelFont(43)
data.GetYaxis().SetTitleFont(43)
data.GetXaxis().SetLabelSize(26)
data.GetYaxis().SetLabelSize(26)
data.GetXaxis().SetTitleSize(28)
data.GetYaxis().SetTitleSize(28)

## Compute the envelope
env=base.Clone("envelope")
env.Reset("ACE")
for idx,mc in enumerate(envelope):
   for i in range(0,mc.GetNbinsX()+2):
	c= env.GetBinContent(i)
	e= env.GetBinError(i)

	c2=mc.GetBinContent(i)

	if idx!=0  : 
		up = max(c+e,c2) 
		down = min(c-e,c2)
	else: ## first element just take it as is
		up = c2
		down = c2

	env.SetBinContent(i, (up+down)/2.0)
	env.SetBinError(i,(up-down)/2.0)
env.SetFillColor(ROOT.kRed)
env.SetFillStyle(3001)
env.SetMarkerStyle(0)
env.SetMarkerColor(ROOT.kRed)

## draw
c=PrepareCanvas()

pup=ROOT.TPad("pad1" ,"pad1", 0,0.25, 1 ,1)
pdn=ROOT.TPad("pad2" ,"pad2", 0,0, 1 ,.25)

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

base.Draw("HIST")
env.Draw("PE2 SAME")
data.Draw("P E SAME")

CMS()
## ratio

pdn.cd()
pdn.SetGridy()

a=base.Clone("all")
for i in range(0,a.GetNbinsX()+2):
	a.SetBinError(i,0)

data_r = data.Clone("data_r")
data_r.Divide(a)

env_r = env.Clone("env_r")
env_r.Divide(a)

env_r.Draw("PE2")
data_r.Draw("PE SAME")

env_r.GetYaxis().SetRangeUser(0,2)
env_r.GetXaxis().SetTitleOffset(3)

raw_input("ok?")
