import os,sys

import time

from optparse import OptionParser, OptionGroup
from subprocess import call,check_output

from NeroProducer.Nero.GitHubHandler import *

parser = OptionParser(usage = "this macro produces an html page with a summary of the merged pull req. tags and associated labels.");
parser.add_option("-r","--repo",dest="repo",type="string",help="Repository [Default=%default]",default="MiT-HEP/NeroProducer");

(opts,args) = parser.parse_args()


owner="MiT-HEP"
repo= 'NeroProducer'
url = 'https://api.github.com/repos/' + repo
tmpdir="/tmp/" + os.environ['USER']

## bash color string
class colors:
	def __init__(self):
		self.red="\033[01;31m"
		self.green = "\033[01;32m"
		self.yellow = "\033[01;33m"
		self.blue = "\033[01;34m"
		self.pink = "\033[01;35m"
		self.cyan = "\033[01;36m"
		self.white = "\033[00m"
color= colors()



## ############################ MAIN ################################
### maybe just take the list of commits that corresponds to a merging of a pull req ? 

if __name__ == "__main__":
	github= GitHubHandler("MiT-HEP","NeroProducer")
	github.read_token_fromfile("/afs/cern.ch/user/a/amarini/.ssh/id_status_token")

	print "----------- OPEN PULL REQUEST -----------"
	github.get_pullreq()
	#for pr in github.pulls:
	#	print "* " + color.red + str( pr.number) + color.white + ':  "'+ pr.title + '"'
	#	print "\t\t . " + pr.sha
	#	print "\t\t . merged=", pr.merged, "mergeable=", pr.mergeable
	#	print "\t\t . labels=",
	#	for l in pr.labels:
	#		print l,
	#	print 

	print "----------- CLOSED PULL REQUEST -----------"
	github.get_pullreq('closed')
	#for pr in github.pulls:
	#	print "* " + color.red + str( pr.number) + color.white + ':  "'+ pr.title + '"'
	#	print "\t\t . " + pr.sha
	#	print "\t\t . merged=", pr.merged, "mergeable=", pr.mergeable
	#	print "\t\t . labels=",
	#	for l in pr.labels:
	#		print l,
	#	print 

	print "----------- TAGS -----------"
	github.get_tags()
	for tag in github.tags:
		print color.green + tag.name + color.white +": " + tag.sha

	time.sleep(1)
	print "----------- COMMITS -----------"
	github.get_commits()
	for sha in github.commits:	
		cm=github.commits[sha]
		if cm.date != None:
			print color.yellow + cm.date.strftime("%d %b %Y, %H:%M") + color.white + " :\t" + cm.sha
		else:
			print color.yellow + "None" + color.white + " :\t" + cm.sha

	objects = {} ## dictionary to order commits, pullreq and tags

	# tag -> sha -> commit
	# pullReq -> sha ->  commit -> label
	print "-------------- SUMMARY ----------------"

	commits={}
	## order commits by date
	for sha in github.commits :
		cm = github.commits[sha]
		commits[ sha ] = (cm,[])

	for tag in github.tags	:
		if tag.sha not in commits:
			print "tag",tag.name,"not in commits"
			continue
		commits[ tag.sha ][1].append( color.green + tag.name + color.white) 
	for pr in github.pulls: ## CLOSED PR
		if pr.sha not in commits:
			print "* " + color.red + str( pr.number) + color.white + ':  "'+ pr.title + '" not in commits'
			continue
		descr= color.cyan + str(pr.number) + color.white + ": " + pr.title + " | " 
		for l in pr.labels:
			descr +=  color.yellow +  l + color.white + " "
		commits[ pr.sha ] [1] .append( descr )  
	#make the dictionary a list
	lcommits=[]
	for sha in commits:
		lcommits.append ( commits[sha] )
	# sort accortingly to date
	lcommits.sort()

	for cm,l in lcommits:
		print "* "+ color.yellow + str(cm.date) + color.white,
		print cm.sha + ":",
		for elem in l :
			print elem,
		print 


