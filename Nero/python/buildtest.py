import os,sys
import re

import json
import requests

from optparse import OptionParser, OptionGroup
from subprocess import call,check_output

parser = OptionParser(usage = "usage");
parser.add_option("-y","--yes",dest="yes",type="int",help="Should I assume yes as an answer. [0-3],0 = ask, 1= almost always, 3= yes (dangerous).",default=0);
parser.add_option("-t","--token",dest="token",type="string",help="token [Default=%default]",default="");
parser.add_option("-f","--token-file",dest="tokenfile",type="string",help="tokenfile [Default=%default]",default=os.environ["HOME"]+"/.ssh/id_status_token");
parser.add_option("-o","--override",dest="override",type="string",help="setup file to be parser instead of the PR one [Default=%default]",default="");
parser.add_option("","--pr",dest="pr",type="string",help="PR number to force check [Default=%default]",default="");

(opts,args) = parser.parse_args()

mystatustoken=opts.token ### PWD

if mystatustoken == '' :
	#token = open(os.environ["HOME"]+"/.ssh/id_status_token")
	token = open( opts.tokenfile)
	for l in token:
		l=l.split('#')[0]
		l=l.replace('\n','')
		if l !="":
			mystatustoken=l
			break;
	token.close()


repo= 'MiT-HEP/NeroProducer'
tag = ''
url = 'https://api.github.com/repos/' + repo
tmpdir="/tmp/" + os.environ['USER']

## bash color string
red="\033[01;31m"
green = "\033[01;32m"
yellow = "\033[01;33m"
blue = "\033[01;34m"
pink = "\033[01;35m"
cyan = "\033[01;36m"
white = "\033[00m"

def GetPullReqList():
	dict={}
	myreq="/pulls"
	params={"state":"open"}
	r = requests.get(url+myreq,params=params)
	for pr in r.json():
		print "*", str(pr['number']) + ":", pr['title']
		dict[str(pr['number'])] = {"title":pr["title"],
				"sha":pr['head']['sha'],
				"origin":pr['head']['repo']['full_name'],
				"mergeable" : pr['merge_commit_sha'], ## FIXME, will become mergeable in the future
				}
		if not dict[str(pr['number'])]['mergeable']:
			print "\t"+red + "CONFLICT" + white
		else:
			print "\t" +  yellow + "TOCHECK" + white

	return dict	

def TryPullReq(sha, origin):
	## create the place where store the output
	CMSSW="CMSSW_7_4_5" ## some default
	tag = ''
	cmd = "mkdir -p %s" % os.environ["HOME"]+"/www/%s/"%(repo.split('/')[1]) + sha + "/"
	call(cmd,shell=True)

	## download setup.sh, parse it
	#https://raw.githubusercontent.com/amarini/NeroProducer/17006845ca21076e6f6966b4576dd228f9d4555c/Nero/script/setup.sh
	dep ="rm %s/setup.sh ; wget --no-check-certificate 'https://raw.githubusercontent.com/%s/%s/Nero/script/setup.sh' -O %s/setup.sh"% (tmpdir,origin,sha,tmpdir)
	print "Calling",dep ##DEBUG
	call (dep,shell=True)
	if opts.override != "":
		print cyan+"-> Replace setup.sh"+white
		cmd = "cp -v %s %s/setup.sh"%(opts.override,tmpdir)
		call(cmd,shell=True)
	setup =open("%s/setup.sh"%tmpdir)
	dangerous = re.compile('[$`;!]')
	options=""
	flStr=""
	maxEvtStr=""
	for line in setup:
		if '[CMSSW]' in line and not dangerous.search(line): 
			# line is of the form # [CMSSW] CMSSW_release
			parts = line.split()
			for i in range(0, len(parts) -1 ) :
				if parts[i] == "[CMSSW]":
					CMSSW=parts[i+1]
					print "-> Setting CMSSW to ", CMSSW
		if '[Tag]' in line and not dangerous.search(line): 
			# line is of the form # [Tag] Tag_release
			parts = line.split()
			for i in range(0, len(parts) -1 ) :
				if parts[i] == "[Tag]":
					tag=parts[i+1]
					print "-> Setting Tag to ", tag
		if '[Options]' in line and not dangerous.search(line):
			parts= line.split()
			for i in range(0,len(parts)-1):
				if parts[i] == '[Options]':
					options = " ".join(parts[i+1:])

		if '[fileList]' in line and not dangerous.search(line):
			parts = line.split()
			for i in range(0, len(parts) -1 ) :
				if parts[i] == "[fileList]":
					inStr=parts[i+1]
			fl = [ '"' + f + '"' for f in inStr.split(',') ]
			#flStr = "fileList = [ "+ ','.join(fl) + "]";
			#flStr = "options.inputFiles = [ "+ ','.join(fl) + "]";
			options += ' inputFiles=' + " ".join(inStr.split(','))
			#replace
			#cmd = "sed -i'' 's:###FILELIST###:"+flStr+":g' " + pset 
			#call(cmd,shell=True)
		if '[MaxEvents]' in line and not dangerous.search(line):
			parts = line.split()
			for i in range(0, len(parts) -1 ) :
				if parts[i] == "[MaxEvents]":
					maxEvtStr=parts[i+1]
			options += ' maxEvents=' + maxEvtStr

		l = line.split('#')[0]
		l = re.sub('\n','',l)
		l = re.sub('^\ *','',l)
		l = re.sub('\ *$','',l)
		l = re.sub('^\t*','',l)
		l = re.sub('\t*$','',l)
		if l== "": continue
		if l.startswith('git cms-merge-topic') and not dangerous.search(l)  : continue
		if l.startswith('function') and not dangerous.search(l) : continue
		if l.startswith('git cms-init') and not dangerous.search(l)  : continue
		if l.startswith('echo') and not dangerous.search(l)  : continue
		if l.startswith('git') and not dangerous.search(l)  : continue
		if l.startswith('mv RecoEgamma') and not dangerous.search(l)  : continue
		if l.startswith('rsync') and not dangerous.search(l)  : continue
		if l == '$1' : continue # execute argument, checked that CMSSW release is not dangerous
		if l.startswith('[ "X$1" == "X" ] && $1=$CMSSW_VERSION') : continue # execute argument, checked that CMSSW release is not dangerous
		if l == 'true' : continue # execute argument, checked that CMSSW release is not dangerous
		if l == '}' : continue # end function
		print "potential dangerous line:"
		print "\t'"+ l + "'"
		if opts.yes<3:  raw_input("is_ok?")
	setup.close()

	print cyan+"-> Removing CMSSW"+white
	cmd = "cd %s &&"% tmpdir	
	cmd += "rm -rf %s/%s 2>/dev/null"%(tmpdir,CMSSW)
	cmd += " || true" #ignore failure
	status = call(cmd,shell=True)

	## cd in cmssw and cmsenv
	cmsenv="eval `scramv1 runtime -sh`"
	cmd = "cd "+ tmpdir +"; "
	## cmsrel
	cmd +="/cvmfs/cms.cern.ch/common/scramv1 project CMSSW  %s && "%CMSSW #cmsrel
	cmd +="cd %s/src &&" % CMSSW

	cmd += cmsenv+' && '
	cmd += 'source %s/setup.sh %s '%(tmpdir,CMSSW)

	print cyan+"-> Setting up scram area"+ white
	status = call(cmd,shell=True)
	if status >0 : 
		print red +"ERROR: "+white + "unable to set up scram area"
		print "\t'"+cmd+"'"
		return 1

	print cyan+"-> Setting up repo"+white
	cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
	cmd += "git clone git@github.com:"+repo
	status = call(cmd,shell=True)
	if status >0 : 
		print red +"ERROR: "+white + "unable to set up repo"
		print "\t'"+cmd+"'"
		return 2

	if tag !='': 
	    print cyan+"-> Checking out ",tag
	    cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
	    cmd += "cd " + origin.split('/')[1] + " && "
	    cmd += " git checkout "+ tag
	    status= call (cmd,shell=True)
	    if status >0 : 
	        print red +"ERROR: "+white + "unable to checkout "+ tag
	        print "\t'"+cmd+"'"
	        return 2

	print cyan+"-> Merging PR"+white
	cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
	cmd += "cd " + origin.split('/')[1] + " && "
	cmd += "git remote add pr git@github.com:%s && "%origin
	cmd += "git fetch pr && "
	cmd += "git merge --no-ff -m 'merge commit' %s  "%sha
	status = call(cmd,shell=True)
	if status >0 : 
		print red +"ERROR: "+white + "unable to merge"
		print "\t'"+cmd+"'"
		return 3

	print cyan+"-> Build"+white
	cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
	cmd += "scram b -j 16 2>&1 | tee %s "%  (os.environ["HOME"]+"/www/%s/"%(repo.split('/')[1]) + sha + "/build.txt")
	cmd += "; EXIT=${PIPESTATUS[0]};  echo \"<-> EXIT STATUS is ${EXIT}\" ; exit $EXIT ; "

	ntry=0
	status = -1
	#try to build 3 times. gmake crashes sometimes
	while status != 0 or ntry <3: 
		status = call( cmd ,shell=True)
		ntry += 1

	if status >0 : 
		print red +"ERROR: "+white + "unable to build"
		print "\t'"+cmd+"'"
		return 4
	
	print cyan+"-> Replacing"+white
	if flStr != "":
		#replace
		print ' * str=' + '"'+ flStr + '"'
		cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
		cmd += "cd NeroProducer/Nero/test && "
		#cmd += "sed -i'' 's:###FILELIST###:"+flStr+":g' " + "testNero.py"
		cmd += "sed -i'' 's:###FILELIST###:"+flStr+"\\n###FILELIST###:g' " + "testNero.py"
		call(cmd,shell=True)

	#if maxEvtStr != "":
	#	#replace
	#	print ' * str=' + '"'+ maxEvtStr + '"'
	#	rplStr="process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(%s) )"%maxEvtStr
	#	cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
	#	cmd += "cd NeroProducer/Nero/test && "
	#	cmd += "sed -i'' 's:###FILELIST###:"+rplStr+":g' " + "testNero.py"
	#	call(cmd,shell=True)

	print cyan+"-> Test"+white
	cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
	cmd += "cd NeroProducer/Nero/test && "
	cmd += "cmsRun testNero.py %s 2>&1 | tee %s "%( options, os.environ["HOME"]+"/www/%s/"%(repo.split('/')[1]) + sha + "/run.txt")
	cmd += "; EXIT=${PIPESTATUS[0]};  echo \"<-> EXIT STATUS is ${EXIT}\" ; exit $EXIT ; "
	status = call( cmd , shell=True ) 
	if status >0 : 
		print red +"ERROR: "+white + "unable to run"
		print "\t'"+cmd+"'"
		return 5
	
	print cyan+"-> Running Core Test"+white
	cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)
	cmd += "cd NeroProducer/Nero/script &&"
	cmd += "python testUnit.py -c -f ../test/NeroNtuples.root | tee %s "% ( os.environ["HOME"]+"/www/%s/"%(repo.split('/')[1]) + sha + "/core.txt" )
	cmd += "; EXIT=${PIPESTATUS[0]};  echo \"<-> EXIT STATUS is ${EXIT}\" ; exit $EXIT ; "
	status = call(cmd, shell=True)

	if status >0 :
		print red + "ERROR: "+ white + " core is broken"
		print "\t'"+cmd+"'"
		return 6

	print cyan+"-> Producing NeroSize" + white
	cmd = "cd %s/%s/src && %s &&" %(tmpdir,CMSSW,cmsenv)  
	cmd += "cd NeroProducer/Nero/ && "
	cmd += "python script/neroSize.py -f test/NeroNtuples.root -b -o size &&"
	cmd += "cp -v size.* %s/" %( os.environ["HOME"]+"/www/%s/"%(repo.split('/')[1]) + sha ) 
	status = call( cmd, shell=True)
	if status >0 :
		print red + "ERROR: "+ white + " unable to produce NeroSize"
		print "\t'"+cmd+"'"
		return 7

	print cyan+"-> Removing CMSSW"+white
	cmd = "cd %s &&"% tmpdir	
	cmd += "rm -rf %s"%CMSSW
	cmd += " || true" #ignore failure
	status = call(cmd,shell=True)
	cmd = " rm %s/setup.sh " % tmpdir
	status = call(cmd,shell=True)

	return 0

def GetStatus(sha):
	mystring="/statuses/"+sha 
	# GET /repos/:owner/:repo/statuses/:ref
	# or /repos/:owner/:repo/commits/:ref/statuses
	#Statuses are returned in reverse chronological order. The first status in the list will be the latest one.
	print "get:"+url+mystring
	r = requests.get(url+mystring)
	return r.json()

def  SetStatus(sha,state="success",description="build",ext='txt'):
	print pink+"<-> Setting Status:"+white,description,state, " to ", sha
	for key in GetStatus(sha):
		if key['context'] == description and key['state'] == state: 
			print " * "+sha+" has already status", key['context']
			print "\t with state",key['state']
			print "\t created by",key["creator"]["login"]
			return

	payload= {"state" :state, ## pending failure error
		"target_url":"https://%(USER)s.web.cern.ch/%(USER)s/%(REPONAME)s/%(SHA)s/%(CONTX)s.%(EXT)s"%{'USER':os.environ['USER'],
					'SHA': sha,
					'CONTX': description,
					'REPONAME': repo.split('/')[1],
					'EXT':ext
					},
		 "description":description,
		 "context":description
		 }
	authstring='?access_token=%s'%(mystatustoken)
	mystring = "/statuses/%s"%sha
	r = requests.post(url+mystring+authstring,data=json.dumps(payload))
	## print r.headers ## DEBUG
	## print r.json()


if __name__ == "__main__":
	dict = GetPullReqList()
	for id in dict:
		if not dict[id]['mergeable']: continue
		## check 'build' 'run' and 'core'
		available={}
		for key in GetStatus(dict[id]['sha']):
			if key['state'] == 'success' :  key['state'] = green + key['state'] + white
			elif key['state'] == 'pending' :  key['state'] = yellow + key['state'] + white
			else : key['state'] = red+ key['state'] + white

			# key are ordered
			if key['description'] not in available:
				available[key['description'] ] = key['state']

		if 'run' in available and 'build' in available and 'core' in available: ## 
			print "PR:" +dict[id]["title"]+ " already checked:"
			print "\t* build: " + available['build'] 
			print "\t* run: "+ available['run'] 
			print "\t* core: "+ available['core'] 
			if 'size' in available: print "\t* size: "+ available['size'] 
			if id != opts.pr:
				continue

		## if failed to build, continue
		if 'build' in available and ('success' not in available['build'] and 'pending' not in available['build']): 
			print "PR:" +dict[id]["title"]+ " already checked:"
			print "\t* build: " + available['build'] 
			if opts.yes<1:
				print 
				ans=raw_input("Do you want to re-test Pull Req. " +id+": "+ dict[id]['title'] + "? [y/n]" )
				if ans.lower() != "y"  and ans.lower() != "yes" :  continue;
			elif id != opts.pr:
			   continue

		## if failed to run, continue
		if 'run' in available and 'success' not in available['run']:
			print "PR:" +dict[id]["title"]+ " already checked:"
			if 'build' in available: print "\t* build: " + available['build'] 
			print "\t* run: " + available['run'] 
			if opts.yes<1:
				print 
				ans=raw_input("Do you want to re-test Pull Req. " +id+": "+ dict[id]['title'] + "? [y/n]" )
				if ans.lower() != "y"  and ans.lower() != "yes" :  continue;
			elif id != opts.pr:
			   continue

		## if failed to core, continue
		if 'core' in available and 'success' not in available['core']: 
			print "PR:" +dict[id]["title"]+ " already checked:"
			if 'build' in available: print "\t* build: " + available['build'] 
			if 'run' in available: print "\t* run: " + available['run'] 
			print "\t* core: " + available['core'] 
			if opts.yes<1:
				print 
				ans=raw_input("Do you want to re-test Pull Req. " +id+": "+ dict[id]['title'] + "? [y/n]" )
				if ans.lower() != "y"  and ans.lower() != "yes" :  continue;
			elif id != opts.pr:
			   continue

		if opts.yes<1:
			ans=raw_input("Do you want to test Pull Req. " +id+": "+ dict[id]['title'] + "? [y/n]" )
			if ans.lower() != "y"  and ans.lower() != "yes" :  continue;

		## push a work in progress Status, for building
		SetStatus( dict[id]['sha'],'pending','build')

		status=TryPullReq(dict[id]['sha'], dict[id]['origin'])

		if status == 1: #scram
			pass
		if status == 2: #repo
			pass
		if status == 3: #merge
			pass
		if status == 4: #build
			SetStatus(dict[id]['sha'],'error','build')
			continue
		else:
			SetStatus(dict[id]['sha'],'success','build')

		if status == 5: #run
			SetStatus(dict[id]['sha'],'error','run')
			continue
		else:
			SetStatus(dict[id]['sha'],'success','run')

		if status == 6: #testcore
			SetStatus(dict[id]['sha'],'error','core')
			continue
		else:
			SetStatus(dict[id]['sha'],'success','core')

		if status == 7: #testcore
			SetStatus(dict[id]['sha'],'error','size','png')
			continue
		else:
			SetStatus(dict[id]['sha'],'success','size','png')

		if status == 0:
			print id+":", dict[id]["title"] + ":"
			print "*** "+ green +"DONE" + white +" ***"


