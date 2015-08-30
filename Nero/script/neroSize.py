
import glob
import sys,os
import re
import array

from subprocess import call,check_output

from optparse import OptionParser

parser=OptionParser()
parser.add_option("-f","--file",dest="file",type="string",help="fileName [Default=%default]",default="root://eoscms///store/user/amarini/Nero/v1.0//RunIISpring15DR74/Asympt50ns_MCRUN2_74_V9A-v2/DYJetsToLL_M-50_TuneCUETP8M1_13TeV-amcatnloFXFX-pythia8/NeroNtuples_10.root")
parser.add_option("-b","--batch",dest="batch",action='store_true',help="batch [Default=%default]",default=False)
parser.add_option("-o","--out",dest="out",type='string',help="output filename [Default=%default]",default='NeroSize')

opts,args = parser.parse_args()

import ROOT



cmd = '''root -b -l  <<EOF 
 TFile *f = TFile::Open("%s");
 f->cd("nero");
 all->Print();
 events->Print();
 .q
EOF''' % (opts.file)

out= check_output (cmd ,shell=True)

### Save Infos
trees={}
entries={}

branches={}

### Navigation
lastTree=""
lastBranch= ""

for line in out.split('\n'):
	if '*********' in line : continue
	if '.........' in line : continue
	#if 'root [' in line : continue

	# remove all spaces and *, leave colum separator
	line = line.replace('*','').replace(' ','')
	## remove empty lines
	if line == '' : continue

	print "Processing line '" + line + "'"

	if 'Tree' in line.split(':')[0]:
		'*Tree    :all       : all                                                    *'
		lastTree=line.split(':')[1]
		lastBranch=""

	if 'Br' in line.split(':')[0]:
		'*Br    0 :isRealData : isRealData/I                                          *'
		lastBranch=line.split(':')[1]
	if 'Entries' in line.split(':')[0]:
		'*Entries :   187542 : Total  Size=     752786 bytes  File Size  =       5882 *'
		ntr  = int(line.split(':')[1])
		size = line.split(':')[2] . split('=')[1].replace(' ','')
		size = re.sub('bytes.*','',size)
		size = int(size)

		filesize = int(line.split(':')[2].split('=')[2].replace(' ',''))

		if lastBranch=="":
			trees[lastTree] = filesize
			entries[lastTree] = ntr
		else:
			branches[ (lastTree,lastBranch ) ] = filesize


ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptTitle(1)

c= ROOT.TCanvas("neroSize","neroSize",800,800)
c.Divide(2,2)


### COMMON COLORS
colorList=[ROOT.kGreen+2, ROOT.kRed+2, ROOT.kBlue, ROOT.kMagenta+2, ROOT.kOrange,
		ROOT.kGreen-7, ROOT.kCyan, ROOT.kRed-7, ROOT.kBlue-7,ROOT.kMagenta,
		ROOT.kYellow, ROOT.kViolet, ROOT.kGray, ROOT.kRed, ROOT.kCyan+2,
		]
colors=array.array('i', colorList )

def CreatePieTree(trees):
	valueList=[]
	labelList=[]

	for idx,elem in enumerate(trees):
		valueList.append( trees[elem] )
		labelList.append( elem )

	values=array.array('f', valueList)

	pie = ROOT.TPie("treepie","Trees", len(valueList) ,values, colors)

	for entry in range(0,len( valueList) ):
		pie.SetEntryLineColor(entry, ROOT.kBlack)
		pie.SetEntryLineWidth(entry, ROOT.kBlack)
		pie.SetEntryLabel(entry, labelList[entry] )

	pie.SetCircle(0.5,.45,0.3)
	return pie

def CreatePieBranch(branches, treename="all", collapse=True):
	valueList=[]
	labelList=[]

	for tree, branch in branches:
		if tree != treename : continue

		original = branch[:]

		if collapse:
			## collapse event info in one entry in the pie chart
			if treename != "all" and (branch == 'runNum'  \
				or branch =='eventNum'  \
				or branch == 'lumiNum'  \
				or branch == 'mcWeight' \
				or branch == 'puTrueInt'   \
				or branch == 'isRealData'   \
				or branch == 'rho'   \
				or branch == 'npv'   \
				):
				branch = 'info'
			#from first capital letter
			branch= re.sub('[A-Z].*','', branch)
			#_*
			branch= re.sub('_.*','', branch)
			if 'ak8' in branch:
				branch='fatjet'

		if branch in labelList:
			idx = labelList.index( branch )
			valueList[idx] += branches[(treename,original)]
		else:
			valueList.append( branches[(treename,original)] )
			labelList.append(branch)

	values = array.array('f', valueList)
	pie = ROOT.TPie("branches_%s"%treename, treename, len(valueList) ,values, colors)

	for entry in range(0,len( valueList) ):
		pie.SetEntryLineColor(entry, ROOT.kBlack)
		pie.SetEntryLineWidth(entry, ROOT.kBlack)
		pie.SetEntryLabel(entry, labelList[entry] )

	pie.SetCircle(0.5,.45,0.3)
	return pie

## TreeSize
c.cd(1)

pie1 = CreatePieTree(trees)
pie1 . Draw("rcs")

ltx=ROOT.TLatex()
ltx.SetNDC()
ltx.SetTextSize(0.03)
ltx.SetTextAlign(12)

for idx,elem in enumerate(trees):
	ltx.DrawLatex( 0.15, 0.20 + idx*0.05 , "%s: %.3f b/e" % (elem, float(trees[elem])/float(entries[elem]) ) )

pies=[]
for idx,elem in enumerate(trees):
	c.cd(2+ idx)
	pie=CreatePieBranch(branches,elem,True)
	pies.append(pie) ## garbage collector
	pie.Draw("rcs")

c.Update()

if not opts.batch: raw_input("Looks ok ? ")

if opts.out != "":
	c.SaveAs(opts.out + ".png")
	c.SaveAs(opts.out + ".pdf")
