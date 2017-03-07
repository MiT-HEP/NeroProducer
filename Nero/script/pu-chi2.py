from optparse import OptionParser

parser = OptionParser(usage="usage")
parser.add_option("-f","--file",type="string",help="Input File [%default]",default="plot/mmNpv.root")
#parser.add_option("","--points",type="string",help="point numbers comma separated",default="65000,67000,68000,69000,69200,69300,69400,69500,69600,69800,70000,71000,73000")
parser.add_option("","--points",type="string",help="point numbers comma separated [%default]",default="78000,77000,76000,75000,74000,73000,72000,71000,70800,70600,70400,70200,70000,69800,69600,69400,69200,69000,68000,67000,66000,65000,64000,63000")
parser.add_option("-s","--scale",action='store_true',help="Scale [%default]",default=False)
opts,args= parser.parse_args()

import ROOT
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)
ROOT.gStyle.SetOptFit(0111)


fROOT = ROOT.TFile.Open(opts.file)
th1d_chi2=ROOT.TH1D("chi2","chi2",(75-60)*10,60000,75000)

data=fROOT.Get("mmNpv")

for n in opts.points.split(","):
	num = int(n)
	mc=fROOT.Get("mmNpv_DY_pileup%d"%( num  ) )
	if opts.scale:
		mc.Scale(data.Integral()/mc.Integral())

	chiSquare=0.
	for i in range(0,data.GetNbinsX()):
		iBin=i+1
		den = data.GetBinContent(iBin)
		if den<=0 : den= 1
		chiSquare +=  (mc.GetBinContent(iBin) - data.GetBinContent(iBin) )**2 / den
	bin=th1d_chi2.FindBin(num)
	th1d_chi2.SetBinContent(bin,chiSquare)


c=ROOT.TCanvas("c","c")
c.SetBottomMargin(.15)
c.SetLeftMargin(.15)
c.SetTopMargin(.05)
c.SetRightMargin(.05)

th1d_chi2.Draw("AXIS")
th1d_chi2.Draw("AXIS X+ Y+ SAME")

th1d_chi2.GetXaxis().SetTitle("pu^{true}")
th1d_chi2.GetYaxis().SetTitle("#chi^{2}")
th1d_chi2.GetXaxis().SetTitleOffset(1.2)
th1d_chi2.GetYaxis().SetTitleOffset(1.2)

th1d_chi2.SetMarkerStyle(20)
th1d_chi2.Draw("P SAME")

f=ROOT.TF1("myf","pol2",60e3,75e3)
th1d_chi2.Fit("myf","QN")
th1d_chi2.Fit("myf","QM")
#f.Draw("L SAME")

raw_input("ok?")
name ="chi2"
if opts.scale: name+="_scaled"
c.SaveAs(name+ ".pdf")
c.SaveAs(name+ ".png")

