from optparse import OptionParser

parser = OptionParser(usage="usage")
parser.add_option("-f","--file",type="string",help="Input File",default="plotPu2/mmNpv.root")
parser.add_option("","--points",type="string",help="point numbers comma separated",default="65000,67000,68000,69000,69200,69300,69400,69500,69600,69800,70000,71000,73000")
parser.add_option("-s","--scale",action='store_true',help="Scale default=%default",default=False)
opts,args= parser.parse_args()

import ROOT
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(0)


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
th1d_chi2.SetMarkerStyle(20)
th1d_chi2.Draw("P")

raw_input("ok?")
name ="chi2"
if opts.scale: name+="_scaled"
c.SaveAs(name+ ".pdf")
c.SaveAs(name+ ".png")


# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
